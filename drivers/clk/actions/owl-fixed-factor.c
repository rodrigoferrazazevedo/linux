// SPDX-License-Identifier: GPL-2.0+
//
// OWL fixed factor clock driver
//
// Copyright (c) 2014 Actions Semi Inc.
// Author: David Liu <liuwei@actions-semi.com>
//
// Copyright (c) 2018 Linaro Ltd.
// Author: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>

#include <linux/clk-provider.h>
#include <linux/regmap.h>
#include <linux/slab.h>

#include "owl-fixed-factor.h"

long owl_fix_fact_helper_round_rate(struct owl_clk_common *common,
				const struct owl_fix_fact_hw *fix_fact_hw,
				unsigned long rate,
				unsigned long *parent_rate)
{
	if (clk_hw_get_flags(&common->hw) & CLK_SET_RATE_PARENT) {
		unsigned long best_parent;

		best_parent = (rate / fix_fact_hw->mul) * fix_fact_hw->div;
		*parent_rate = clk_hw_round_rate(clk_hw_get_parent(&common->hw),
							best_parent);
	}

	return (*parent_rate / fix_fact_hw->div) * fix_fact_hw->mul;
}

static long owl_fix_fact_round_rate(struct clk_hw *hw, unsigned long rate,
			unsigned long *parent_rate)
{
	struct owl_fix_fact *fix_fact = hw_to_owl_fix_fact(hw);
	struct owl_fix_fact_hw *fix_fact_hw = &fix_fact->fix_fact_hw;

	return owl_fix_fact_helper_round_rate(&fix_fact->common, fix_fact_hw,
							rate, parent_rate);
}

unsigned long owl_fix_fact_helper_recalc_rate(struct owl_clk_common *common,
			const struct owl_fix_fact_hw *fix_fact_hw,
			unsigned long parent_rate)
{
	unsigned long long int rate;

	rate = (unsigned long long int)parent_rate * fix_fact_hw->mul;
	do_div(rate, fix_fact_hw->div);

	return (unsigned long)rate;
}

static unsigned long owl_fix_fact_recalc_rate(struct clk_hw *hw,
			unsigned long parent_rate)
{
	struct owl_fix_fact *fix_fact = hw_to_owl_fix_fact(hw);
	struct owl_fix_fact_hw *fix_fact_hw = &fix_fact->fix_fact_hw;

	return owl_fix_fact_helper_recalc_rate(&fix_fact->common, fix_fact_hw,
							parent_rate);
}

static int owl_fix_fact_set_rate(struct clk_hw *hw, unsigned long rate,
			       unsigned long parent_rate)
{
	/*
	 * We must report success but we can do so unconditionally because
	 * clk_fix_fact_round_rate returns values that ensure this call is a
	 * nop.
	 */

	return 0;
}

const struct clk_ops owl_fix_fact_ops = {
	.round_rate	= owl_fix_fact_round_rate,
	.recalc_rate	= owl_fix_fact_recalc_rate,
	.set_rate	= owl_fix_fact_set_rate,
};
