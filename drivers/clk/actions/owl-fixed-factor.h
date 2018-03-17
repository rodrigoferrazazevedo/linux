// SPDX-License-Identifier: GPL-2.0+
//
// OWL fixed factor clock driver
//
// Copyright (c) 2014 Actions Semi Inc.
// Author: David Liu <liuwei@actions-semi.com>
//
// Copyright (c) 2018 Linaro Ltd.
// Author: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>

#ifndef _OWL_FIXED_FACTOR_H_
#define _OWL_FIXED_FACTOR_H_

#include "owl-common.h"

struct owl_fix_fact_hw {
	unsigned int		mul;
	unsigned int		div;
};

struct owl_fix_fact {
	struct owl_fix_fact_hw	fix_fact_hw;
	struct owl_clk_common	common;
};

#define OWL_FIX_FACT_HW(_mul, _div)					\
	{								\
		.mul		= _mul,					\
		.div		= _div,					\
	}

#define OWL_FIX_FACT(_struct, _name, _parent, _mul, _div, _flags)	\
	struct owl_fix_fact _struct = {					\
		.fix_fact_hw = OWL_FIX_FACT_HW(_mul, _div),		\
		.common = {						\
			.regmap		= NULL,				\
			.hw.init	= CLK_HW_INIT(_name,		\
						      _parent,		\
						      &owl_fix_fact_ops,\
						      _flags),		\
		},							\
	}

static inline struct owl_fix_fact *hw_to_owl_fix_fact(const struct clk_hw *hw)
{
	struct owl_clk_common *common = hw_to_owl_clk_common(hw);

	return container_of(common, struct owl_fix_fact, common);
}

long owl_fix_fact_helper_round_rate(struct owl_clk_common *common,
				const struct owl_fix_fact_hw *fix_fact_hw,
				unsigned long rate,
				unsigned long *parent_rate);

unsigned long owl_fix_fact_helper_recalc_rate(struct owl_clk_common *common,
				const struct owl_fix_fact_hw *fix_fact_hw,
				unsigned long parent_rate);

extern const struct clk_ops owl_fix_fact_ops;

#endif /* _OWL_FIXED_FACTOR_H_ */
