/*
 * Actions OWL SoCs GPIO driver
 *
 * Copyright (c) 2014 Actions Semi Inc.
 * Author: David Liu <liuwei@actions-semi.com>
 *
 * Copyright (c) 2017 Linaro Ltd.
 * Author: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 */

#include <linux/bitops.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio/driver.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/pinctrl/consumer.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#define GPIO_PER_BANK (32)

enum owl_gpio_id {
	S900_GPIO,
};

struct gpio_reg_info {
	unsigned long outen;
	unsigned long inen;
	unsigned long data;
};

struct owl_gpio_bank_data {
	int ngpio;
	const struct gpio_reg_info info;
};

struct owl_gpio {
	void __iomem *base;
	int id;
	int ngpio;
	enum owl_gpio_id dev_id;
	struct gpio_chip gc;
	const struct gpio_reg_info *info;
	spinlock_t lock;	
};

static inline int is_s900_gpio(struct owl_gpio *gpio)
{
	return gpio->dev_id == S900_GPIO;
}

static int owl_gpio_request(struct gpio_chip *chip, unsigned offset)
{
	return pinctrl_request_gpio(chip->base + offset);
}

static inline struct owl_gpio *to_owl_gpio(struct gpio_chip *chip)
{
	return container_of(chip, struct owl_gpio, gc);
}

static void owl_gpio_free(struct gpio_chip *chip, unsigned offset)
{
	struct owl_gpio *gpio = to_owl_gpio(chip);
	const struct gpio_reg_info *info = gpio->info;
	unsigned long flags = 0;
	u32 val;

	spin_lock_irqsave(&gpio->lock, flags);

	/* disable gpio output */
	val = readl(gpio->base + info->outen);
	val &= ~BIT(offset);
	writel(val, gpio->base + info->outen);

	/* disable gpio input */
	val = readl(gpio->base + info->inen);
	val &= ~BIT(offset);
	writel(val, gpio->base + info->inen);

	spin_unlock_irqrestore(&gpio->lock, flags);

	pinctrl_free_gpio(chip->base + offset);
}

static int owl_gpio_get(struct gpio_chip *chip, unsigned offset)
{
	struct owl_gpio *gpio = to_owl_gpio(chip);
	const struct gpio_reg_info *info = gpio->info;
	u32 val;

	val = readl(gpio->base + info->data);

	pr_info("%s pin: %d regval: %u\n", __func__, offset, val);
	return !!(val & BIT(offset));
}

static void owl_gpio_set(struct gpio_chip *chip, unsigned offset, int value)
{
	struct owl_gpio *gpio = to_owl_gpio(chip);
	const struct gpio_reg_info *info = gpio->info;
	u32 val;

	val = readl(gpio->base + info->data);
	pr_info("%s pin: %d regval: %u val: %d\n", __func__, offset, val, value);

	if (value)
		val |= BIT(offset);
	else
		val &= ~BIT(offset);

	writel(val, gpio->base + info->data);
	pr_info("%s pin: %d regval: %u\n", __func__, offset, val);
}

static int owl_gpio_direction_input(struct gpio_chip *chip, unsigned offset)
{
	struct owl_gpio *gpio = to_owl_gpio(chip);
	const struct gpio_reg_info *info = gpio->info;
	unsigned long flags = 0;
	u32 val;

	spin_lock_irqsave(&gpio->lock, flags);

	val = readl(gpio->base + info->outen);
	pr_info("%s pin: %d regval: %u\n", __func__, offset, val);
	val &= ~BIT(offset);
	writel(val, gpio->base + info->outen);

	val = readl(gpio->base + info->inen);
	val |= BIT(offset);
	writel(val, gpio->base + info->inen);

	spin_unlock_irqrestore(&gpio->lock, flags);
	pr_info("%s pin: %d regval: %u\n", __func__, offset, val);

	return 0;
}

static int owl_gpio_direction_output(struct gpio_chip *chip,
		unsigned offset, int value)
{
	struct owl_gpio *gpio = to_owl_gpio(chip);
	const struct gpio_reg_info *info = gpio->info;
	unsigned long flags = 0;
	u32 val;

	spin_lock_irqsave(&gpio->lock, flags);

	val = readl(gpio->base + info->inen);
	pr_info("%s pin: %d regval: %u\n", __func__, offset, val);
	val &= ~BIT(offset);
	writel(val, gpio->base + info->inen);
	pr_info("%s pin: %d regval: %u\n", __func__, offset, val);
	pr_info("%s pin: %d regval: %u\n", __func__, offset, readl(gpio->base + info->inen));
	
	val = readl(gpio->base + info->outen);
	pr_info("%s pin: %d regval: %u\n", __func__, offset, val);
	val |= BIT(offset);
	writel(val, gpio->base + info->outen);
	pr_info("%s pin: %d regval: %u\n", __func__, offset, readl(gpio->base + info->outen));

	owl_gpio_set(chip, offset, value);

	spin_unlock_irqrestore(&gpio->lock, flags);
	pr_info("%s pin: %d regval: %u\n", __func__, offset, val);

	return 0;
}

static void owl_gpio_bank_setup(struct owl_gpio *gpio,
			struct platform_device *pdev)
{
	struct gpio_chip *gpio_chip = &gpio->gc;

	gpio_chip->base = -1;
	gpio_chip->ngpio = gpio->ngpio;
	gpio_chip->request = owl_gpio_request;
	gpio_chip->free = owl_gpio_free;
	gpio_chip->get = owl_gpio_get;
	gpio_chip->set = owl_gpio_set;
	gpio_chip->direction_input = owl_gpio_direction_input;
	gpio_chip->direction_output = owl_gpio_direction_output;
	gpio_chip->owner = THIS_MODULE;
	gpio_chip->parent = &pdev->dev;
	gpio_chip->label = dev_name(&pdev->dev);
}

static const struct owl_gpio_bank_data s900_banks_data[6] = {
	{ 32, {  0x0,    0x4,   0x8, } },
	{ 32, {  0xc,    0x10,  0x14, } },
	{ 12, {  0x18,   0x1c,  0x20, } },
	{ 30, {  0x24,   0x28,  0x2c, } },
	{ 32, {  0x30,   0x34,  0x38, } },
	{ 8,  {  0xf0,   0xf4,  0xf8, } },
};

static const struct of_device_id owl_gpio_dt_ids[] = {
	{ .compatible = "actions,s900-gpio", .data = (void *) S900_GPIO, },
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, owl_gpio_dt_ids);

static int owl_gpio_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	const struct of_device_id *of_devid;
	struct device_node *np = pdev->dev.of_node;
	struct owl_gpio *gpio;
	const struct owl_gpio_bank_data *pdata;
	u32 val;

	gpio = devm_kzalloc(dev, sizeof(*gpio), GFP_KERNEL);
	if (!gpio)
		return -ENOMEM;

        gpio->base = of_iomap(dev->of_node, 0);
        if (IS_ERR(gpio->base))
                return PTR_ERR(gpio->base);

	if (dev->of_node) {
		of_devid = of_match_device(owl_gpio_dt_ids, dev);
		if (of_devid) {
			if (of_devid->data)
				gpio->dev_id = (enum owl_gpio_id) of_devid->data;
		}	
	}

	gpio->id = of_alias_get_id(np, "gpio");
	if (gpio->id < 0)
		return gpio->id;
	
	if (is_s900_gpio(gpio)) {
		BUG_ON(gpio->id >= ARRAY_SIZE(s900_banks_data));
		pdata = &s900_banks_data[gpio->id];
	} else {
		return -ENOENT;
	}

        /* enable pad control */
        val = readl(gpio->base + 0x74);
        val |= BIT(1);
        writel(val, gpio->base + 0x74);

	gpio->ngpio = pdata->ngpio;
	gpio->info = &pdata->info;
	spin_lock_init(&gpio->lock);

	owl_gpio_bank_setup(gpio, pdev);	
	platform_set_drvdata(pdev, gpio);

	pr_info("GPIO%c initialization with address %p\n", 'A' + gpio->id, gpio->base);

	return devm_gpiochip_add_data(&pdev->dev, &gpio->gc, gpio);
}

static struct platform_driver owl_gpio_driver = {
	.probe		= owl_gpio_probe,
	.driver		= {
		.name	= "owl-gpio",
		.of_match_table = owl_gpio_dt_ids,
	},
};

static int __init owl_gpio_init(void)
{
	return platform_driver_register(&owl_gpio_driver);
}
postcore_initcall(owl_gpio_init);

static void __exit owl_gpio_exit(void)
{
	platform_driver_unregister(&owl_gpio_driver);
}
module_exit(owl_gpio_exit);

MODULE_AUTHOR("David Liu <liuwei@actions-semi.com>");
MODULE_AUTHOR("Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>");
MODULE_DESCRIPTION("Actions OWL SoCs GPIO driver");
MODULE_LICENSE("GPL");
