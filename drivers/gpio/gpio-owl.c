// SPDX-License-Identifier: GPL-2.0+
//
// OWL SoC's GPIO driver
//
// Copyright (c) 2014 Actions Semi Inc.
// Author: David Liu <liuwei@actions-semi.com>
//
// Copyright (c) 2018 Linaro Ltd.
// Author: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>

#include <linux/bitops.h>
#include <linux/err.h>
#include <linux/gpio/driver.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#define GPIO_OUTEN	0x0000
#define GPIO_INEN	0x0004
#define GPIO_DAT	0x0008

#define OWL_GPIO_PORT_A 0
#define OWL_GPIO_PORT_B 1
#define OWL_GPIO_PORT_C 2
#define OWL_GPIO_PORT_D 3
#define OWL_GPIO_PORT_E 4
#define OWL_GPIO_PORT_F 5

struct owl_gpio_port {
	const char *name;
	unsigned int offset;
	unsigned int pins;
};

struct owl_gpio_soc {
	const struct owl_gpio_port *ports;
	unsigned int num_ports;
	const char *name;
};

struct owl_gpio {
	struct gpio_chip gpio;
	const struct owl_gpio_soc *soc;
	void __iomem *base;
};

static void __iomem *owl_gpio_get_base(struct owl_gpio *gpio,
				       unsigned int *pin)
{
	unsigned int start = 0, i;

	for (i = 0; i < gpio->soc->num_ports; i++) {
		const struct owl_gpio_port *port = &gpio->soc->ports[i];

		if (*pin >= start && *pin < start + port->pins) {
			*pin -= start;
			return (gpio->base + port->offset);
		}

		start += port->pins;
	}

	return NULL;
}

static int owl_gpio_request(struct gpio_chip *chip, unsigned int offset)
{
	struct owl_gpio *gpio = gpiochip_get_data(chip);
	void __iomem *gpio_base = owl_gpio_get_base(gpio, &offset);
	u32 val;

	/*
	 * GPIOs have higher priority over other modules, so either setting
	 * them as OUT or IN is sufficient
	 */
	val = readl(gpio_base + GPIO_OUTEN);
	val |= BIT(offset);
	writel(val, gpio_base + GPIO_OUTEN);

	return 0;
}

static void owl_gpio_free(struct gpio_chip *chip, unsigned int offset)
{
	struct owl_gpio *gpio = gpiochip_get_data(chip);
	void __iomem *gpio_base = owl_gpio_get_base(gpio, &offset);
	u32 val;

	/* disable gpio output */
	val = readl(gpio_base + GPIO_OUTEN);
	val &= ~BIT(offset);
	writel(val, gpio_base + GPIO_OUTEN);

	/* disable gpio input */
	val = readl(gpio_base + GPIO_INEN);
	val &= ~BIT(offset);
	writel(val, gpio_base + GPIO_INEN);
}

static int owl_gpio_get(struct gpio_chip *chip, unsigned int offset)
{
	struct owl_gpio *gpio = gpiochip_get_data(chip);
	void __iomem *gpio_base = owl_gpio_get_base(gpio, &offset);
	u32 val;

	val = readl(gpio_base + GPIO_DAT);

	return !!(val & BIT(offset));
}

static void owl_gpio_set(struct gpio_chip *chip, unsigned int offset, int value)
{
	struct owl_gpio *gpio = gpiochip_get_data(chip);
	void __iomem *gpio_base = owl_gpio_get_base(gpio, &offset);
	u32 val;

	val = readl(gpio_base + GPIO_DAT);

	if (value)
		val |= BIT(offset);
	else
		val &= ~BIT(offset);

	writel(val, gpio_base + GPIO_DAT);
}

static int owl_gpio_direction_input(struct gpio_chip *chip, unsigned int offset)
{
	struct owl_gpio *gpio = gpiochip_get_data(chip);
	void __iomem *gpio_base = owl_gpio_get_base(gpio, &offset);
	u32 val;

	val = readl(gpio_base + GPIO_OUTEN);
	val &= ~BIT(offset);
	writel(val, gpio_base + GPIO_OUTEN);

	val = readl(gpio_base + GPIO_INEN);
	val |= BIT(offset);
	writel(val, gpio_base + GPIO_INEN);

	return 0;
}

static int owl_gpio_direction_output(struct gpio_chip *chip,
				unsigned int offset, int value)
{
	struct owl_gpio *gpio = gpiochip_get_data(chip);
	unsigned int pin = offset;
	void __iomem *gpio_base = owl_gpio_get_base(gpio, &pin);
	u32 val;

	val = readl(gpio_base + GPIO_INEN);
	val &= ~BIT(pin);
	writel(val, gpio_base + GPIO_INEN);

	val = readl(gpio_base + GPIO_OUTEN);
	val |= BIT(pin);
	writel(val, gpio_base + GPIO_OUTEN);

	owl_gpio_set(chip, offset, value);

	return 0;
}

static int owl_gpio_probe(struct platform_device *pdev)
{
	struct owl_gpio *gpio;
	int ret, i;

	gpio = devm_kzalloc(&pdev->dev, sizeof(*gpio), GFP_KERNEL);
	if (!gpio)
		return -ENOMEM;

	gpio->soc = of_device_get_match_data(&pdev->dev);

	gpio->base = of_iomap(pdev->dev.of_node, 0);
	if (IS_ERR(gpio->base))
		return PTR_ERR(gpio->base);

	gpio->gpio.request = owl_gpio_request;
	gpio->gpio.free = owl_gpio_free;
	gpio->gpio.get = owl_gpio_get;
	gpio->gpio.set = owl_gpio_set;
	gpio->gpio.direction_input = owl_gpio_direction_input;
	gpio->gpio.direction_output = owl_gpio_direction_output;

	gpio->gpio.base = -1;
	gpio->gpio.parent = &pdev->dev;
	gpio->gpio.label = gpio->soc->name;
	gpio->gpio.of_node = pdev->dev.of_node;

	for (i = 0; i < gpio->soc->num_ports; i++)
		gpio->gpio.ngpio += gpio->soc->ports[i].pins;

	platform_set_drvdata(pdev, gpio);

	ret = devm_gpiochip_add_data(&pdev->dev, &gpio->gpio, gpio);
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to register gpiochip\n");
		return ret;
	}

	pr_info("Initialized Actions OWL gpio driver\n");

	return 0;
}

static int owl_gpio_remove(struct platform_device *pdev)
{
	return 0;
}

#define OWL_GPIO_PORT(port, base, count)		\
	[OWL_GPIO_PORT_##port] = {			\
		.name = #port,				\
		.offset = base,				\
		.pins = count,				\
	}

static const struct owl_gpio_port s900_gpio_ports[] = {
	OWL_GPIO_PORT(A, 0x0000, 32),
	OWL_GPIO_PORT(B, 0x000C, 32),
	OWL_GPIO_PORT(C, 0x0018, 12),
	OWL_GPIO_PORT(D, 0x0024, 30),
	OWL_GPIO_PORT(E, 0x0030, 32),
	OWL_GPIO_PORT(F, 0x00F0, 8),
};

static const struct owl_gpio_soc s900_gpio_soc = {
	.num_ports = ARRAY_SIZE(s900_gpio_ports),
	.ports = s900_gpio_ports,
	.name = "s900-gpio",
};

static const struct of_device_id owl_gpio_of_match[] = {
	{ .compatible = "actions,s900-gpio", .data = &s900_gpio_soc },
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, owl_gpio_of_match);

static struct platform_driver owl_gpio_driver = {
	.driver		= {
		.name	= "owl-gpio",
		.of_match_table = owl_gpio_of_match,
	},
	.probe		= owl_gpio_probe,
	.remove		= owl_gpio_remove,
};
module_platform_driver(owl_gpio_driver);

MODULE_AUTHOR("David Liu <liuwei@actions-semi.com>");
MODULE_AUTHOR("Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>");
MODULE_DESCRIPTION("Actions OWL SoCs GPIO driver");
MODULE_LICENSE("GPL v2");
