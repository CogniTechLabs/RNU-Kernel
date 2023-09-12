// SPDX-License-Identifier: GPL-2.0
/*
 * Intel Jasper Lake PCH pinctrl/GPIO driver
 *
 * Copyright (C) 2020, Intel Corporation
 * Author: Andy Shevchenko <andriy.shevchenko@linux.intel.com>
 */

#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include <linux/pinctrl/pinctrl.h>

#include "pinctrl-intel.h"

#define JSL_PAD_OWN	0x020
#define JSL_PADCFGLOCK	0x080
#define JSL_HOSTSW_OWN	0x0c0
#define JSL_GPI_IS	0x100
#define JSL_GPI_IE	0x120

#define JSL_GPP(r, s, e, g)				\
	{						\
		.reg_num = (r),				\
		.base = (s),				\
		.size = ((e) - (s) + 1),		\
		.gpio_base = (g),			\
	}

#define JSL_COMMUNITY(b, s, e, g)			\
	INTEL_COMMUNITY_GPPS(b, s, e, g, JSL)

/* Jasper Lake */
static const struct pinctrl_pin_desc jsl_pins[] = {
	/* GPP_F */
	PINCTRL_PIN(0, "CNV_BRI_DT_UART0_RTSB"),
	PINCTRL_PIN(1, "CNV_BRI_RSP_UART0_RXD"),
	PINCTRL_PIN(2, "EMMC_HIP_MON"),
	PINCTRL_PIN(3, "CNV_RGI_RSP_UART0_CTSB"),
	PINCTRL_PIN(4, "CNV_RF_RESET_B"),
	PINCTRL_PIN(5, "MODEM_CLKREQ"),
	PINCTRL_PIN(6, "CNV_PA_BLANKING"),
	PINCTRL_PIN(7, "EMMC_CMD"),
	PINCTRL_PIN(8, "EMMC_DATA0"),
	PINCTRL_PIN(9, "EMMC_DATA1"),
	PINCTRL_PIN(10, "EMMC_DATA2"),
	PINCTRL_PIN(11, "EMMC_DATA3"),
	PINCTRL_PIN(12, "EMMC_DATA4"),
	PINCTRL_PIN(13, "EMMC_DATA5"),
	PINCTRL_PIN(14, "EMMC_DATA6"),
	PINCTRL_PIN(15, "EMMC_DATA7"),
	PINCTRL_PIN(16, "EMMC_RCLK"),
	PINCTRL_PIN(17, "EMMC_CLK"),
	PINCTRL_PIN(18, "EMMC_RESETB"),
	PINCTRL_PIN(19, "A4WP_PRESENT"),
	/* SPI */
	PINCTRL_PIN(20, "SPI0_IO_2"),
	PINCTRL_PIN(21, "SPI0_IO_3"),
	PINCTRL_PIN(22, "SPI0_MOSI_IO_0"),
	PINCTRL_PIN(23, "SPI0_MISO_IO_1"),
	PINCTRL_PIN(24, "SPI0_TPM_CSB"),
	PINCTRL_PIN(25, "SPI0_FLASH_0_CSB"),
	PINCTRL_PIN(26, "SPI0_FLASH_1_CSB"),
	PINCTRL_PIN(27, "SPI0_CLK"),
	PINCTRL_PIN(28, "SPI0_CLK_LOOPBK"),
	/* GPP_B */
	PINCTRL_PIN(29, "CORE_VID_0"),
	PINCTRL_PIN(30, "CORE_VID_1"),
	PINCTRL_PIN(31, "VRALERTB"),
	PINCTRL_PIN(32, "CPU_GP_2"),
	PINCTRL_PIN(33, "CPU_GP_3"),
	PINCTRL_PIN(34, "SRCCLKREQB_0"),
	PINCTRL_PIN(35, "SRCCLKREQB_1"),
	PINCTRL_PIN(36, "SRCCLKREQB_2"),
	PINCTRL_PIN(37, "SRCCLKREQB_3"),
	PINCTRL_PIN(38, "SRCCLKREQB_4"),
	PINCTRL_PIN(39, "SRCCLKREQB_5"),
	PINCTRL_PIN(40, "PMCALERTB"),
	PINCTRL_PIN(41, "SLP_S0B"),
	PINCTRL_PIN(42, "PLTRSTB"),
	PINCTRL_PIN(43, "SPKR"),
	PINCTRL_PIN(44, "GSPI0_CS0B"),
	PINCTRL_PIN(45, "GSPI0_CLK"),
	PINCTRL_PIN(46, "GSPI0_MISO"),
	PINCTRL_PIN(47, "GSPI0_MOSI"),
	PINCTRL_PIN(48, "GSPI1_CS0B"),
	PINCTRL_PIN(49, "GSPI1_CLK"),
	PINCTRL_PIN(50, "GSPI1_MISO"),
	PINCTRL_PIN(51, "GSPI1_MOSI"),
	PINCTRL_PIN(52, "DDSP_HPD_A"),
	PINCTRL_PIN(53, "GSPI0_CLK_LOOPBK"),
	PINCTRL_PIN(54, "GSPI1_CLK_LOOPBK"),
	/* GPP_A */
	PINCTRL_PIN(55, "ESPI_IO_0"),
	PINCTRL_PIN(56, "ESPI_IO_1"),
	PINCTRL_PIN(57, "ESPI_IO_2"),
	PINCTRL_PIN(58, "ESPI_IO_3"),
	PINCTRL_PIN(59, "ESPI_CSB"),
	PINCTRL_PIN(60, "ESPI_CLK"),
	PINCTRL_PIN(61, "ESPI_RESETB"),
	PINCTRL_PIN(62, "SMBCLK"),
	PINCTRL_PIN(63, "SMBDATA"),
	PINCTRL_PIN(64, "SMBALERTB"),
	PINCTRL_PIN(65, "CPU_GP_0"),
	PINCTRL_PIN(66, "CPU_GP_1"),
	PINCTRL_PIN(67, "USB2_OCB_1"),
	PINCTRL_PIN(68, "USB2_OCB_2"),
	PINCTRL_PIN(69, "USB2_OCB_3"),
	PINCTRL_PIN(70, "DDSP_HPD_A_TIME_SYNC_0"),
	PINCTRL_PIN(71, "DDSP_HPD_B"),
	PINCTRL_PIN(72, "DDSP_HPD_C"),
	PINCTRL_PIN(73, "USB2_OCB_0"),
	PINCTRL_PIN(74, "PCHHOTB"),
	PINCTRL_PIN(75, "ESPI_CLK_LOOPBK"),
	/* GPP_S */
	PINCTRL_PIN(76, "SNDW1_CLK"),
	PINCTRL_PIN(77, "SNDW1_DATA"),
	PINCTRL_PIN(78, "SNDW2_CLK"),
	PINCTRL_PIN(79, "SNDW2_DATA"),
	PINCTRL_PIN(80, "SNDW1_CLK"),
	PINCTRL_PIN(81, "SNDW1_DATA"),
	PINCTRL_PIN(82, "SNDW4_CLK_DMIC_CLK_0"),
	PINCTRL_PIN(83, "SNDW4_DATA_DMIC_DATA_0"),
	/* GPP_R */
	PINCTRL_PIN(84, "HDA_BCLK"),
	PINCTRL_PIN(85, "HDA_SYNC"),
	PINCTRL_PIN(86, "HDA_SDO"),
	PINCTRL_PIN(87, "HDA_SDI_0"),
	PINCTRL_PIN(88, "HDA_RSTB"),
	PINCTRL_PIN(89, "HDA_SDI_1"),
	PINCTRL_PIN(90, "I2S1_SFRM"),
	PINCTRL_PIN(91, "I2S1_TXD"),
	/* GPP_H */
	PINCTRL_PIN(92, "GPPC_H_0"),
	PINCTRL_PIN(93, "SD_PWR_EN_B"),
	PINCTRL_PIN(94, "MODEM_CLKREQ"),
	PINCTRL_PIN(95, "SX_EXIT_HOLDOFFB"),
	PINCTRL_PIN(96, "I2C2_SDA"),
	PINCTRL_PIN(97, "I2C2_SCL"),
	PINCTRL_PIN(98, "I2C3_SDA"),
	PINCTRL_PIN(99, "I2C3_SCL"),
	PINCTRL_PIN(100, "I2C4_SDA"),
	PINCTRL_PIN(101, "I2C4_SCL"),
	PINCTRL_PIN(102, "CPU_VCCIO_PWR_GATEB"),
	PINCTRL_PIN(103, "I2S2_SCLK"),
	PINCTRL_PIN(104, "I2S2_SFRM"),
	PINCTRL_PIN(105, "I2S2_TXD"),
	PINCTRL_PIN(106, "I2S2_RXD"),
	PINCTRL_PIN(107, "I2S1_SCLK"),
	PINCTRL_PIN(108, "GPPC_H_16"),
	PINCTRL_PIN(109, "GPPC_H_17"),
	PINCTRL_PIN(110, "GPPC_H_18"),
	PINCTRL_PIN(111, "GPPC_H_19"),
	PINCTRL_PIN(112, "GPPC_H_20"),
	PINCTRL_PIN(113, "GPPC_H_21"),
	PINCTRL_PIN(114, "GPPC_H_22"),
	PINCTRL_PIN(115, "GPPC_H_23"),
	/* GPP_D */
	PINCTRL_PIN(116, "SPI1_CSB"),
	PINCTRL_PIN(117, "SPI1_CLK"),
	PINCTRL_PIN(118, "SPI1_MISO_IO_1"),
	PINCTRL_PIN(119, "SPI1_MOSI_IO_0"),
	PINCTRL_PIN(120, "ISH_I2C0_SDA"),
	PINCTRL_PIN(121, "ISH_I2C0_SCL"),
	PINCTRL_PIN(122, "ISH_I2C1_SDA"),
	PINCTRL_PIN(123, "ISH_I2C1_SCL"),
	PINCTRL_PIN(124, "ISH_SPI_CSB"),
	PINCTRL_PIN(125, "ISH_SPI_CLK"),
	PINCTRL_PIN(126, "ISH_SPI_MISO"),
	PINCTRL_PIN(127, "ISH_SPI_MOSI"),
	PINCTRL_PIN(128, "ISH_UART0_RXD"),
	PINCTRL_PIN(129, "ISH_UART0_TXD"),
	PINCTRL_PIN(130, "ISH_UART0_RTSB"),
	PINCTRL_PIN(131, "ISH_UART0_CTSB"),
	PINCTRL_PIN(132, "SPI1_IO_2"),
	PINCTRL_PIN(133, "SPI1_IO_3"),
	PINCTRL_PIN(134, "I2S_MCLK"),
	PINCTRL_PIN(135, "CNV_MFUART2_RXD"),
	PINCTRL_PIN(136, "CNV_MFUART2_TXD"),
	PINCTRL_PIN(137, "CNV_PA_BLANKING"),
	PINCTRL_PIN(138, "I2C5_SDA"),
	PINCTRL_PIN(139, "I2C5_SCL"),
	PINCTRL_PIN(140, "GSPI2_CLK_LOOPBK"),
	PINCTRL_PIN(141, "SPI1_CLK_LOOPBK"),
	/* vGPIO */
	PINCTRL_PIN(142, "CNV_BTEN"),
	PINCTRL_PIN(143, "CNV_WCEN"),
	PINCTRL_PIN(144, "CNV_BT_HOST_WAKEB"),
	PINCTRL_PIN(145, "CNV_BT_IF_SELECT"),
	PINCTRL_PIN(146, "vCNV_BT_UART_TXD"),
	PINCTRL_PIN(147, "vCNV_BT_UART_RXD"),
	PINCTRL_PIN(148, "vCNV_BT_UART_CTS_B"),
	PINCTRL_PIN(149, "vCNV_BT_UART_RTS_B"),
	PINCTRL_PIN(150, "vCNV_MFUART1_TXD"),
	PINCTRL_PIN(151, "vCNV_MFUART1_RXD"),
	PINCTRL_PIN(152, "vCNV_MFUART1_CTS_B"),
	PINCTRL_PIN(153, "vCNV_MFUART1_RTS_B"),
	PINCTRL_PIN(154, "vUART0_TXD"),
	PINCTRL_PIN(155, "vUART0_RXD"),
	PINCTRL_PIN(156, "vUART0_CTS_B"),
	PINCTRL_PIN(157, "vUART0_RTS_B"),
	PINCTRL_PIN(158, "vISH_UART0_TXD"),
	PINCTRL_PIN(159, "vISH_UART0_RXD"),
	PINCTRL_PIN(160, "vISH_UART0_CTS_B"),
	PINCTRL_PIN(161, "vISH_UART0_RTS_B"),
	PINCTRL_PIN(162, "vCNV_BT_I2S_BCLK"),
	PINCTRL_PIN(163, "vCNV_BT_I2S_WS_SYNC"),
	PINCTRL_PIN(164, "vCNV_BT_I2S_SDO"),
	PINCTRL_PIN(165, "vCNV_BT_I2S_SDI"),
	PINCTRL_PIN(166, "vI2S2_SCLK"),
	PINCTRL_PIN(167, "vI2S2_SFRM"),
	PINCTRL_PIN(168, "vI2S2_TXD"),
	PINCTRL_PIN(169, "vI2S2_RXD"),
	PINCTRL_PIN(170, "vSD3_CD_B"),
	/* GPP_C */
	PINCTRL_PIN(171, "GPPC_C_0"),
	PINCTRL_PIN(172, "GPPC_C_1"),
	PINCTRL_PIN(173, "GPPC_C_2"),
	PINCTRL_PIN(174, "GPPC_C_3"),
	PINCTRL_PIN(175, "GPPC_C_4"),
	PINCTRL_PIN(176, "GPPC_C_5"),
	PINCTRL_PIN(177, "SUSWARNB_SUSPWRDNACK"),
	PINCTRL_PIN(178, "SUSACKB"),
	PINCTRL_PIN(179, "UART0_RXD"),
	PINCTRL_PIN(180, "UART0_TXD"),
	PINCTRL_PIN(181, "UART0_RTSB"),
	PINCTRL_PIN(182, "UART0_CTSB"),
	PINCTRL_PIN(183, "UART1_RXD"),
	PINCTRL_PIN(184, "UART1_TXD"),
	PINCTRL_PIN(185, "UART1_RTSB"),
	PINCTRL_PIN(186, "UART1_CTSB"),
	PINCTRL_PIN(187, "I2C0_SDA"),
	PINCTRL_PIN(188, "I2C0_SCL"),
	PINCTRL_PIN(189, "I2C1_SDA"),
	PINCTRL_PIN(190, "I2C1_SCL"),
	PINCTRL_PIN(191, "UART2_RXD"),
	PINCTRL_PIN(192, "UART2_TXD"),
	PINCTRL_PIN(193, "UART2_RTSB"),
	PINCTRL_PIN(194, "UART2_CTSB"),
	/* HVCMOS */
	PINCTRL_PIN(195, "L_BKLTEN"),
	PINCTRL_PIN(196, "L_BKLTCTL"),
	PINCTRL_PIN(197, "L_VDDEN"),
	PINCTRL_PIN(198, "SYS_PWROK"),
	PINCTRL_PIN(199, "SYS_RESETB"),
	PINCTRL_PIN(200, "MLK_RSTB"),
	/* GPP_E */
	PINCTRL_PIN(201, "ISH_GP_0"),
	PINCTRL_PIN(202, "ISH_GP_1"),
	PINCTRL_PIN(203, "IMGCLKOUT_1"),
	PINCTRL_PIN(204, "ISH_GP_2"),
	PINCTRL_PIN(205, "IMGCLKOUT_2"),
	PINCTRL_PIN(206, "SATA_LEDB"),
	PINCTRL_PIN(207, "IMGCLKOUT_3"),
	PINCTRL_PIN(208, "ISH_GP_3"),
	PINCTRL_PIN(209, "ISH_GP_4"),
	PINCTRL_PIN(210, "ISH_GP_5"),
	PINCTRL_PIN(211, "ISH_GP_6"),
	PINCTRL_PIN(212, "ISH_GP_7"),
	PINCTRL_PIN(213, "IMGCLKOUT_4"),
	PINCTRL_PIN(214, "DDPA_CTRLCLK"),
	PINCTRL_PIN(215, "DDPA_CTRLDATA"),
	PINCTRL_PIN(216, "DDPB_CTRLCLK"),
	PINCTRL_PIN(217, "DDPB_CTRLDATA"),
	PINCTRL_PIN(218, "DDPC_CTRLCLK"),
	PINCTRL_PIN(219, "DDPC_CTRLDATA"),
	PINCTRL_PIN(220, "IMGCLKOUT_5"),
	PINCTRL_PIN(221, "CNV_BRI_DT"),
	PINCTRL_PIN(222, "CNV_BRI_RSP"),
	PINCTRL_PIN(223, "CNV_RGI_DT"),
	PINCTRL_PIN(224, "CNV_RGI_RSP"),
	/* GPP_G */
	PINCTRL_PIN(225, "SD3_CMD"),
	PINCTRL_PIN(226, "SD3_D0"),
	PINCTRL_PIN(227, "SD3_D1"),
	PINCTRL_PIN(228, "SD3_D2"),
	PINCTRL_PIN(229, "SD3_D3"),
	PINCTRL_PIN(230, "SD3_CDB"),
	PINCTRL_PIN(231, "SD3_CLK"),
	PINCTRL_PIN(232, "SD3_WP"),
};

static const struct intel_padgroup jsl_community0_gpps[] = {
	JSL_GPP(0, 0, 19, 320),				/* GPP_F */
	JSL_GPP(1, 20, 28, INTEL_GPIO_BASE_NOMAP),	/* SPI */
	JSL_GPP(2, 29, 54, 32),				/* GPP_B */
	JSL_GPP(3, 55, 75, 64),				/* GPP_A */
	JSL_GPP(4, 76, 83, 96),				/* GPP_S */
	JSL_GPP(5, 84, 91, 128),			/* GPP_R */
};

static const struct intel_padgroup jsl_community1_gpps[] = {
	JSL_GPP(0, 92, 115, 160),			/* GPP_H */
	JSL_GPP(1, 116, 141, 192),			/* GPP_D */
	JSL_GPP(2, 142, 170, 224),			/* vGPIO */
	JSL_GPP(3, 171, 194, 256),			/* GPP_C */
};

static const struct intel_padgroup jsl_community4_gpps[] = {
	JSL_GPP(0, 195, 200, INTEL_GPIO_BASE_NOMAP),	/* HVCMOS */
	JSL_GPP(1, 201, 224, 288),			/* GPP_E */
};

static const struct intel_padgroup jsl_community5_gpps[] = {
	JSL_GPP(0, 225, 232, INTEL_GPIO_BASE_ZERO),	/* GPP_G */
};

static const struct intel_community jsl_communities[] = {
	JSL_COMMUNITY(0, 0, 91, jsl_community0_gpps),
	JSL_COMMUNITY(1, 92, 194, jsl_community1_gpps),
	JSL_COMMUNITY(2, 195, 224, jsl_community4_gpps),
	JSL_COMMUNITY(3, 225, 232, jsl_community5_gpps),
};

static const struct intel_pinctrl_soc_data jsl_soc_data = {
	.pins = jsl_pins,
	.npins = ARRAY_SIZE(jsl_pins),
	.communities = jsl_communities,
	.ncommunities = ARRAY_SIZE(jsl_communities),
};

static const struct acpi_device_id jsl_pinctrl_acpi_match[] = {
	{ "INT34C8", (kernel_ulong_t)&jsl_soc_data },
	{ }
};
MODULE_DEVICE_TABLE(acpi, jsl_pinctrl_acpi_match);

static INTEL_PINCTRL_PM_OPS(jsl_pinctrl_pm_ops);

static struct platform_driver jsl_pinctrl_driver = {
	.probe = intel_pinctrl_probe_by_hid,
	.driver = {
		.name = "jasperlake-pinctrl",
		.acpi_match_table = jsl_pinctrl_acpi_match,
		.pm = &jsl_pinctrl_pm_ops,
	},
};
module_platform_driver(jsl_pinctrl_driver);

MODULE_AUTHOR("Andy Shevchenko <andriy.shevchenko@linux.intel.com>");
MODULE_DESCRIPTION("Intel Jasper Lake PCH pinctrl/GPIO driver");
MODULE_LICENSE("GPL v2");
MODULE_IMPORT_NS(PINCTRL_INTEL);
