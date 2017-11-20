/*
 * Pinctrl definitions for Actions SOC
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

#ifndef __PINCTRL_OWL_H__
#define __PINCTRL_OWL_H__

enum owl_pinconf_pull {
	OWL_PINCONF_PULL_HIZ,
	OWL_PINCONF_PULL_DOWN,
	OWL_PINCONF_PULL_UP,
	OWL_PINCONF_PULL_HOLD,
};

enum owl_pinconf_drv {
	OWL_PINCONF_DRV_2MA,
	OWL_PINCONF_DRV_4MA,
	OWL_PINCONF_DRV_8MA,
	OWL_PINCONF_DRV_12MA,
};

/**
 * struct owl_mfpctl - Actions multiplexing control register
 * @reg: offset to the multiplexing control register
 * @shift: shift value of the register
 * @width: width of the register
 */
struct owl_mfpctl {
	int reg;
	unsigned int shift;
	unsigned int width;
};

/**
 * struct owl_drv - Actions pad drive capacity register
 * @reg: offset to the drive capacity register
 * @shift: shift value of the register
 * @width: width of the register
 */
struct owl_drv {
	int reg;
	unsigned int shift;
	unsigned int width;
};

/**
 * struct owl_pullctl - Actions pad pull control register
 * @reg: offset to the pull control register
 * @shift: shift value of the register
 * @width: width of the register
 */
struct owl_pullctl {
	int reg;
	unsigned int shift;
	unsigned int width;
};

/**
 * struct owl_st - Actions pad schmitt trigger enable register
 * @reg: offset to the schmitt trigger enable register
 * @shift: shift value of the register
 * @width: width of the register
 */
struct owl_st {
	int reg;
	unsigned int shift;
	unsigned int width;
};

/**
 * struct owl_sr - Actions pad slew rate control register
 * @reg: offset to the slew rate control register
 * @shift: shift value of the register
 * @width: width of the register
 */
struct owl_sr {
	int reg;
	unsigned int shift;
	unsigned int width;
};

/**
 * struct owl_pingroup - Actions pingroup definition
 * @name: name of the  pin group
 * @pads: list of pins assigned to this pingroup
 * @npads: size of @pads array
 * @funcs: list of pinmux functions for this pingroup
 * @nfuncs: size of @funcs array
 * @mfpctl_reg: multiplexing control register offset
 * @mfpctl_shift: multiplexing control register bit mask
 * @mfpctl_width: multiplexing control register width
 * @drv_reg: drive control register offset
 * @drv_shift: drive control register bit mask
 * @drv_width: driver control register width
 * @sr_reg: slew rate control register offset
 * @sr_shift: slew rate control register bit mask
 * @sr_width: slew rate control register width
 */
struct owl_pingroup {
	const char *name;
	unsigned int *pads;
	unsigned int npads;
	unsigned int *funcs;
	unsigned int nfuncs;

	int mfpctl_reg;
	unsigned int mfpctl_shift;
	unsigned int mfpctl_width;

	int drv_reg;
	unsigned int drv_shift;
	unsigned int drv_width;

	int sr_reg;
	unsigned int sr_shift;
	unsigned int sr_width;
};

/**
 * struct owl_padinfo - Actions pinctrl pad info
 * @pad: pad name of the SoC
 * @gpio: gpio number of the pad
 * @pullctl: pull control register info
 * @st: schmitt trigger register info
 */
struct owl_padinfo {
	int pad;
	int gpio;
	struct owl_pullctl *pullctl;
	struct owl_st *st;
};

/**
 * struct owl_pinmux_func - Actions pinctrl mux functions
 * @name: name of the pinmux function.
 * @groups: array of pin groups that may select this function.
 * @ngroups: number of entries in @groups.
 */
struct owl_pinmux_func {
	const char *name;
	const char * const *groups;
	unsigned ngroups;
};

/**
 * struct owl_pinctrl_soc_data - Actions pin controller driver configuration
 * @pins: array describing all pins of the pin controller.
 * @npins: number of entries in @pins.
 * @functions: array describing all mux functions of this SoC.
 * @nfunction: number of entries in @functions.
 * @groups: array describing all pin groups of this SoC.
 * @ngroups: number of entries in @groups.
 * @padinfo: array describing the pad info of this SoC.
 * @ngpios: number of pingroups the driver should expose as GPIOs.
 */
struct owl_pinctrl_soc_data {
	const struct pinctrl_pin_desc *pins;
	unsigned int npins;
	const struct owl_pinmux_func *functions;
	unsigned int nfunctions;
	const struct owl_pingroup *groups;
	unsigned int ngroups;
	const struct owl_padinfo *padinfo;
	unsigned int ngpios;
};

int owl_pinctrl_probe(struct platform_device *pdev,
		struct owl_pinctrl_soc_data *soc_data);
int owl_pinctrl_remove(struct platform_device *pdev);

#endif /* __PINCTRL_OWL_H__ */
