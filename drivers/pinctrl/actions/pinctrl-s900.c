// SPDX-License-Identifier: GPL-2.0+
/*
 * OWL S900 Pinctrl driver
 *
 * Copyright (c) 2014 Actions Semi Inc.
 * Author: David Liu <liuwei@actions-semi.com>
 *
 * Copyright (c) 2018 Linaro Ltd.
 * Author: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
 */

#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pinctrl/pinctrl.h>
#include "pinctrl-owl.h"

/* Pinctrl registers offset */
#define MFCTL0			(0x0040)
#define MFCTL1			(0x0044)
#define MFCTL2			(0x0048)
#define MFCTL3			(0x004C)
#define PAD_PULLCTL0		(0x0060)
#define PAD_PULLCTL1		(0x0064)
#define PAD_PULLCTL2		(0x0068)
#define PAD_ST0			(0x006C)
#define PAD_ST1			(0x0070)
#define PAD_CTL			(0x0074)
#define PAD_DRV0		(0x0080)
#define PAD_DRV1		(0x0084)
#define PAD_DRV2		(0x0088)
#define PAD_SR0			(0x0270)
#define PAD_SR1			(0x0274)
#define PAD_SR2			(0x0278)

#define _GPIOA(offset)		(offset)
#define _GPIOB(offset)		(32 + (offset))
#define _GPIOC(offset)		(64 + (offset))
#define _GPIOD(offset)		(76 + (offset))
#define _GPIOE(offset)		(106 + (offset))
#define _GPIOF(offset)		(138 + (offset))

#define NUM_GPIOS		(_GPIOF(7) + 1)
#define _PIN(offset)		(NUM_GPIOS + (offset))

/* Ethernet MAC */
#define ETH_TXD0		_GPIOA(0)
#define ETH_TXD1		_GPIOA(1)
#define ETH_TXEN		_GPIOA(2)
#define ETH_RXER		_GPIOA(3)
#define ETH_CRS_DV		_GPIOA(4)
#define ETH_RXD1		_GPIOA(5)
#define ETH_RXD0		_GPIOA(6)
#define ETH_REF_CLK		_GPIOA(7)
#define ETH_MDC			_GPIOA(8)
#define ETH_MDIO		_GPIOA(9)

/* SIRQ */
#define SIRQ0			_GPIOA(10)
#define SIRQ1			_GPIOA(11)
#define SIRQ2			_GPIOA(12)

/* I2S */
#define I2S_D0			_GPIOA(13)
#define I2S_BCLK0		_GPIOA(14)
#define I2S_LRCLK0		_GPIOA(15)
#define I2S_MCLK0		_GPIOA(16)
#define I2S_D1			_GPIOA(17)
#define I2S_BCLK1		_GPIOA(18)
#define I2S_LRCLK1		_GPIOA(19)
#define I2S_MCLK1		_GPIOA(20)

/* PCM1 */
#define PCM1_IN			_GPIOF(0)
#define PCM1_CLK		_GPIOF(1)
#define PCM1_SYNC		_GPIOF(2)
#define PCM1_OUT		_GPIOF(3)

/* ERAM */
#define ERAM_A5			_GPIOA(21)
#define ERAM_A6			_GPIOA(22)
#define ERAM_A7			_GPIOA(23)
#define ERAM_A8			_GPIOA(24)
#define ERAM_A9			_GPIOA(25)
#define ERAM_A10		_GPIOA(26)
#define ERAM_A11		_GPIOA(27)

/* LCD0 */
#define LVDS_OEP		_GPIOD(0)
#define LVDS_OEN		_GPIOD(1)
#define LVDS_ODP		_GPIOD(2)
#define LVDS_ODN		_GPIOD(3)
#define LVDS_OCP		_GPIOD(4)
#define LVDS_OCN		_GPIOD(5)
#define LVDS_OBP		_GPIOD(6)
#define LVDS_OBN		_GPIOD(7)
#define LVDS_OAP		_GPIOD(8)
#define LVDS_OAN		_GPIOD(9)
#define LVDS_EEP		_GPIOD(10)
#define LVDS_EEN		_GPIOD(11)
#define LVDS_EDP		_GPIOD(12)
#define LVDS_EDN		_GPIOD(13)
#define LVDS_ECP		_GPIOD(14)
#define LVDS_ECN		_GPIOD(15)
#define LVDS_EBP		_GPIOD(16)
#define LVDS_EBN		_GPIOD(17)
#define LVDS_EAP		_GPIOD(18)
#define LVDS_EAN		_GPIOD(19)

/* SD */
#define SD0_D0			_GPIOA(28)
#define SD0_D1			_GPIOA(29)
#define SD0_D2			_GPIOA(30)
#define SD0_D3			_GPIOA(31)
#define SD1_D0			_GPIOB(0)
#define SD1_D1			_GPIOB(1)
#define SD1_D2			_GPIOB(2)
#define SD1_D3			_GPIOB(3)
#define SD0_CMD			_GPIOB(4)
#define SD0_CLK			_GPIOB(5)
#define SD1_CMD			_GPIOB(6)
#define SD1_CLK			_GPIOB(7)

/* SPI */
#define SPI0_SCLK		_GPIOB(8)
#define SPI0_SS			_GPIOB(9)
#define SPI0_MISO		_GPIOB(10)
#define SPI0_MOSI		_GPIOB(11)

/* UART */
#define UART0_RX		_GPIOB(12)
#define UART0_TX		_GPIOB(13)
#define UART2_RX		_GPIOB(14)
#define UART2_TX		_GPIOB(15)
#define UART2_RTSB		_GPIOB(16)
#define UART2_CTSB		_GPIOB(17)
#define UART3_RX		_GPIOF(4)
#define UART3_TX		_GPIOF(5)
#define UART3_RTSB		_GPIOF(6)
#define UART3_CTSB		_GPIOF(7)
#define UART4_RX		_GPIOB(18)
#define UART4_TX		_GPIOB(19)

/* I2C */
#define I2C0_SCLK		_GPIOB(20)
#define I2C0_SDATA		_GPIOB(21)
#define I2C1_SCLK		_GPIOB(22)
#define I2C1_SDATA		_GPIOB(23)
#define I2C2_SCLK		_GPIOB(24)
#define I2C2_SDATA		_GPIOB(25)

/* MIPI CSI */
#define CSI0_DN0		_GPIOB(26)
#define CSI0_DP0		_GPIOB(27)
#define CSI0_DN1		_GPIOB(28)
#define CSI0_DP1		_GPIOB(29)
#define CSI0_CN			_GPIOB(30)
#define CSI0_CP			_GPIOB(31)
#define CSI0_DN2		_GPIOC(0)
#define CSI0_DP2		_GPIOC(1)
#define CSI0_DN3		_GPIOC(2)
#define CSI0_DP3		_GPIOC(3)

/* MIPI DSI */
#define DSI_DP3			_GPIOD(20)
#define DSI_DN3			_GPIOD(21)
#define DSI_DP1			_GPIOD(22)
#define DSI_DN1			_GPIOD(23)
#define DSI_CP			_GPIOD(24)
#define DSI_CN			_GPIOD(25)
#define DSI_DP0			_GPIOD(26)
#define DSI_DN0			_GPIOD(27)
#define DSI_DP2			_GPIOD(28)
#define DSI_DN2			_GPIOD(29)

/* Sensor */
#define SENSOR0_PCLK		_GPIOC(4)

#define CSI1_DN0		_GPIOC(5)
#define CSI1_DP0		_GPIOC(6)
#define CSI1_DN1		_GPIOC(7)
#define CSI1_DP1		_GPIOC(8)
#define CSI1_CN			_GPIOC(9)
#define CSI1_CP			_GPIOC(10)

#define SENSOR0_CKOUT		_GPIOC(11)

/* NAND (1.8v / 3.3v) */
#define NAND0_D0		_GPIOE(0)
#define NAND0_D1		_GPIOE(1)
#define NAND0_D2		_GPIOE(2)
#define NAND0_D3		_GPIOE(3)
#define NAND0_D4		_GPIOE(4)
#define NAND0_D5		_GPIOE(5)
#define NAND0_D6		_GPIOE(6)
#define NAND0_D7		_GPIOE(7)
#define NAND0_DQS		_GPIOE(8)
#define NAND0_DQSN		_GPIOE(9)
#define NAND0_ALE		_GPIOE(10)
#define NAND0_CLE		_GPIOE(11)
#define NAND0_CEB0		_GPIOE(12)
#define NAND0_CEB1		_GPIOE(13)
#define NAND0_CEB2		_GPIOE(14)
#define NAND0_CEB3		_GPIOE(15)

#define NAND1_D0		_GPIOE(16)
#define NAND1_D1		_GPIOE(17)
#define NAND1_D2		_GPIOE(18)
#define NAND1_D3		_GPIOE(19)
#define NAND1_D4		_GPIOE(20)
#define NAND1_D5		_GPIOE(21)
#define NAND1_D6		_GPIOE(22)
#define NAND1_D7		_GPIOE(23)
#define NAND1_DQS		_GPIOE(24)
#define NAND1_DQSN		_GPIOE(25)
#define NAND1_ALE		_GPIOE(26)
#define NAND1_CLE		_GPIOE(27)
#define NAND1_CEB0		_GPIOE(28)
#define NAND1_CEB1		_GPIOE(29)
#define NAND1_CEB2		_GPIOE(30)
#define NAND1_CEB3		_GPIOE(31)

/* System */
#define SGPIO0			_PIN(0)
#define SGPIO1			_PIN(1)
#define SGPIO2			_PIN(2)
#define SGPIO3			_PIN(3)

#define NUM_PADS		(_PIN(3) + 1)

/* Pad names as specified in datasheet */
const struct pinctrl_pin_desc s900_pads[] = {
	PINCTRL_PIN(ETH_TXD0, "ETH_TXD0"),
	PINCTRL_PIN(ETH_TXD1, "ETH_TXD1"),
	PINCTRL_PIN(ETH_TXEN, "ETH_TXEN"),
	PINCTRL_PIN(ETH_RXER, "ETH_RXER"),
	PINCTRL_PIN(ETH_CRS_DV, "ETH_CRS_DV"),
	PINCTRL_PIN(ETH_RXD1, "ETH_RXD1"),
	PINCTRL_PIN(ETH_RXD0, "ETH_RXD0"),
	PINCTRL_PIN(ETH_REF_CLK, "ETH_REF_CLK"),
	PINCTRL_PIN(ETH_MDC, "ETH_MDC"),
	PINCTRL_PIN(ETH_MDIO, "ETH_MDIO"),
	PINCTRL_PIN(SIRQ0, "SIRQ0"),
	PINCTRL_PIN(SIRQ1, "SIRQ1"),
	PINCTRL_PIN(SIRQ2, "SIRQ2"),
	PINCTRL_PIN(I2S_D0, "I2S_D0"),
	PINCTRL_PIN(I2S_BCLK0, "I2S_BCLK0"),
	PINCTRL_PIN(I2S_LRCLK0, "I2S_LRCLK0"),
	PINCTRL_PIN(I2S_MCLK0, "I2S_MCLK0"),
	PINCTRL_PIN(I2S_D1, "I2S_D1"),
	PINCTRL_PIN(I2S_BCLK1, "I2S_BCLK1"),
	PINCTRL_PIN(I2S_LRCLK1, "I2S_LRCLK1"),
	PINCTRL_PIN(I2S_MCLK1, "I2S_MCLK1"),
	PINCTRL_PIN(PCM1_IN, "PCM1_IN"),
	PINCTRL_PIN(PCM1_CLK, "PCM1_CLK"),
	PINCTRL_PIN(PCM1_SYNC, "PCM1_SYNC"),
	PINCTRL_PIN(PCM1_OUT, "PCM1_OUT"),
	PINCTRL_PIN(ERAM_A5, "ERAM_A5"),
	PINCTRL_PIN(ERAM_A6, "ERAM_A6"),
	PINCTRL_PIN(ERAM_A7, "ERAM_A7"),
	PINCTRL_PIN(ERAM_A8, "ERAM_A8"),
	PINCTRL_PIN(ERAM_A9, "ERAM_A9"),
	PINCTRL_PIN(ERAM_A10, "ERAM_A10"),
	PINCTRL_PIN(ERAM_A11, "ERAM_A11"),
	PINCTRL_PIN(LVDS_OEP, "LVDS_OEP"),
	PINCTRL_PIN(LVDS_OEN, "LVDS_OEN"),
	PINCTRL_PIN(LVDS_ODP, "LVDS_ODP"),
	PINCTRL_PIN(LVDS_ODN, "LVDS_ODN"),
	PINCTRL_PIN(LVDS_OCP, "LVDS_OCP"),
	PINCTRL_PIN(LVDS_OCN, "LVDS_OCN"),
	PINCTRL_PIN(LVDS_OBP, "LVDS_OBP"),
	PINCTRL_PIN(LVDS_OBN, "LVDS_OBN"),
	PINCTRL_PIN(LVDS_OAP, "LVDS_OAP"),
	PINCTRL_PIN(LVDS_OAN, "LVDS_OAN"),
	PINCTRL_PIN(LVDS_EEP, "LVDS_EEP"),
	PINCTRL_PIN(LVDS_EEN, "LVDS_EEN"),
	PINCTRL_PIN(LVDS_EDP, "LVDS_EDP"),
	PINCTRL_PIN(LVDS_EDN, "LVDS_EDN"),
	PINCTRL_PIN(LVDS_ECP, "LVDS_ECP"),
	PINCTRL_PIN(LVDS_ECN, "LVDS_ECN"),
	PINCTRL_PIN(LVDS_EBP, "LVDS_EBP"),
	PINCTRL_PIN(LVDS_EBN, "LVDS_EBN"),
	PINCTRL_PIN(LVDS_EAP, "LVDS_EAP"),
	PINCTRL_PIN(LVDS_EAN, "LVDS_EAN"),
	PINCTRL_PIN(SD0_D0, "SD0_D0"),
	PINCTRL_PIN(SD0_D1, "SD0_D1"),
	PINCTRL_PIN(SD0_D2, "SD0_D2"),
	PINCTRL_PIN(SD0_D3, "SD0_D3"),
	PINCTRL_PIN(SD1_D0, "SD1_D0"),
	PINCTRL_PIN(SD1_D1, "SD1_D1"),
	PINCTRL_PIN(SD1_D2, "SD1_D2"),
	PINCTRL_PIN(SD1_D3, "SD1_D3"),
	PINCTRL_PIN(SD0_CMD, "SD0_CMD"),
	PINCTRL_PIN(SD0_CLK, "SD0_CLK"),
	PINCTRL_PIN(SD1_CMD, "SD1_CMD"),
	PINCTRL_PIN(SD1_CLK, "SD1_CLK"),
	PINCTRL_PIN(SPI0_SCLK, "SPI0_SCLK"),
	PINCTRL_PIN(SPI0_SS, "SPI0_SS"),
	PINCTRL_PIN(SPI0_MISO, "SPI0_MISO"),
	PINCTRL_PIN(SPI0_MOSI, "SPI0_MOSI"),
	PINCTRL_PIN(UART0_RX, "UART0_RX"),
	PINCTRL_PIN(UART0_TX, "UART0_TX"),
	PINCTRL_PIN(UART2_RX, "UART2_RX"),
	PINCTRL_PIN(UART2_TX, "UART2_TX"),
	PINCTRL_PIN(UART2_RTSB, "UART2_RTSB"),
	PINCTRL_PIN(UART2_CTSB, "UART2_CTSB"),
	PINCTRL_PIN(UART3_RX, "UART3_RX"),
	PINCTRL_PIN(UART3_TX, "UART3_TX"),
	PINCTRL_PIN(UART3_RTSB, "UART3_RTSB"),
	PINCTRL_PIN(UART3_CTSB, "UART3_CTSB"),
	PINCTRL_PIN(UART4_RX, "UART4_RX"),
	PINCTRL_PIN(UART4_TX, "UART4_TX"),
	PINCTRL_PIN(I2C0_SCLK, "I2C0_SCLK"),
	PINCTRL_PIN(I2C0_SDATA, "I2C0_SDATA"),
	PINCTRL_PIN(I2C1_SCLK, "I2C1_SCLK"),
	PINCTRL_PIN(I2C1_SDATA, "I2C1_SDATA"),
	PINCTRL_PIN(I2C2_SCLK, "I2C2_SCLK"),
	PINCTRL_PIN(I2C2_SDATA, "I2C2_SDATA"),
	PINCTRL_PIN(CSI0_DN0, "CSI0_DN0"),
	PINCTRL_PIN(CSI0_DP0, "CSI0_DP0"),
	PINCTRL_PIN(CSI0_DN1, "CSI0_DN1"),
	PINCTRL_PIN(CSI0_DP1, "CSI0_DP1"),
	PINCTRL_PIN(CSI0_CN, "CSI0_CN"),
	PINCTRL_PIN(CSI0_CP, "CSI0_CP"),
	PINCTRL_PIN(CSI0_DN2, "CSI0_DN2"),
	PINCTRL_PIN(CSI0_DP2, "CSI0_DP2"),
	PINCTRL_PIN(CSI0_DN3, "CSI0_DN3"),
	PINCTRL_PIN(CSI0_DP3, "CSI0_DP3"),
	PINCTRL_PIN(DSI_DP3, "DSI_DP3"),
	PINCTRL_PIN(DSI_DN3, "DSI_DN3"),
	PINCTRL_PIN(DSI_DP1, "DSI_DP1"),
	PINCTRL_PIN(DSI_DN1, "DSI_DN1"),
	PINCTRL_PIN(DSI_CP, "DSI_CP"),
	PINCTRL_PIN(DSI_CN, "DSI_CN"),
	PINCTRL_PIN(DSI_DP0, "DSI_DP0"),
	PINCTRL_PIN(DSI_DN0, "DSI_DN0"),
	PINCTRL_PIN(DSI_DP2, "DSI_DP2"),
	PINCTRL_PIN(DSI_DN2, "DSI_DN2"),
	PINCTRL_PIN(SENSOR0_PCLK, "SENSOR0_PCLK"),
	PINCTRL_PIN(CSI1_DN0, "CSI1_DN0"),
	PINCTRL_PIN(CSI1_DP0, "CSI1_DP0"),
	PINCTRL_PIN(CSI1_DN1, "CSI1_DN1"),
	PINCTRL_PIN(CSI1_DP1, "CSI1_DP1"),
	PINCTRL_PIN(CSI1_CN, "CSI1_CN"),
	PINCTRL_PIN(CSI1_CP, "CSI1_CP"),
	PINCTRL_PIN(SENSOR0_CKOUT, "SENSOR0_CKOUT"),
	PINCTRL_PIN(NAND0_D0, "NAND0_D0"),
	PINCTRL_PIN(NAND0_D1, "NAND0_D1"),
	PINCTRL_PIN(NAND0_D2, "NAND0_D2"),
	PINCTRL_PIN(NAND0_D3, "NAND0_D3"),
	PINCTRL_PIN(NAND0_D4, "NAND0_D4"),
	PINCTRL_PIN(NAND0_D5, "NAND0_D5"),
	PINCTRL_PIN(NAND0_D6, "NAND0_D6"),
	PINCTRL_PIN(NAND0_D7, "NAND0_D7"),
	PINCTRL_PIN(NAND0_DQS, "NAND0_DQS"),
	PINCTRL_PIN(NAND0_DQSN, "NAND0_DQSN"),
	PINCTRL_PIN(NAND0_ALE, "NAND0_ALE"),
	PINCTRL_PIN(NAND0_CLE, "NAND0_CLE"),
	PINCTRL_PIN(NAND0_CEB0, "NAND0_CEB0"),
	PINCTRL_PIN(NAND0_CEB1, "NAND0_CEB1"),
	PINCTRL_PIN(NAND0_CEB2, "NAND0_CEB2"),
	PINCTRL_PIN(NAND0_CEB3, "NAND0_CEB3"),
	PINCTRL_PIN(NAND1_D0, "NAND1_D0"),
	PINCTRL_PIN(NAND1_D1, "NAND1_D1"),
	PINCTRL_PIN(NAND1_D2, "NAND1_D2"),
	PINCTRL_PIN(NAND1_D3, "NAND1_D3"),
	PINCTRL_PIN(NAND1_D4, "NAND1_D4"),
	PINCTRL_PIN(NAND1_D5, "NAND1_D5"),
	PINCTRL_PIN(NAND1_D6, "NAND1_D6"),
	PINCTRL_PIN(NAND1_D7, "NAND1_D7"),
	PINCTRL_PIN(NAND1_DQS, "NAND1_DQS"),
	PINCTRL_PIN(NAND1_DQSN, "NAND1_DQSN"),
	PINCTRL_PIN(NAND1_ALE, "NAND1_ALE"),
	PINCTRL_PIN(NAND1_CLE, "NAND1_CLE"),
	PINCTRL_PIN(NAND1_CEB0, "NAND1_CEB0"),
	PINCTRL_PIN(NAND1_CEB1, "NAND1_CEB1"),
	PINCTRL_PIN(NAND1_CEB2, "NAND1_CEB2"),
	PINCTRL_PIN(NAND1_CEB3, "NAND1_CEB3"),
	PINCTRL_PIN(SGPIO0, "SGPIO0"),
	PINCTRL_PIN(SGPIO1, "SGPIO1"),
	PINCTRL_PIN(SGPIO2, "SGPIO2"),
	PINCTRL_PIN(SGPIO3, "SGPIO3"),
};

enum s900_pinmux_functions {
	S900_MUX_ERAM,
	S900_MUX_ETH_RMII,
	S900_MUX_ETH_SMII,
	S900_MUX_SPI0,
	S900_MUX_SPI1,
	S900_MUX_SPI2,
	S900_MUX_SPI3,
	S900_MUX_SENS0,
	S900_MUX_UART0,
	S900_MUX_UART1,
	S900_MUX_UART2,
	S900_MUX_UART3,
	S900_MUX_UART4,
	S900_MUX_UART5,
	S900_MUX_UART6,
	S900_MUX_I2S0,
	S900_MUX_I2S1,
	S900_MUX_PCM0,
	S900_MUX_PCM1,
	S900_MUX_JTAG,
	S900_MUX_PWM0,
	S900_MUX_PWM1,
	S900_MUX_PWM2,
	S900_MUX_PWM3,
	S900_MUX_PWM4,
	S900_MUX_PWM5,
	S900_MUX_SD0,
	S900_MUX_SD1,
	S900_MUX_SD2,
	S900_MUX_SD3,
	S900_MUX_I2C0,
	S900_MUX_I2C1,
	S900_MUX_I2C2,
	S900_MUX_I2C3,
	S900_MUX_I2C4,
	S900_MUX_I2C5,
	S900_MUX_LVDS,
	S900_MUX_USB20,
	S900_MUX_USB30,
	S900_MUX_GPU,
	S900_MUX_MIPI_CSI0,
	S900_MUX_MIPI_CSI1,
	S900_MUX_MIPI_DSI,
	S900_MUX_NAND0,
	S900_MUX_NAND1,
	S900_MUX_SPDIF,
	S900_MUX_SIRQ0,
	S900_MUX_SIRQ1,
	S900_MUX_SIRQ2,
	S900_MUX_AUX_START,
	S900_MUX_MAX,
	S900_MUX_RESERVED,
};

/* mfp0_22 */
static unsigned int owl_mfp0_22_pads[] = {
	LVDS_OAP,
	LVDS_OAN,
};

static unsigned int owl_mfp0_22_funcs[] = {
	S900_MUX_ERAM,
	S900_MUX_UART4,
};

/* mfp0_21_20 */
static unsigned int owl_mfp0_21_20_eth_mdc_pads[] = {
	ETH_MDC,
};

static unsigned int owl_mfp0_21_20_eth_mdc_funcs[] = {
	S900_MUX_ETH_RMII,
	S900_MUX_PWM2,
	S900_MUX_UART2,
	S900_MUX_RESERVED,
};

static unsigned int owl_mfp0_21_20_eth_mdio_pads[] = {
	ETH_MDIO,
};

static unsigned int owl_mfp0_21_20_eth_mdio_funcs[] = {
	S900_MUX_ETH_RMII,
	S900_MUX_PWM3,
	S900_MUX_UART2,
	S900_MUX_RESERVED,
};

/* mfp0_19 */
static unsigned int owl_mfp0_19_sirq0_pads[] = {
	SIRQ0,
};

static unsigned int owl_mfp0_19_sirq0_funcs[] = {
	S900_MUX_SIRQ0,
	S900_MUX_PWM0,
};

static unsigned int owl_mfp0_19_sirq1_pads[] = {
	SIRQ1,
};

static unsigned int owl_mfp0_19_sirq1_funcs[] = {
	S900_MUX_SIRQ1,
	S900_MUX_PWM1,
};

/* mfp0_18_16 */
static unsigned int owl_mfp0_18_16_eth_txd0_pads[] = {
	ETH_TXD0,
};

static unsigned int owl_mfp0_18_16_eth_txd0_funcs[] = {
	S900_MUX_ETH_RMII,
	S900_MUX_ETH_SMII,
	S900_MUX_SPI2,
	S900_MUX_UART6,
	S900_MUX_SENS0,
	S900_MUX_PWM0,
};

static unsigned int owl_mfp0_18_16_eth_txd1_pads[] = {
	ETH_TXD1,
};

static unsigned int owl_mfp0_18_16_eth_txd1_funcs[] = {
	S900_MUX_ETH_RMII,
	S900_MUX_ETH_SMII,
	S900_MUX_SPI2,
	S900_MUX_UART6,
	S900_MUX_SENS0,
	S900_MUX_PWM1,
};

/* mfp0_15_13 */
static unsigned int owl_mfp0_15_13_eth_txen_pads[] = {
	ETH_TXEN,
};

static unsigned int owl_mfp0_15_13_eth_txen_funcs[] = {
	S900_MUX_ETH_RMII,
	S900_MUX_UART2,
	S900_MUX_SPI3,
	S900_MUX_RESERVED,
	S900_MUX_RESERVED,
	S900_MUX_PWM2,
	S900_MUX_SENS0,
};

static unsigned int owl_mfp0_15_13_eth_rxer_pads[] = {
	ETH_RXER,
};

static unsigned int owl_mfp0_15_13_eth_rxer_funcs[] = {
	S900_MUX_ETH_RMII,
	S900_MUX_UART2,
	S900_MUX_SPI3,
	S900_MUX_RESERVED,
	S900_MUX_RESERVED,
	S900_MUX_PWM3,
	S900_MUX_SENS0,
};

/* mfp0_12_11 */
static unsigned int owl_mfp0_12_11_pads[] = {
	ETH_CRS_DV,
};

static unsigned int owl_mfp0_12_11_funcs[] = {
	S900_MUX_ETH_RMII,
	S900_MUX_ETH_SMII,
	S900_MUX_SPI2,
	S900_MUX_UART4,
};

/* mfp0_10_8 */
static unsigned int owl_mfp0_10_8_eth_rxd1_pads[] = {
	ETH_RXD1,
};

static unsigned int owl_mfp0_10_8_eth_rxd1_funcs[] = {
	S900_MUX_ETH_RMII,
	S900_MUX_UART2,
	S900_MUX_SPI3,
	S900_MUX_RESERVED,
	S900_MUX_UART5,
	S900_MUX_PWM0,
	S900_MUX_SENS0,
};

static unsigned int owl_mfp0_10_8_eth_rxd0_pads[] = {
	ETH_RXD0,
};

static unsigned int owl_mfp0_10_8_eth_rxd0_funcs[] = {
	S900_MUX_ETH_RMII,
	S900_MUX_UART2,
	S900_MUX_SPI3,
	S900_MUX_RESERVED,
	S900_MUX_UART5,
	S900_MUX_PWM1,
	S900_MUX_SENS0,
};

/* mfp0_7_6 */
static unsigned int owl_mfp0_7_6_pads[] = {
	ETH_REF_CLK,
};

static unsigned int owl_mfp0_7_6_funcs[] = {
	S900_MUX_ETH_RMII,
	S900_MUX_UART4,
	S900_MUX_SPI2,
	S900_MUX_RESERVED,
};

/* mfp0_5 */
static unsigned int owl_mfp0_5_i2s_d0_pads[] = {
	I2S_D0,
};

static unsigned int owl_mfp0_5_i2s_d0_funcs[] = {
	S900_MUX_I2S0,
	S900_MUX_PCM0,
};

static unsigned int owl_mfp0_5_i2s_d1_pads[] = {
	I2S_D1,
};

static unsigned int owl_mfp0_5_i2s_d1_funcs[] = {
	S900_MUX_I2S1,
	S900_MUX_PCM0,
};

/* mfp0_4_3 */
static unsigned int owl_mfp0_4_3_pads[] = {
	I2S_LRCLK0,
	I2S_MCLK0,
};

static unsigned int owl_mfp0_4_3_funcs[] = {
	S900_MUX_I2S0,
	S900_MUX_PCM0,
	S900_MUX_PCM1,
	S900_MUX_RESERVED,
};

/* mfp0_2 */
static unsigned int owl_mfp0_2_i2s0_pads[] = {
	I2S_BCLK0,
};

static unsigned int owl_mfp0_2_i2s0_funcs[] = {
	S900_MUX_I2S0,
	S900_MUX_PCM0,
};

static unsigned int owl_mfp0_2_i2s1_pads[] = {
	I2S_BCLK1,
};

static unsigned int owl_mfp0_2_i2s1_funcs[] = {
	S900_MUX_I2S1,
	S900_MUX_PCM0,
};

/* mfp0_1_0 */
static unsigned int owl_mfp0_1_0_pcm1_in_out_pads[] = {
	PCM1_IN,
	PCM1_OUT,
};

static unsigned int owl_mfp0_1_0_pcm1_in_out_funcs[] = {
	S900_MUX_PCM1,
	S900_MUX_SPI1,
	S900_MUX_I2C3,
	S900_MUX_UART4,
};

static unsigned int owl_mfp0_1_0_pcm1_clk_pads[] = {
	PCM1_CLK,
};

static unsigned int owl_mfp0_1_0_pcm1_clk_funcs[] = {
	S900_MUX_PCM1,
	S900_MUX_SPI1,
	S900_MUX_PWM4,
	S900_MUX_UART4,
};

static unsigned int owl_mfp0_1_0_pcm1_sync_pads[] = {
	PCM1_SYNC,
};

static unsigned int owl_mfp0_1_0_pcm1_sync_funcs[] = {
	S900_MUX_PCM1,
	S900_MUX_SPI1,
	S900_MUX_PWM5,
	S900_MUX_UART4,
};

/* mfp1_31_29 */
static unsigned int owl_mfp1_31_29_eram_a5_pads[] = {
	ERAM_A5,
};

static unsigned int owl_mfp1_31_29_eram_a5_funcs[] = {
	S900_MUX_UART4,
	S900_MUX_JTAG,
	S900_MUX_ERAM,
	S900_MUX_PWM0,
	S900_MUX_RESERVED,
	S900_MUX_SENS0,
};

static unsigned int owl_mfp1_31_29_eram_a6_pads[] = {
	ERAM_A6,
};

static unsigned int owl_mfp1_31_29_eram_a6_funcs[] = {
	S900_MUX_UART4,
	S900_MUX_JTAG,
	S900_MUX_ERAM,
	S900_MUX_PWM1,
	S900_MUX_RESERVED,
	S900_MUX_SENS0,
};

static unsigned int owl_mfp1_31_29_eram_a7_pads[] = {
	ERAM_A7,
};

static unsigned int owl_mfp1_31_29_eram_a7_funcs[] = {
	S900_MUX_RESERVED,
	S900_MUX_JTAG,
	S900_MUX_ERAM,
	S900_MUX_RESERVED,
	S900_MUX_RESERVED,
	S900_MUX_SENS0,
};

/* mfp1_28_26 */
static unsigned int owl_mfp1_28_26_eram_a8_pads[] = {
	ERAM_A8,
};

static unsigned int owl_mfp1_28_26_eram_a8_funcs[] = {
	S900_MUX_RESERVED,
	S900_MUX_JTAG,
	S900_MUX_ERAM,
	S900_MUX_PWM1,
	S900_MUX_RESERVED,
	S900_MUX_SENS0,
};

static unsigned int owl_mfp1_28_26_eram_a9_pads[] = {
	ERAM_A9,
};

static unsigned int owl_mfp1_28_26_eram_a9_funcs[] = {
	S900_MUX_USB20,
	S900_MUX_UART5,
	S900_MUX_ERAM,
	S900_MUX_PWM2,
	S900_MUX_RESERVED,
	S900_MUX_SENS0,
};

static unsigned int owl_mfp1_28_26_eram_a10_pads[] = {
	ERAM_A10,
};

static unsigned int owl_mfp1_28_26_eram_a10_funcs[] = {
	S900_MUX_USB30,
	S900_MUX_JTAG,
	S900_MUX_ERAM,
	S900_MUX_PWM3,
	S900_MUX_RESERVED,
	S900_MUX_SENS0,
	S900_MUX_RESERVED,
	S900_MUX_RESERVED,
};

/* mfp1_25_23 */
static unsigned int owl_mfp1_25_23_pads[] = {
	ERAM_A11,
};

static unsigned int owl_mfp1_25_23_funcs[] = {
	S900_MUX_RESERVED,
	S900_MUX_RESERVED,
	S900_MUX_ERAM,
	S900_MUX_PWM2,
	S900_MUX_UART5,
	S900_MUX_RESERVED,
	S900_MUX_SENS0,
	S900_MUX_RESERVED,
};

/* mfp1_22_lvds_o */
static unsigned int owl_mfp1_22_lvds_oep_odn_pads[] = {
	LVDS_OEP,
	LVDS_OEN,
	LVDS_ODP,
	LVDS_ODN,
};

static unsigned int owl_mfp1_22_lvds_oep_odn_funcs[] = {
	S900_MUX_LVDS,
	S900_MUX_UART2,
};

static unsigned int owl_mfp1_22_lvds_ocp_obn_pads[] = {
	LVDS_OCP,
	LVDS_OCN,
	LVDS_OBP,
	LVDS_OBN,
};

static unsigned int owl_mfp1_22_lvds_ocp_obn_funcs[] = {
	S900_MUX_LVDS,
	S900_MUX_PCM1,
};

static unsigned int owl_mfp1_22_lvds_oap_oan_pads[] = {
	LVDS_OAP,
	LVDS_OAN,
};

static unsigned int owl_mfp1_22_lvds_oap_oan_funcs[] = {
	S900_MUX_LVDS,
	S900_MUX_ERAM,
};

/* mfp1_21_lvds_e */
static unsigned int owl_mfp1_21_lvds_e_pads[] = {
	LVDS_EEP,
	LVDS_EEN,
	LVDS_EDP,
	LVDS_EDN,
	LVDS_ECP,
	LVDS_ECN,
	LVDS_EBP,
	LVDS_EBN,
	LVDS_EAP,
	LVDS_EAN,
};

static unsigned int owl_mfp1_21_lvds_e_funcs[] = {
	S900_MUX_LVDS,
	S900_MUX_ERAM,
};

/* mfp1_5_4 */
static unsigned int owl_mfp1_5_4_pads[] = {
	SPI0_SCLK,
	SPI0_MOSI,
};

static unsigned int owl_mfp1_5_4_funcs[] = {
	S900_MUX_SPI0,
	S900_MUX_ERAM,
	S900_MUX_I2C3,
	S900_MUX_PCM0,
};

/* mfp1_3_1 */
static unsigned int owl_mfp1_3_1_spi0_ss_pads[] = {
	SPI0_SS,
};

static unsigned int owl_mfp1_3_1_spi0_ss_funcs[] = {
	S900_MUX_SPI0,
	S900_MUX_ERAM,
	S900_MUX_I2S1,
	S900_MUX_PCM1,
	S900_MUX_PCM0,
	S900_MUX_PWM4,
};

static unsigned int owl_mfp1_3_1_spi0_miso_pads[] = {
	SPI0_MISO,
};

static unsigned int owl_mfp1_3_1_spi0_miso_funcs[] = {
	S900_MUX_SPI0,
	S900_MUX_ERAM,
	S900_MUX_I2S1,
	S900_MUX_PCM1,
	S900_MUX_PCM0,
	S900_MUX_PWM5,
};

/* mfp2_23 */
static unsigned int owl_mfp2_23_pads[] = {
	UART2_RTSB,
};

static unsigned int owl_mfp2_23_funcs[] = {
	S900_MUX_UART2,
	S900_MUX_UART0,
};

/* mfp2_22 */
static unsigned int owl_mfp2_22_pads[] = {
	UART2_CTSB,
};

static unsigned int owl_mfp2_22_funcs[] = {
	S900_MUX_UART2,
	S900_MUX_UART0,
};

/* mfp2_21 */
static unsigned int owl_mfp2_21_pads[] = {
	UART3_RTSB,
};

static unsigned int owl_mfp2_21_funcs[] = {
	S900_MUX_UART3,
	S900_MUX_UART5,
};

/* mfp2_20 */
static unsigned int owl_mfp2_20_pads[] = {
	UART3_CTSB,
};

static unsigned int owl_mfp2_20_funcs[] = {
	S900_MUX_UART3,
	S900_MUX_UART5,
};

/* mfp2_19_17 */
static unsigned int owl_mfp2_19_17_pads[] = {
	SD0_D0,
};

static unsigned int owl_mfp2_19_17_funcs[] = {
	S900_MUX_SD0,
	S900_MUX_ERAM,
	S900_MUX_RESERVED,
	S900_MUX_JTAG,
	S900_MUX_UART2,
	S900_MUX_UART5,
	S900_MUX_GPU,
};

/* mfp2_16_14 */
static unsigned int owl_mfp2_16_14_pads[] = {
	SD0_D1,
};

static unsigned int owl_mfp2_16_14_funcs[] = {
	S900_MUX_SD0,
	S900_MUX_ERAM,
	S900_MUX_GPU,
	S900_MUX_RESERVED,
	S900_MUX_UART2,
	S900_MUX_UART5,
};

/* mfp2_13_11 */
static unsigned int owl_mfp2_13_11_pads[] = {
	SD0_D2,
	SD0_D3,
};

static unsigned int owl_mfp2_13_11_funcs[] = {
	S900_MUX_SD0,
	S900_MUX_ERAM,
	S900_MUX_RESERVED,
	S900_MUX_JTAG,
	S900_MUX_UART2,
	S900_MUX_UART1,
	S900_MUX_GPU,
};

/* mfp2_10_9 */
static unsigned int owl_mfp2_10_9_pads[] = {
	SD1_D0,
	SD1_D1,
	SD1_D2,
	SD1_D3,
};

static unsigned int owl_mfp2_10_9_funcs[] = {
	S900_MUX_SD1,
	S900_MUX_ERAM,
};

/* mfp2_8_7 */
static unsigned int owl_mfp2_8_7_pads[] = {
	SD0_CMD,
};

static unsigned int owl_mfp2_8_7_funcs[] = {
	S900_MUX_SD0,
	S900_MUX_ERAM,
	S900_MUX_GPU,
	S900_MUX_JTAG,
};

/* mfp2_6_5 */
static unsigned int owl_mfp2_6_5_pads[] = {
	SD0_CLK,
};

static unsigned int owl_mfp2_6_5_funcs[] = {
	S900_MUX_SD0,
	S900_MUX_ERAM,
	S900_MUX_JTAG,
	S900_MUX_GPU,
};

/* mfp2_4_3 */
static unsigned int owl_mfp2_4_3_pads[] = {
	SD1_CMD,
	SD1_CLK,
};

static unsigned int owl_mfp2_4_3_funcs[] = {
	S900_MUX_SD1,
	S900_MUX_ERAM,
};

/* mfp2_2_0 */
static unsigned int owl_mfp2_2_0_pads[] = {
	UART0_RX,
};

static unsigned int owl_mfp2_2_0_funcs[] = {
	S900_MUX_UART0,
	S900_MUX_UART2,
	S900_MUX_SPI1,
	S900_MUX_I2C5,
	S900_MUX_PCM1,
	S900_MUX_I2S1,
};


/* mfp3_27 */
static unsigned int owl_mfp3_27_pads[] = {
	NAND0_D0,
	NAND0_D1,
	NAND0_D2,
	NAND0_D3,
	NAND0_D4,
	NAND0_D5,
	NAND0_D6,
	NAND0_D7,
	NAND0_DQSN,
	NAND0_CEB3,
};

static unsigned int owl_mfp3_27_funcs[] = {
	S900_MUX_NAND0,
	S900_MUX_SD2,
};


/* mfp3_21_19 */
static unsigned int owl_mfp3_21_19_pads[] = {
	UART0_TX,
};

static unsigned int owl_mfp3_21_19_funcs[] = {
	S900_MUX_UART0,
	S900_MUX_UART2,
	S900_MUX_SPI1,
	S900_MUX_I2C5,
	S900_MUX_SPDIF,
	S900_MUX_PCM1,
	S900_MUX_I2S1,
};

/* mfp3_18_16 */
static unsigned int owl_mfp3_18_16_pads[] = {
	I2C0_SCLK,
	I2C0_SDATA,
};

static unsigned int owl_mfp3_18_16_funcs[] = {
	S900_MUX_I2C0,
	S900_MUX_UART2,
	S900_MUX_I2C1,
	S900_MUX_UART1,
	S900_MUX_SPI1,
};

/* mfp3_15 */
static unsigned int owl_mfp3_15_pads[] = {
	CSI0_CN,
	CSI0_CP,
};

static unsigned int owl_mfp3_15_funcs[] = {
	S900_MUX_SENS0,
	S900_MUX_SENS0,
};

/* mfp3_14 */
static unsigned int owl_mfp3_14_pads[] = {
	CSI0_DN0,
	CSI0_DP0,
	CSI0_DN1,
	CSI0_DP1,
	CSI0_CN,
	CSI0_CP,
	CSI0_DP2,
	CSI0_DN2,
	CSI0_DN3,
	CSI0_DP3,
};

static unsigned int owl_mfp3_14_funcs[] = {
	S900_MUX_MIPI_CSI0,
	S900_MUX_SENS0,
};

/* mfp3_13 */
static unsigned int owl_mfp3_13_pads[] = {
	CSI1_DN0,
	CSI1_DP0,
	CSI1_DN1,
	CSI1_DP1,
	CSI1_CN,
	CSI1_CP,
};

static unsigned int owl_mfp3_13_funcs[] = {
	S900_MUX_MIPI_CSI1,
	S900_MUX_SENS0,
};


/* mfp3_12_dsi */
static unsigned int owl_mfp3_12_dsi_dp3_dn1_pads[] = {
	DSI_DP3,
	DSI_DN2,
	DSI_DP1,
	DSI_DN1,
};

static unsigned int owl_mfp3_12_dsi_dp3_dn1_funcs[] = {
	S900_MUX_MIPI_DSI,
	S900_MUX_UART2,
};

static unsigned int owl_mfp3_12_dsi_cp_dn0_pads[] = {
	DSI_CP,
	DSI_CN,
	DSI_DP0,
	DSI_DN0,
};

static unsigned int owl_mfp3_12_dsi_cp_dn0_funcs[] = {
	S900_MUX_MIPI_DSI,
	S900_MUX_PCM1,
};

static unsigned int owl_mfp3_12_dsi_dp2_dn2_pads[] = {
	DSI_DP2,
	DSI_DN2,
};

static unsigned int owl_mfp3_12_dsi_dp2_dn2_funcs[] = {
	S900_MUX_MIPI_DSI,
	S900_MUX_UART4,
};

/* mfp3_11 */
static unsigned int owl_mfp3_11_pads[] = {
	NAND1_D0,
	NAND1_D1,
	NAND1_D2,
	NAND1_D3,
	NAND1_D4,
	NAND1_D5,
	NAND1_D6,
	NAND1_D7,
	NAND1_DQSN,
	NAND1_CEB1,
};

static unsigned int owl_mfp3_11_funcs[] = {
	S900_MUX_NAND1,
	S900_MUX_SD3,
};

/* mfp3_10 */
static unsigned int owl_mfp3_10_nand1_ceb3_pads[] = {
	NAND1_CEB3,
};

static unsigned int owl_mfp3_10_nand1_ceb3_funcs[] = {
	S900_MUX_NAND1,
	S900_MUX_PWM0,
};

static unsigned int owl_mfp3_10_nand1_ceb0_pads[] = {
	NAND1_CEB0,
};

static unsigned int owl_mfp3_10_nand1_ceb0_funcs[] = {
	S900_MUX_NAND1,
	S900_MUX_PWM1,
};

/* mfp3_9 */
static unsigned int owl_mfp3_9_pads[] = {
	CSI1_DN0,
	CSI1_DP0,
};

static unsigned int owl_mfp3_9_funcs[] = {
	S900_MUX_SENS0,
	S900_MUX_SENS0,
};

/* mfp3_8 */
static unsigned int owl_mfp3_8_pads[] = {
	UART4_RX,
	UART4_TX,
};

static unsigned int owl_mfp3_8_funcs[] = {
	S900_MUX_UART4,
	S900_MUX_I2C4,
};


/* PADDRV group data */

/* drv0 */
static unsigned int owl_drv0_31_30_pads[] = {
	SGPIO3,
};

static unsigned int owl_drv0_29_28_pads[] = {
	SGPIO2,
};

static unsigned int owl_drv0_27_26_pads[] = {
	SGPIO1,
};

static unsigned int owl_drv0_25_24_pads[] = {
	SGPIO0,
};

static unsigned int owl_drv0_23_22_pads[] = {
	ETH_TXD0,
	ETH_TXD1,
};

static unsigned int owl_drv0_21_20_pads[] = {
	ETH_TXEN,
	ETH_RXER,
};

static unsigned int owl_drv0_19_18_pads[] = {
	ETH_CRS_DV,
};

static unsigned int owl_drv0_17_16_pads[] = {
	ETH_RXD1,
	ETH_RXD0,
};

static unsigned int owl_drv0_15_14_pads[] = {
	ETH_REF_CLK,
};

static unsigned int owl_drv0_13_12_pads[] = {
	ETH_MDC,
	ETH_MDIO,
};

static unsigned int owl_drv0_11_10_pads[] = {
	SIRQ0,
	SIRQ1,
};

static unsigned int owl_drv0_9_8_pads[] = {
	SIRQ2,
};

static unsigned int owl_drv0_7_6_pads[] = {
	I2S_D0,
	I2S_D1,
};

static unsigned int owl_drv0_5_4_pads[] = {
	I2S_LRCLK0,
	I2S_MCLK0,
};

static unsigned int owl_drv0_3_2_pads[] = {
	I2S_BCLK0,
	I2S_BCLK1,
	I2S_LRCLK1,
	I2S_MCLK1,
};

static unsigned int owl_drv0_1_0_pads[] = {
	PCM1_IN,
	PCM1_CLK,
	PCM1_SYNC,
	PCM1_OUT,
};

/* drv1 */
static unsigned int owl_drv1_29_28_pads[] = {
	LVDS_OAP,
	LVDS_OAN,
};

static unsigned int owl_drv1_27_26_pads[] = {
	LVDS_OEP,
	LVDS_OEN,
	LVDS_ODP,
	LVDS_ODN,
};

static unsigned int owl_drv1_25_24_pads[] = {
	LVDS_OCP,
	LVDS_OCN,
	LVDS_OBP,
	LVDS_OBN,
};

static unsigned int owl_drv1_23_22_pads[] = {
	LVDS_EEP,
	LVDS_EEN,
	LVDS_EDP,
	LVDS_EDN,
	LVDS_ECP,
	LVDS_ECN,
	LVDS_EBP,
	LVDS_EBN,
};

static unsigned int owl_drv1_21_20_pads[] = {
	SD0_D3,
	SD0_D2,
	SD0_D1,
	SD0_D0,
};
static unsigned int owl_drv1_19_18_pads[] = {
	SD1_D3,
	SD1_D2,
	SD1_D1,
	SD1_D0,
};

static unsigned int owl_drv1_17_16_pads[] = {
	SD0_CLK,
	SD0_CMD,
	SD1_CLK,
	SD1_CMD,
};

static unsigned int owl_drv1_15_14_pads[] = {
	SPI0_SCLK,
	SPI0_MOSI,
};

static unsigned int owl_drv1_13_12_pads[] = {
	SPI0_SS,
	SPI0_MISO,
};

static unsigned int owl_drv1_11_10_pads[] = {
	UART0_RX,
	UART0_TX,
};

static unsigned int owl_drv1_9_8_pads[] = {
	UART4_RX,
	UART4_TX,
};

static unsigned int owl_drv1_7_6_pads[] = {
	UART2_RX,
	UART2_TX,
	UART2_RTSB,
	UART2_CTSB,
};

static unsigned int owl_drv1_5_4_pads[] = {
	UART3_RX,
	UART3_TX,
	UART3_RTSB,
	UART3_CTSB,
};

/* drv2 */
static unsigned int owl_drv2_31_30_pads[] = {
	I2C0_SCLK,
	I2C0_SDATA,
};

static unsigned int owl_drv2_29_28_pads[] = {
	I2C1_SCLK,
	I2C1_SDATA,
};

static unsigned int owl_drv2_27_26_pads[] = {
	I2C2_SCLK,
	I2C2_SDATA,
};

static unsigned int owl_drv2_21_20_pads[] = {
	SENSOR0_PCLK,
	SENSOR0_CKOUT,
};

/* SR group data */

/* sr0 */
static unsigned int owl_sr0_15_pads[] = {
	SGPIO3,
};

static unsigned int owl_sr0_14_pads[] = {
	SGPIO2,
};

static unsigned int owl_sr0_13_pads[] = {
	SGPIO1,
};

static unsigned int owl_sr0_12_pads[] = {
	SGPIO0,
};

static unsigned int owl_sr0_11_pads[] = {
	ETH_TXD0,
	ETH_TXD1,
};

static unsigned int owl_sr0_10_pads[] = {
	ETH_TXEN,
	ETH_RXER,
};

static unsigned int owl_sr0_9_pads[] = {
	ETH_CRS_DV,
};

static unsigned int owl_sr0_8_pads[] = {
	ETH_RXD1,
	ETH_RXD0,
};

static unsigned int owl_sr0_7_pads[] = {
	ETH_REF_CLK,
};

static unsigned int owl_sr0_6_pads[] = {
	ETH_MDC,
	ETH_MDIO,
};

static unsigned int owl_sr0_5_pads[] = {
	SIRQ0,
	SIRQ1,
};

static unsigned int owl_sr0_4_pads[] = {
	SIRQ2,
};

static unsigned int owl_sr0_3_pads[] = {
	I2S_D0,
	I2S_D1,
};

static unsigned int owl_sr0_2_pads[] = {
	I2S_LRCLK0,
	I2S_MCLK0,
};

static unsigned int owl_sr0_1_pads[] = {
	I2S_BCLK0,
	I2S_BCLK1,
	I2S_LRCLK1,
	I2S_MCLK1,
};

static unsigned int owl_sr0_0_pads[] = {
	PCM1_IN,
	PCM1_CLK,
	PCM1_SYNC,
	PCM1_OUT,
};

/* sr1 */
static unsigned int owl_sr1_25_pads[] = {
	SD1_D3,
	SD1_D2,
	SD1_D1,
	SD1_D0,
};

static unsigned int owl_sr1_24_pads[] = {
	SD0_CLK,
	SD0_CMD,
	SD1_CLK,
	SD1_CMD,
};

static unsigned int owl_sr1_23_pads[] = {
	SPI0_SCLK,
	SPI0_MOSI,
};

static unsigned int owl_sr1_22_pads[] = {
	SPI0_SS,
	SPI0_MISO,
};

static unsigned int owl_sr1_21_pads[] = {
	UART0_RX,
	UART0_TX,
};

static unsigned int owl_sr1_20_pads[] = {
	UART4_RX,
	UART4_TX,
};

static unsigned int owl_sr1_19_pads[] = {
	UART2_RX,
	UART2_TX,
	UART2_RTSB,
	UART2_CTSB,
};

static unsigned int owl_sr1_18_pads[] = {
	UART3_RX,
	UART3_TX,
	UART3_RTSB,
	UART3_CTSB,
};

/* sr2 */
static unsigned int owl_sr2_31_pads[] = {
	I2C0_SCLK,
	I2C0_SDATA,
};

static unsigned int owl_sr2_30_pads[] = {
	I2C1_SCLK,
	I2C1_SDATA,
};

static unsigned int owl_sr2_29_pads[] = {
	I2C2_SCLK,
	I2C2_SDATA,
};

static unsigned int owl_sr2_25_pads[] = {
	SENSOR0_PCLK,
	SENSOR0_CKOUT,
};

#define MUX_PG(group_name, reg, shift, width)				\
	{								\
		.name = #group_name,					\
		.pads = owl_##group_name##_pads,			\
		.npads = ARRAY_SIZE(owl_##group_name##_pads),		\
		.funcs = owl_##group_name##_funcs,			\
		.nfuncs = ARRAY_SIZE(owl_##group_name##_funcs),		\
		.mfpctl_reg  = MFCTL##reg,				\
		.mfpctl_shift = shift,					\
		.mfpctl_width = width,					\
		.drv_reg = -1,						\
		.drv_shift = -1,					\
		.drv_width = -1,					\
		.sr_reg = -1,						\
		.sr_shift = -1,						\
		.sr_width = -1,						\
	}

#define DRV_PG(group_name, reg, shift, width)				\
	{								\
		.name = #group_name,					\
		.pads = owl_##group_name##_pads,			\
		.npads = ARRAY_SIZE(owl_##group_name##_pads),		\
		.mfpctl_reg  = -1,					\
		.mfpctl_shift = -1,					\
		.mfpctl_width = -1,					\
		.drv_reg = PAD_DRV##reg,				\
		.drv_shift = shift,					\
		.drv_width = width,					\
		.sr_reg = -1,						\
		.sr_shift = -1,						\
		.sr_width = -1,						\
	}

#define SR_PG(group_name, reg, shift, width)			\
	{								\
		.name = #group_name,					\
		.pads = owl_##group_name##_pads,			\
		.npads = ARRAY_SIZE(owl_##group_name##_pads),		\
		.mfpctl_reg  = -1,					\
		.mfpctl_shift = -1,					\
		.mfpctl_width = -1,					\
		.drv_reg = -1,						\
		.drv_shift = -1,					\
		.drv_width = -1,					\
		.sr_reg = PAD_SR##reg,				\
		.sr_shift = shift,					\
		.sr_width = width,					\
	}

/* Pinctrl groups */
static const struct owl_pingroup s900_groups[] = {
	MUX_PG(mfp0_22, 0, 22, 1),
	MUX_PG(mfp0_21_20_eth_mdc, 0, 20, 2),
	MUX_PG(mfp0_21_20_eth_mdio, 0, 20, 2),
	MUX_PG(mfp0_19_sirq0, 0, 19, 1),
	MUX_PG(mfp0_19_sirq1, 0, 19, 1),
	MUX_PG(mfp0_18_16_eth_txd0, 0, 16, 3),
	MUX_PG(mfp0_18_16_eth_txd1, 0, 16, 3),
	MUX_PG(mfp0_15_13_eth_txen, 0, 13, 3),
	MUX_PG(mfp0_15_13_eth_rxer, 0, 13, 3),
	MUX_PG(mfp0_12_11, 0, 11, 2),
	MUX_PG(mfp0_10_8_eth_rxd1, 0, 8, 3),
	MUX_PG(mfp0_10_8_eth_rxd0, 0, 8, 3),
	MUX_PG(mfp0_7_6, 0, 6, 2),
	MUX_PG(mfp0_5_i2s_d0, 0, 5, 1),
	MUX_PG(mfp0_5_i2s_d1, 0, 5, 1),
	MUX_PG(mfp0_4_3, 0, 3, 2),
	MUX_PG(mfp0_2_i2s0, 0, 2, 1),
	MUX_PG(mfp0_2_i2s1, 0, 2, 1),
	MUX_PG(mfp0_1_0_pcm1_in_out, 0, 0, 2),
	MUX_PG(mfp0_1_0_pcm1_clk, 0, 0, 2),
	MUX_PG(mfp0_1_0_pcm1_sync, 0, 0, 2),
	MUX_PG(mfp1_31_29_eram_a5, 1, 29, 3),
	MUX_PG(mfp1_31_29_eram_a6, 1, 29, 3),
	MUX_PG(mfp1_31_29_eram_a7, 1, 29, 3),
	MUX_PG(mfp1_28_26_eram_a8, 1, 26, 3),
	MUX_PG(mfp1_28_26_eram_a9, 1, 26, 3),
	MUX_PG(mfp1_28_26_eram_a10, 1, 26, 3),
	MUX_PG(mfp1_25_23, 1, 23, 3),
	MUX_PG(mfp1_22_lvds_oep_odn, 1, 22, 1),
	MUX_PG(mfp1_22_lvds_ocp_obn, 1, 22, 1),
	MUX_PG(mfp1_22_lvds_oap_oan, 1, 22, 1),
	MUX_PG(mfp1_21_lvds_e, 1, 21, 1),
	MUX_PG(mfp1_5_4, 1, 4, 2),
	MUX_PG(mfp1_3_1_spi0_ss, 1, 1, 3),
	MUX_PG(mfp1_3_1_spi0_miso, 1, 1, 3),
	MUX_PG(mfp2_23, 2, 23, 1),
	MUX_PG(mfp2_22, 2, 22, 1),
	MUX_PG(mfp2_21, 2, 21, 1),
	MUX_PG(mfp2_20, 2, 20, 1),
	MUX_PG(mfp2_19_17, 2, 17, 3),
	MUX_PG(mfp2_16_14, 2, 14, 3),
	MUX_PG(mfp2_13_11, 2, 11, 3),
	MUX_PG(mfp2_10_9, 2, 9, 2),
	MUX_PG(mfp2_8_7, 2, 7, 2),
	MUX_PG(mfp2_6_5, 2, 5, 2),
	MUX_PG(mfp2_4_3, 2, 3, 2),
	MUX_PG(mfp2_2_0, 2, 0, 3),
	MUX_PG(mfp3_27, 3, 27, 1),
	MUX_PG(mfp3_21_19, 3, 19, 3),
	MUX_PG(mfp3_18_16, 3, 16, 3),
	MUX_PG(mfp3_15, 3, 15, 1),
	MUX_PG(mfp3_14, 3, 14, 1),
	MUX_PG(mfp3_13, 3, 13, 1),
	MUX_PG(mfp3_12_dsi_dp3_dn1, 3, 12, 1),
	MUX_PG(mfp3_12_dsi_cp_dn0, 3, 12, 1),
	MUX_PG(mfp3_12_dsi_dp2_dn2, 3, 12, 1),
	MUX_PG(mfp3_11, 3, 11, 1),
	MUX_PG(mfp3_10_nand1_ceb3, 3, 10, 1),
	MUX_PG(mfp3_10_nand1_ceb0, 3, 10, 1),
	MUX_PG(mfp3_9, 3, 9, 1),
	MUX_PG(mfp3_8, 3, 8, 1),

	DRV_PG(drv0_31_30, 0, 30, 2),
	DRV_PG(drv0_29_28, 0, 28, 2),
	DRV_PG(drv0_27_26, 0, 26, 2),
	DRV_PG(drv0_25_24, 0, 24, 2),
	DRV_PG(drv0_23_22, 0, 22, 2),
	DRV_PG(drv0_21_20, 0, 20, 2),
	DRV_PG(drv0_19_18, 0, 18, 2),
	DRV_PG(drv0_17_16, 0, 16, 2),
	DRV_PG(drv0_15_14, 0, 14, 2),
	DRV_PG(drv0_13_12, 0, 12, 2),
	DRV_PG(drv0_11_10, 0, 10, 2),
	DRV_PG(drv0_9_8, 0, 8, 2),
	DRV_PG(drv0_7_6, 0, 6, 2),
	DRV_PG(drv0_5_4, 0, 4, 2),
	DRV_PG(drv0_3_2, 0, 2, 2),
	DRV_PG(drv0_1_0, 0, 0, 2),
	DRV_PG(drv1_29_28, 1, 28, 2),
	DRV_PG(drv1_27_26, 1, 26, 2),
	DRV_PG(drv1_25_24, 1, 24, 2),
	DRV_PG(drv1_23_22, 1, 22, 2),
	DRV_PG(drv1_21_20, 1, 20, 2),
	DRV_PG(drv1_19_18, 1, 18, 2),
	DRV_PG(drv1_17_16, 1, 16, 2),
	DRV_PG(drv1_15_14, 1, 14, 2),
	DRV_PG(drv1_13_12, 1, 12, 2),
	DRV_PG(drv1_11_10, 1, 10, 2),
	DRV_PG(drv1_9_8, 1, 8, 2),
	DRV_PG(drv1_7_6, 1, 6, 2),
	DRV_PG(drv1_5_4, 1, 4, 2),
	DRV_PG(drv2_31_30, 2, 30, 2),
	DRV_PG(drv2_29_28, 2, 28, 2),
	DRV_PG(drv2_27_26, 2, 26, 2),
	DRV_PG(drv2_21_20, 2, 20, 2),

	SR_PG(sr0_15, 0, 15, 1),
	SR_PG(sr0_14, 0, 14, 1),
	SR_PG(sr0_13, 0, 13, 1),
	SR_PG(sr0_12, 0, 12, 1),
	SR_PG(sr0_11, 0, 11, 1),
	SR_PG(sr0_10, 0, 10, 1),
	SR_PG(sr0_9, 0, 9, 1),
	SR_PG(sr0_8, 0, 8, 1),
	SR_PG(sr0_7, 0, 7, 1),
	SR_PG(sr0_6, 0, 6, 1),
	SR_PG(sr0_5, 0, 5, 1),
	SR_PG(sr0_4, 0, 4, 1),
	SR_PG(sr0_3, 0, 3, 1),
	SR_PG(sr0_2, 0, 2, 1),
	SR_PG(sr0_1, 0, 1, 1),
	SR_PG(sr0_0, 0, 0, 1),
	SR_PG(sr1_25, 1, 25, 1),
	SR_PG(sr1_24, 1, 24, 1),
	SR_PG(sr1_23, 1, 23, 1),
	SR_PG(sr1_22, 1, 22, 1),
	SR_PG(sr1_21, 1, 21, 1),
	SR_PG(sr1_20, 1, 20, 1),
	SR_PG(sr1_19, 1, 19, 1),
	SR_PG(sr1_18, 1, 18, 1),
	SR_PG(sr2_31, 2, 31, 1),
	SR_PG(sr2_30, 2, 30, 1),
	SR_PG(sr2_29, 2, 29, 1),
	SR_PG(sr2_25, 2, 25, 1),
};

static const char * const eram_groups[] = {
	"mfp0_22",
	"mfp1_31_29_eram_a5",
	"mfp1_31_29_eram_a6",
	"mfp1_31_29_eram_a7",
	"mfp1_28_26_eram_a8",
	"mfp1_28_26_eram_a9",
	"mfp1_28_26_eram_a10",
	"mfp1_25_23",
	"mfp1_22_lvds_oap_oan",
	"mfp1_21_lvds_e",
	"mfp1_5_4",
	"mfp1_3_1_spi0_ss",
	"mfp1_3_1_spi0_miso",
	"mfp2_19_17",
	"mfp2_16_14",
	"mfp2_13_11",
	"mfp2_10_9",
	"mfp2_8_7",
	"mfp2_6_5",
	"mfp2_4_3",
};

static const char * const eth_rmii_groups[] = {
	"mfp0_21_20_eth_mdc",
	"mfp0_21_20_eth_mdio",
	"mfp0_18_16_eth_txd0",
	"mfp0_18_16_eth_txd1",
	"mfp0_15_13_eth_txen",
	"mfp0_15_13_eth_rxer",
	"mfp0_12_11",
	"mfp0_10_8_eth_rxd1",
	"mfp0_10_8_eth_rxd0",
	"mfp0_7_6",
	"eth_smi_dummy",
};

static const char * const eth_smii_groups[] = {
	"mfp0_18_16_eth_txd0",
	"mfp0_18_16_eth_txd1",
	"mfp0_12_11",
	"eth_smi_dummy",
};

static const char * const spi0_groups[] = {
	"mfp1_5_4",
	"mfp1_3_1_spi0_ss",
	"mfp1_3_1_spi0_miso",
	"mfp1_5_4",
	"mfp1_3_1_spi0_ss",
	"mfp1_3_1_spi0_miso",
};

static const char * const spi1_groups[] = {
	"mfp0_1_0_pcm1_in_out",
	"mfp0_1_0_pcm1_clk",
	"mfp0_1_0_pcm1_sync",
	"mfp2_2_0",
	"mfp3_21_19",
	"mfp3_18_16",
};

static const char * const spi2_groups[] = {
	"mfp0_18_16_eth_txd0",
	"mfp0_18_16_eth_txd1",
	"mfp0_12_11",
	"mfp0_7_6",
};

static const char * const spi3_groups[] = {
	"mfp0_15_13_eth_txen",
	"mfp0_15_13_eth_rxer",
};

static const char * const sens0_groups[] = {
	"mfp0_18_16_eth_txd0",
	"mfp0_18_16_eth_txd1",
	"mfp0_15_13_eth_txen",
	"mfp0_15_13_eth_rxer",
	"mfp0_10_8_eth_rxd1",
	"mfp0_10_8_eth_rxd0",
	"mfp1_31_29_eram_a5",
	"mfp1_31_29_eram_a6",
	"mfp1_31_29_eram_a7",
	"mfp1_28_26_eram_a8",
	"mfp1_28_26_eram_a9",
	"mfp3_15_pads",
	"mfp3_14_pads",
	"mfp3_13_pads",
	"mfp3_9_pads",
};

static const char * const uart0_groups[] = {
	"mfp2_23",
	"mfp2_22",
	"mfp2_2_0",
	"mfp3_21_19",
};

static const char * const uart1_groups[] = {
	"mfp2_13_11",
	"mfp3_18_16",
};

static const char * const uart2_groups[] = {
	"mfp0_21_20_eth_mdc",
	"mfp0_21_20_eth_mdio",
	"mfp0_15_13_eth_txen",
	"mfp0_15_13_eth_rxer",
	"mfp0_10_8_eth_rxd1",
	"mfp0_10_8_eth_rxd0",
	"mfp1_22_lvds_oep_odn",
	"mfp2_23",
	"mfp2_22",
	"mfp2_19_17",
	"mfp2_16_14",
	"mfp2_13_11",
	"mfp2_2_0",
	"mfp3_21_19_pads",
	"mfp3_18_16_pads",
	"mfp3_12_dsi_dp3_dn1",
	"uart2_dummy"
};

static const char * const uart3_groups[] = {
	"mfp2_21",
	"mfp2_20",
	"uart3_dummy"
};

static const char * const uart4_groups[] = {
	"mfp0_22",
	"mfp0_12_11",
	"mfp0_7_6",
	"mfp0_1_0_pcm1_in_out",
	"mfp0_1_0_pcm1_clk",
	"mfp0_1_0_pcm1_sync",
	"mfp1_31_29_eram_a5",
	"mfp1_31_29_eram_a6",
	"mfp3_12_dsi_dp2_dn2",
	"mfp3_8_pads",
	"uart4_dummy"
};

static const char * const uart5_groups[] = {
	"mfp0_10_8_eth_rxd1",
	"mfp0_10_8_eth_rxd0",
	"mfp1_28_26_eram_a9",
	"mfp1_25_23",
	"mfp2_21",
	"mfp2_20",
	"mfp2_19_17",
	"mfp2_16_14",
};

static const char * const uart6_groups[] = {
	"mfp0_18_16_eth_txd0",
	"mfp0_18_16_eth_txd1",
};

static const char * const i2s0_groups[] = {
	"mfp0_5_i2s_d0",
	"mfp0_4_3",
	"mfp0_2_i2s0",
	"i2s0_dummy",
};

static const char * const i2s1_groups[] = {
	"mfp0_5_i2s_d1",
	"mfp0_2_i2s1",
	"mfp1_3_1_spi0_ss",
	"mfp1_3_1_spi0_miso",
	"mfp2_2_0",
	"mfp3_21_19",
	"i2s1_dummy",
};

static const char * const pcm0_groups[] = {
	"_mfp0_5_i2s_d0",
	"_mfp0_5_i2s_d1",
	"_mfp0_4_3",
	"_mfp0_2_i2s0",
	"_mfp0_2_i2s1",
	"_mfp1_5_4",
	"_mfp1_3_1_spi0_ss",
	"_mfp1_3_1_spi0_miso",
};

static const char * const pcm1_groups[] = {
	"mfp0_4_3",
	"mfp0_1_0_pcm1_in_out",
	"mfp0_1_0_pcm1_clk",
	"mfp0_1_0_pcm1_sync",
	"mfp1_22_lvds_oep_odn",
	"mfp1_3_1_spi0_ss",
	"mfp1_3_1_spi0_miso",
	"mfp2_2_0",
	"mfp3_21_19",
	"mfp3_12_dsi_cp_dn0",
	"pcm1_dummy",
};

static const char * const jtag_groups[] = {
	"mfp1_31_29_eram_a5",
	"mfp1_31_29_eram_a6",
	"mfp1_31_29_eram_a7",
	"mfp1_28_26_eram_a8",
	"mfp1_28_26_eram_a10",
	"mfp1_28_26_eram_a10",
	"mfp2_13_11",
	"mfp2_8_7",
	"mfp2_6_5",
};

static const char * const pwm0_groups[] = {
	"owl_mfp0_19_sirq0",
	"owl_mfp0_18_16_eth_txd0",
	"owl_mfp0_10_8_eth_rxd1",
	"owl_mfp1_31_29_eram_a5",
	"owl_mfp3_10_nand1_ceb3",
};

static const char * const pwm1_groups[] = {
	"mfp0_19_sirq1",
	"mfp0_18_16_eth_txd1",
	"mfp0_10_8_eth_rxd0",
	"mfp1_31_29_eram_a6",
	"mfp1_28_26_eram_a8",
	"mfp3_10_nand1_ceb0",
};

static const char * const pwm2_groups[] = {
	"mfp0_21_20_eth_mdc",
	"mfp0_15_13_eth_txen",
	"mfp1_28_26_eram_a9",
	"mfp1_25_23",
};

static const char * const pwm3_groups[] = {
	"mfp0_21_20_eth_mdio",
	"mfp0_15_13_eth_rxer",
	"mfp1_28_26_eram_a10",
};

static const char * const pwm4_groups[] = {
	"mfp0_1_0_pcm1_clk",
	"mfp1_3_1_spi0_ss",
};

static const char * const pwm5_groups[] = {
	"mfp0_1_0_pcm1_sync",
	"mfp1_3_1_spi0_miso",
};

static const char * const sd0_groups[] = {
	"mfp2_19_17",
	"mfp2_16_14",
	"mfp2_13_11",
	"mfp2_8_7",
	"mfp2_6_5",
};

static const char * const sd1_groups[] = {
	"mfp2_10_9",
	"mfp2_4_3",
	"sd1_dummy",
};

static const char * const sd2_groups[] = {
	"mfp3_27",
};

static const char * const sd3_groups[] = {
	"mfp3_11",
};

static const char * const i2c0_groups[] = {
	"mfp3_18_16",
};

static const char * const i2c1_groups[] = {
	"mfp3_18_16",
	"i2c1_dummy"
};

static const char * const i2c2_groups[] = {
	"i2c2_dummy"
};

static const char * const i2c3_groups[] = {
	"mfp0_1_0",
	"mfp1_5_4",
};

static const char * const i2c4_groups[] = {
	"mfp3_8",
};

static const char * const i2c5_groups[] = {
	"mfp2_2_0",
	"mfp3_21_19",
};


static const char * const lvds_groups[] = {
	"mfp1_22_lvds_oep_odn",
	"mfp1_22_lvds_ocp_obn",
	"mfp1_22_lvds_oap_oan",
	"mfp1_21_lvds_e",
};

static const char * const usb20_groups[] = {
	"mfp1_28_26_eram_a9",
};

static const char * const usb30_groups[] = {
	"mfp1_28_26_eram_a10",
};

static const char * const gpu_groups[] = {
	"mfp2_19_17",
	"mfp2_16_14",
	"mfp2_13_11",
	"mfp2_8_7",
	"mfp2_6_5",
};

static const char * const mipi_csi0_groups[] = {
	"mfp3_14",
};

static const char * const mipi_csi1_groups[] = {
	"mfp3_13",
};

static const char * const mipi_dsi_groups[] = {
	"mfp3_12_dsi_dp3_dn1",
	"mfp3_12_dsi_cp_dn0",
	"mfp3_12_dsi_dp2_dn2",
	"mipi_dsi_dummy",
};

static const char * const nand0_groups[] = {
	"mfp3_27",
	"nand0_dummy",
};

static const char * const nand1_groups[] = {
	"mfp3_11",
	"mfp3_10_nand1_ceb3",
	"mfp3_10_nand1_ceb0",
	"nand1_dummy",
};

static const char * const spdif_groups[] = {
	"mfp3_21_19",
};

static const char * const lens_groups[] = {
	"mfp3_11_10",
	"mfp3_9_7",
	"mfp3_6_4",
	"mfp3_3_2",
	"mfp3_1_0",
};

static const char * const sirq0_groups[] = {
	"mfp0_19_sirq0",
	"sirq0_dummy",
};

static const char * const sirq1_groups[] = {
	"mfp0_19_sirq1",
	"sirq1_dummy",
};

static const char * const sirq2_groups[] = {
	"sirq2_dummy",
};

#define FUNCTION(fname)					\
	{						\
		.name = #fname,				\
		.groups = fname##_groups,		\
		.ngroups = ARRAY_SIZE(fname##_groups),	\
	}

const struct owl_pinmux_func s900_functions[] = {
	[S900_MUX_ERAM] = FUNCTION(eram),
	[S900_MUX_ETH_RMII] = FUNCTION(eth_rmii),
	[S900_MUX_ETH_SMII] = FUNCTION(eth_smii),
	[S900_MUX_SPI0] = FUNCTION(spi0),
	[S900_MUX_SPI1] = FUNCTION(spi1),
	[S900_MUX_SPI2] = FUNCTION(spi2),
	[S900_MUX_SPI3] = FUNCTION(spi3),
	[S900_MUX_SENS0] = FUNCTION(sens0),
	[S900_MUX_UART0] = FUNCTION(uart0),
	[S900_MUX_UART1] = FUNCTION(uart1),
	[S900_MUX_UART2] = FUNCTION(uart2),
	[S900_MUX_UART3] = FUNCTION(uart3),
	[S900_MUX_UART4] = FUNCTION(uart4),
	[S900_MUX_UART5] = FUNCTION(uart5),
	[S900_MUX_UART6] = FUNCTION(uart6),
	[S900_MUX_I2S0] = FUNCTION(i2s0),
	[S900_MUX_I2S1] = FUNCTION(i2s1),
	[S900_MUX_PCM0] = FUNCTION(pcm0),
	[S900_MUX_PCM1] = FUNCTION(pcm1),
	[S900_MUX_JTAG] = FUNCTION(jtag),
	[S900_MUX_PWM0] = FUNCTION(pwm0),
	[S900_MUX_PWM1] = FUNCTION(pwm1),
	[S900_MUX_PWM2] = FUNCTION(pwm2),
	[S900_MUX_PWM3] = FUNCTION(pwm3),
	[S900_MUX_PWM4] = FUNCTION(pwm4),
	[S900_MUX_PWM5] = FUNCTION(pwm5),
	[S900_MUX_SD0] = FUNCTION(sd0),
	[S900_MUX_SD1] = FUNCTION(sd1),
	[S900_MUX_SD2] = FUNCTION(sd2),
	[S900_MUX_SD3] = FUNCTION(sd3),
	[S900_MUX_I2C0] = FUNCTION(i2c0),
	[S900_MUX_I2C1] = FUNCTION(i2c1),
	[S900_MUX_I2C2] = FUNCTION(i2c2),
	[S900_MUX_I2C3] = FUNCTION(i2c3),
	[S900_MUX_I2C4] = FUNCTION(i2c4),
	[S900_MUX_I2C5] = FUNCTION(i2c5),
	[S900_MUX_LVDS] = FUNCTION(lvds),
	[S900_MUX_USB30] = FUNCTION(usb30),
	[S900_MUX_USB20] = FUNCTION(usb20),
	[S900_MUX_GPU] = FUNCTION(gpu),
	[S900_MUX_MIPI_CSI0] = FUNCTION(mipi_csi0),
	[S900_MUX_MIPI_CSI1] = FUNCTION(mipi_csi1),
	[S900_MUX_MIPI_DSI] = FUNCTION(mipi_dsi),
	[S900_MUX_NAND0] = FUNCTION(nand0),
	[S900_MUX_NAND1] = FUNCTION(nand1),
	[S900_MUX_SPDIF] = FUNCTION(spdif),
	[S900_MUX_SIRQ0] = FUNCTION(sirq0),
	[S900_MUX_SIRQ1] = FUNCTION(sirq1),
	[S900_MUX_SIRQ2] = FUNCTION(sirq2),
};
/* PAD PULL UP/DOWN CONFIGURES */
#define PULLCTL_CONF(pull_reg, pull_sft, pull_wdt)			\
	{								\
		.reg = PAD_PULLCTL##pull_reg,				\
		.shift = pull_sft,					\
		.width = pull_wdt,					\
	}

#define PAD_PULLCTL_CONF(pad_name, pull_reg, pull_sft, pull_wdt)	\
	struct owl_pullctl pad_name##_pullctl_conf			\
		= PULLCTL_CONF(pull_reg, pull_sft, pull_wdt)

#define ST_CONF(st_reg, st_sft, st_wdt)					\
	{								\
		.reg = PAD_ST##st_reg,					\
		.shift = st_sft,					\
		.width = st_wdt,					\
	}

#define PAD_ST_CONF(pad_name, st_reg, st_sft, st_wdt)			\
	struct owl_st pad_name##_st_conf				\
		= ST_CONF(st_reg, st_sft, st_wdt)

/* PAD_PULLCTL0 */
static PAD_PULLCTL_CONF(ETH_RXER, 0, 18, 2);
static PAD_PULLCTL_CONF(SIRQ0, 0, 16, 2);
static PAD_PULLCTL_CONF(SIRQ1, 0, 14, 2);
static PAD_PULLCTL_CONF(SIRQ2, 0, 12, 2);
static PAD_PULLCTL_CONF(I2C0_SDATA, 0, 10, 2);
static PAD_PULLCTL_CONF(I2C0_SCLK, 0, 8, 2);
static PAD_PULLCTL_CONF(ERAM_A5, 0, 6, 2);
static PAD_PULLCTL_CONF(ERAM_A6, 0, 4, 2);
static PAD_PULLCTL_CONF(ERAM_A7, 0, 2, 2);
static PAD_PULLCTL_CONF(ERAM_A10, 0, 0, 2);

/* PAD_PULLCTL1 */
static PAD_PULLCTL_CONF(PCM1_IN, 1, 30, 2);
static PAD_PULLCTL_CONF(PCM1_OUT, 1, 28, 2);
static PAD_PULLCTL_CONF(SD0_D0, 1, 26, 2);
static PAD_PULLCTL_CONF(SD0_D1, 1, 24, 2);
static PAD_PULLCTL_CONF(SD0_D2, 1, 22, 2);
static PAD_PULLCTL_CONF(SD0_D3, 1, 20, 2);
static PAD_PULLCTL_CONF(SD0_CMD, 1, 18, 2);
static PAD_PULLCTL_CONF(SD0_CLK, 1, 16, 2);
static PAD_PULLCTL_CONF(SD1_CMD, 1, 14, 2);
static PAD_PULLCTL_CONF(SD1_D0, 1, 12, 2);
static PAD_PULLCTL_CONF(SD1_D1, 1, 10, 2);
static PAD_PULLCTL_CONF(SD1_D2, 1, 8, 2);
static PAD_PULLCTL_CONF(SD1_D3, 1, 6, 2);
static PAD_PULLCTL_CONF(UART0_RX, 1, 4, 2);
static PAD_PULLCTL_CONF(UART0_TX, 1, 2, 2);

/* PAD_PULLCTL2 */
static PAD_PULLCTL_CONF(I2C2_SDATA, 2, 26, 2);
static PAD_PULLCTL_CONF(I2C2_SCLK, 2, 24, 2);
static PAD_PULLCTL_CONF(SPI0_SCLK, 2, 22, 2);
static PAD_PULLCTL_CONF(SPI0_MOSI, 2, 20, 2);
static PAD_PULLCTL_CONF(I2C1_SDATA, 2, 18, 2);
static PAD_PULLCTL_CONF(I2C1_SCLK, 2, 16, 2);
static PAD_PULLCTL_CONF(NAND0_D0, 2, 15, 1);
static PAD_PULLCTL_CONF(NAND0_D1, 2, 15, 1);
static PAD_PULLCTL_CONF(NAND0_D2, 2, 15, 1);
static PAD_PULLCTL_CONF(NAND0_D3, 2, 15, 1);
static PAD_PULLCTL_CONF(NAND0_D4, 2, 15, 1);
static PAD_PULLCTL_CONF(NAND0_D5, 2, 15, 1);
static PAD_PULLCTL_CONF(NAND0_D6, 2, 15, 1);
static PAD_PULLCTL_CONF(NAND0_D7, 2, 15, 1);
static PAD_PULLCTL_CONF(NAND0_DQSN, 2, 14, 1);
static PAD_PULLCTL_CONF(NAND0_DQS, 2, 13, 1);
static PAD_PULLCTL_CONF(NAND1_D0, 2, 12, 1);
static PAD_PULLCTL_CONF(NAND1_D1, 2, 12, 1);
static PAD_PULLCTL_CONF(NAND1_D2, 2, 12, 1);
static PAD_PULLCTL_CONF(NAND1_D3, 2, 12, 1);
static PAD_PULLCTL_CONF(NAND1_D4, 2, 12, 1);
static PAD_PULLCTL_CONF(NAND1_D5, 2, 12, 1);
static PAD_PULLCTL_CONF(NAND1_D6, 2, 12, 1);
static PAD_PULLCTL_CONF(NAND1_D7, 2, 12, 1);
static PAD_PULLCTL_CONF(NAND1_DQSN, 2, 11, 1);
static PAD_PULLCTL_CONF(NAND1_DQS, 2, 10, 1);
static PAD_PULLCTL_CONF(SGPIO2, 2, 8, 2);
static PAD_PULLCTL_CONF(SGPIO3, 2, 6, 2);
static PAD_PULLCTL_CONF(UART4_RX, 2, 4, 2);
static PAD_PULLCTL_CONF(UART4_TX, 2, 2, 2);

/* PAD_ST0 */
static PAD_ST_CONF(I2C0_SDATA, 0, 30, 1);
static PAD_ST_CONF(UART0_RX, 0, 29, 1);
static PAD_ST_CONF(ETH_MDC, 0, 28, 1);
static PAD_ST_CONF(I2S_MCLK1, 0, 23, 1);
static PAD_ST_CONF(ETH_REF_CLK, 0, 22, 1);
static PAD_ST_CONF(ETH_TXEN, 0, 21, 1);
static PAD_ST_CONF(ETH_TXD0, 0, 20, 1);
static PAD_ST_CONF(I2S_LRCLK1, 0, 19, 1);
static PAD_ST_CONF(SGPIO2, 0, 18, 1);
static PAD_ST_CONF(SGPIO3, 0, 17, 1);
static PAD_ST_CONF(UART4_TX, 0, 16, 1);
static PAD_ST_CONF(I2S_D1, 0, 15, 1);
static PAD_ST_CONF(UART0_TX, 0, 14, 1);
static PAD_ST_CONF(SPI0_SCLK, 0, 13, 1);
static PAD_ST_CONF(SD0_CLK, 0, 12, 1);
static PAD_ST_CONF(ERAM_A5, 0, 11, 1);
static PAD_ST_CONF(I2C0_SCLK, 0, 7, 1);
static PAD_ST_CONF(ERAM_A9, 0, 6, 1);
static PAD_ST_CONF(LVDS_OEP, 0, 5, 1);
static PAD_ST_CONF(LVDS_ODN, 0, 4, 1);
static PAD_ST_CONF(LVDS_OAP, 0, 3, 1);
static PAD_ST_CONF(I2S_BCLK1, 0, 2, 1);

/* PAD_ST1 */
static PAD_ST_CONF(I2S_LRCLK0, 1, 29, 1);
static PAD_ST_CONF(UART4_RX, 1, 28, 1);
static PAD_ST_CONF(UART3_CTSB, 1, 27, 1);
static PAD_ST_CONF(UART3_RTSB, 1, 26, 1);
static PAD_ST_CONF(UART3_RX, 1, 25, 1);
static PAD_ST_CONF(UART2_RTSB, 1, 24, 1);
static PAD_ST_CONF(UART2_CTSB, 1, 23, 1);
static PAD_ST_CONF(UART2_RX, 1, 22, 1);
static PAD_ST_CONF(ETH_RXD0, 1, 21, 1);
static PAD_ST_CONF(ETH_RXD1, 1, 20, 1);
static PAD_ST_CONF(ETH_CRS_DV, 1, 19, 1);
static PAD_ST_CONF(ETH_RXER, 1, 18, 1);
static PAD_ST_CONF(ETH_TXD1, 1, 17, 1);
static PAD_ST_CONF(LVDS_OCP, 1, 16, 1);
static PAD_ST_CONF(LVDS_OBP, 1, 15, 1);
static PAD_ST_CONF(LVDS_OBN, 1, 14, 1);
static PAD_ST_CONF(PCM1_OUT, 1, 12, 1);
static PAD_ST_CONF(PCM1_CLK, 1, 11, 1);
static PAD_ST_CONF(PCM1_IN, 1, 10, 1);
static PAD_ST_CONF(PCM1_SYNC, 1, 9, 1);
static PAD_ST_CONF(I2C1_SCLK, 1, 8, 1);
static PAD_ST_CONF(I2C1_SDATA, 1, 7, 1);
static PAD_ST_CONF(I2C2_SCLK, 1, 6, 1);
static PAD_ST_CONF(I2C2_SDATA, 1, 5, 1);
static PAD_ST_CONF(SPI0_MOSI, 1, 4, 1);
static PAD_ST_CONF(SPI0_MISO, 1, 3, 1);
static PAD_ST_CONF(SPI0_SS, 1, 2, 1);
static PAD_ST_CONF(I2S_BCLK0, 1, 1, 1);
static PAD_ST_CONF(I2S_MCLK0, 1, 0, 1);

#define PAD_INFO(name)							\
	{								\
		.pad = name,						\
		.pullctl = NULL,					\
		.st = NULL,						\
	}

#define PAD_INFO_ST(name)						\
	{								\
		.pad = name,						\
		.pullctl = NULL,					\
		.st = &name##_st_conf,					\
	}

#define PAD_INFO_PULLCTL(name)						\
	{								\
		.pad = name,						\
		.pullctl = &name##_pullctl_conf,			\
		.st = NULL,						\
	}

#define PAD_INFO_PULLCTL_ST(name)					\
	{								\
		.pad = name,						\
		.pullctl = &name##_pullctl_conf,			\
		.st = &name##_st_conf,					\
	}

/* Pad info table */
struct owl_padinfo s900_padinfo[NUM_PADS] = {
	[ETH_TXD0] = PAD_INFO_ST(ETH_TXD0),
	[ETH_TXD1] = PAD_INFO_ST(ETH_TXD1),
	[ETH_TXEN] = PAD_INFO_ST(ETH_TXEN),
	[ETH_RXER] = PAD_INFO_PULLCTL_ST(ETH_RXER),
	[ETH_CRS_DV] = PAD_INFO_ST(ETH_CRS_DV),
	[ETH_RXD1] = PAD_INFO_ST(ETH_RXD1),
	[ETH_RXD0] = PAD_INFO_ST(ETH_RXD0),
	[ETH_REF_CLK] = PAD_INFO_ST(ETH_REF_CLK),
	[ETH_MDC] = PAD_INFO_ST(ETH_MDC),
	[ETH_MDIO] = PAD_INFO(ETH_MDIO),
	[SIRQ0] = PAD_INFO_PULLCTL(SIRQ0),
	[SIRQ1] = PAD_INFO_PULLCTL(SIRQ1),
	[SIRQ2] = PAD_INFO_PULLCTL(SIRQ2),
	[I2S_D0] = PAD_INFO(I2S_D0),
	[I2S_BCLK0] = PAD_INFO_ST(I2S_BCLK0),
	[I2S_LRCLK0] = PAD_INFO_ST(I2S_LRCLK0),
	[I2S_MCLK0] = PAD_INFO_ST(I2S_MCLK0),
	[I2S_D1] = PAD_INFO_ST(I2S_D1),
	[I2S_BCLK1] = PAD_INFO_ST(I2S_BCLK1),
	[I2S_LRCLK1] = PAD_INFO_ST(I2S_LRCLK1),
	[I2S_MCLK1] = PAD_INFO_ST(I2S_MCLK1),
	[PCM1_IN] = PAD_INFO_PULLCTL_ST(PCM1_IN),
	[PCM1_CLK] = PAD_INFO_ST(PCM1_CLK),
	[PCM1_SYNC] = PAD_INFO_ST(PCM1_SYNC),
	[PCM1_OUT] = PAD_INFO_PULLCTL_ST(PCM1_OUT),
	[ERAM_A5] = PAD_INFO_PULLCTL_ST(ERAM_A5),
	[ERAM_A6] = PAD_INFO_PULLCTL(ERAM_A6),
	[ERAM_A7] = PAD_INFO_PULLCTL(ERAM_A7),
	[ERAM_A8] = PAD_INFO(ERAM_A8),
	[ERAM_A9] = PAD_INFO_ST(ERAM_A9),
	[ERAM_A10] = PAD_INFO_PULLCTL(ERAM_A10),
	[ERAM_A11] = PAD_INFO(ERAM_A11),
	[LVDS_OEP] = PAD_INFO_ST(LVDS_OEP),
	[LVDS_OEN] = PAD_INFO(LVDS_OEN),
	[LVDS_ODP] = PAD_INFO(LVDS_ODP),
	[LVDS_ODN] = PAD_INFO_ST(LVDS_ODN),
	[LVDS_OCP] = PAD_INFO_ST(LVDS_OCP),
	[LVDS_OCN] = PAD_INFO(LVDS_OCN),
	[LVDS_OBP] = PAD_INFO_ST(LVDS_OBP),
	[LVDS_OBN] = PAD_INFO_ST(LVDS_OBN),
	[LVDS_OAP] = PAD_INFO_ST(LVDS_OAP),
	[LVDS_OAN] = PAD_INFO(LVDS_OAN),
	[LVDS_EEP] = PAD_INFO(LVDS_EEP),
	[LVDS_EEN] = PAD_INFO(LVDS_EEN),
	[LVDS_EDP] = PAD_INFO(LVDS_EDP),
	[LVDS_EDN] = PAD_INFO(LVDS_EDN),
	[LVDS_ECP] = PAD_INFO(LVDS_ECP),
	[LVDS_ECN] = PAD_INFO(LVDS_ECN),
	[LVDS_EBP] = PAD_INFO(LVDS_EBP),
	[LVDS_EBN] = PAD_INFO(LVDS_EBN),
	[LVDS_EAP] = PAD_INFO(LVDS_EAP),
	[LVDS_EAN] = PAD_INFO(LVDS_EAN),
	[SD0_D0] = PAD_INFO_PULLCTL(SD0_D0),
	[SD0_D1] = PAD_INFO_PULLCTL(SD0_D1),
	[SD0_D2] = PAD_INFO_PULLCTL(SD0_D2),
	[SD0_D3] = PAD_INFO_PULLCTL(SD0_D3),
	[SD1_D0] = PAD_INFO_PULLCTL(SD1_D0),
	[SD1_D1] = PAD_INFO_PULLCTL(SD1_D1),
	[SD1_D2] = PAD_INFO_PULLCTL(SD1_D2),
	[SD1_D3] = PAD_INFO_PULLCTL(SD1_D3),
	[SD0_CMD] = PAD_INFO_PULLCTL(SD0_CMD),
	[SD0_CLK] = PAD_INFO_PULLCTL_ST(SD0_CLK),
	[SD1_CMD] = PAD_INFO_PULLCTL(SD1_CMD),
	[SD1_CLK] = PAD_INFO(SD1_CLK),
	[SPI0_SCLK] = PAD_INFO_PULLCTL_ST(SPI0_SCLK),
	[SPI0_SS] = PAD_INFO_ST(SPI0_SS),
	[SPI0_MISO] = PAD_INFO_ST(SPI0_MISO),
	[SPI0_MOSI] = PAD_INFO_PULLCTL_ST(SPI0_MOSI),
	[UART0_RX] = PAD_INFO_PULLCTL_ST(UART0_RX),
	[UART0_TX] = PAD_INFO_PULLCTL_ST(UART0_TX),
	[UART2_RX] = PAD_INFO_ST(UART2_RX),
	[UART2_TX] = PAD_INFO(UART2_TX),
	[UART2_RTSB] = PAD_INFO_ST(UART2_RTSB),
	[UART2_CTSB] = PAD_INFO_ST(UART2_CTSB),
	[UART3_RX] = PAD_INFO_ST(UART3_RX),
	[UART3_TX] = PAD_INFO(UART3_TX),
	[UART3_RTSB] = PAD_INFO_ST(UART3_RTSB),
	[UART3_CTSB] = PAD_INFO_ST(UART3_CTSB),
	[UART4_RX] = PAD_INFO_PULLCTL_ST(UART4_RX),
	[UART4_TX] = PAD_INFO_PULLCTL_ST(UART4_TX),
	[I2C0_SCLK] = PAD_INFO_PULLCTL_ST(I2C0_SCLK),
	[I2C0_SDATA] = PAD_INFO_PULLCTL_ST(I2C0_SDATA),
	[I2C1_SCLK] = PAD_INFO_PULLCTL_ST(I2C1_SCLK),
	[I2C1_SDATA] = PAD_INFO_PULLCTL_ST(I2C1_SDATA),
	[I2C2_SCLK] = PAD_INFO_PULLCTL_ST(I2C2_SCLK),
	[I2C2_SDATA] = PAD_INFO_PULLCTL_ST(I2C2_SDATA),
	[CSI0_DN0] = PAD_INFO(CSI0_DN0),
	[CSI0_DP0] = PAD_INFO(CSI0_DP0),
	[CSI0_DN1] = PAD_INFO(CSI0_DN1),
	[CSI0_DP1] = PAD_INFO(CSI0_DP1),
	[CSI0_CN] = PAD_INFO(CSI0_CN),
	[CSI0_CP] = PAD_INFO(CSI0_CP),
	[CSI0_DN2] = PAD_INFO(CSI0_DN2),
	[CSI0_DP2] = PAD_INFO(CSI0_DP2),
	[CSI0_DN3] = PAD_INFO(CSI0_DN3),
	[CSI0_DP3] = PAD_INFO(CSI0_DP3),
	[DSI_DP3] = PAD_INFO(DSI_DP3),
	[DSI_DN3] = PAD_INFO(DSI_DN3),
	[DSI_DP1] = PAD_INFO(DSI_DP1),
	[DSI_DN1] = PAD_INFO(DSI_DN1),
	[DSI_CP] = PAD_INFO(DSI_CP),
	[DSI_CN] = PAD_INFO(DSI_CN),
	[DSI_DP0] = PAD_INFO(DSI_DP0),
	[DSI_DN0] = PAD_INFO(DSI_DN0),
	[DSI_DP2] = PAD_INFO(DSI_DP2),
	[DSI_DN2] = PAD_INFO(DSI_DN2),
	[SENSOR0_PCLK] = PAD_INFO(SENSOR0_PCLK),
	[CSI1_DN0] = PAD_INFO(CSI1_DN0),
	[CSI1_DP0] = PAD_INFO(CSI1_DP0),
	[CSI1_DN1] = PAD_INFO(CSI1_DN1),
	[CSI1_DP1] = PAD_INFO(CSI1_DP1),
	[CSI1_CN] = PAD_INFO(CSI1_CN),
	[CSI1_CP] = PAD_INFO(CSI1_CP),
	[SENSOR0_CKOUT] = PAD_INFO(SENSOR0_CKOUT),
	[NAND0_D0] = PAD_INFO_PULLCTL(NAND0_D0),
	[NAND0_D1] = PAD_INFO_PULLCTL(NAND0_D1),
	[NAND0_D2] = PAD_INFO_PULLCTL(NAND0_D2),
	[NAND0_D3] = PAD_INFO_PULLCTL(NAND0_D3),
	[NAND0_D4] = PAD_INFO_PULLCTL(NAND0_D4),
	[NAND0_D5] = PAD_INFO_PULLCTL(NAND0_D5),
	[NAND0_D6] = PAD_INFO_PULLCTL(NAND0_D6),
	[NAND0_D7] = PAD_INFO_PULLCTL(NAND0_D7),
	[NAND0_DQS] = PAD_INFO_PULLCTL(NAND0_DQS),
	[NAND0_DQSN] = PAD_INFO_PULLCTL(NAND0_DQSN),
	[NAND0_ALE] = PAD_INFO(NAND0_ALE),
	[NAND0_CLE] = PAD_INFO(NAND0_CLE),
	[NAND0_CEB0] = PAD_INFO(NAND0_CEB0),
	[NAND0_CEB1] = PAD_INFO(NAND0_CEB1),
	[NAND0_CEB2] = PAD_INFO(NAND0_CEB2),
	[NAND0_CEB3] = PAD_INFO(NAND0_CEB3),
	[NAND1_D0] = PAD_INFO_PULLCTL(NAND1_D0),
	[NAND1_D1] = PAD_INFO_PULLCTL(NAND1_D1),
	[NAND1_D2] = PAD_INFO_PULLCTL(NAND1_D2),
	[NAND1_D3] = PAD_INFO_PULLCTL(NAND1_D3),
	[NAND1_D4] = PAD_INFO_PULLCTL(NAND1_D4),
	[NAND1_D5] = PAD_INFO_PULLCTL(NAND1_D5),
	[NAND1_D6] = PAD_INFO_PULLCTL(NAND1_D6),
	[NAND1_D7] = PAD_INFO_PULLCTL(NAND1_D7),
	[NAND1_DQS] = PAD_INFO_PULLCTL(NAND1_DQS),
	[NAND1_DQSN] = PAD_INFO_PULLCTL(NAND1_DQSN),
	[NAND1_ALE] = PAD_INFO(NAND1_ALE),
	[NAND1_CLE] = PAD_INFO(NAND1_CLE),
	[NAND1_CEB0] = PAD_INFO(NAND1_CEB0),
	[NAND1_CEB1] = PAD_INFO(NAND1_CEB1),
	[NAND1_CEB2] = PAD_INFO(NAND1_CEB2),
	[NAND1_CEB3] = PAD_INFO(NAND1_CEB3),
	[SGPIO0] = PAD_INFO(SGPIO0),
	[SGPIO1] = PAD_INFO(SGPIO1),
	[SGPIO2] = PAD_INFO_PULLCTL_ST(SGPIO2),
	[SGPIO3] = PAD_INFO_PULLCTL_ST(SGPIO3),
};

static struct owl_pinctrl_soc_data s900_pinctrl_data = {
	.padinfo = s900_padinfo,
	.pins = (const struct pinctrl_pin_desc *)s900_pads,
	.npins = ARRAY_SIZE(s900_pads),
	.functions = s900_functions,
	.nfunctions = ARRAY_SIZE(s900_functions),
	.groups = s900_groups,
	.ngroups = ARRAY_SIZE(s900_groups),
	.ngpios = NUM_GPIOS,
};

static int s900_pinctrl_probe(struct platform_device *pdev)
{
	return owl_pinctrl_probe(pdev, &s900_pinctrl_data);
}

static const struct of_device_id s900_pinctrl_of_match[] = {
	{ .compatible = "actions,s900-pinctrl", },
	{ },
};

static struct platform_driver s900_pinctrl_driver = {
	.driver = {
		.name = "pinctrl-s900",
		.of_match_table = of_match_ptr(s900_pinctrl_of_match),
	},
	.probe = s900_pinctrl_probe,
};

static int __init s900_pinctrl_init(void)
{
	return platform_driver_register(&s900_pinctrl_driver);
}
arch_initcall(s900_pinctrl_init);

static void __exit s900_pinctrl_exit(void)
{
	platform_driver_unregister(&s900_pinctrl_driver);
}
module_exit(s900_pinctrl_exit);

MODULE_AUTHOR("Actions Semi Inc.");
MODULE_AUTHOR("Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>");
MODULE_DESCRIPTION("Actions S900 SoC Pinctrl Driver");
MODULE_LICENSE("GPL v2");
