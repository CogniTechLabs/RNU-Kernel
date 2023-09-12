// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2021 Rockchip Electronics Co., Ltd.
 * Copyright (c) 2022 Collabora Ltd.
 * Author: Sebastian Reichel <sebastian.reichel@collabora.com>
 */

#include <linux/module.h>
#include <linux/of.h>
#include <dt-bindings/reset/rockchip,rk3588-cru.h>
#include "clk.h"

/* 0xFD7C0000 + 0x0A00 */
#define RK3588_CRU_RESET_OFFSET(id, reg, bit) [id] = (0 + reg * 16 + bit)

/* 0xFD7C8000 + 0x0A00 */
#define RK3588_PHPTOPCRU_RESET_OFFSET(id, reg, bit) [id] = (0x8000*4 + reg * 16 + bit)

/* 0xFD7D0000 + 0x0A00 */
#define RK3588_SECURECRU_RESET_OFFSET(id, reg, bit) [id] = (0x10000*4 + reg * 16 + bit)

/* 0xFD7F0000 + 0x0A00 */
#define RK3588_PMU1CRU_RESET_OFFSET(id, reg, bit) [id] = (0x30000*4 + reg * 16 + bit)

/* mapping table for reset ID to register offset */
static const int rk3588_register_offset[] = {
	/* SOFTRST_CON01 */
	RK3588_CRU_RESET_OFFSET(SRST_A_TOP_BIU, 1, 3),
	RK3588_CRU_RESET_OFFSET(SRST_P_TOP_BIU, 1, 4),
	RK3588_CRU_RESET_OFFSET(SRST_P_CSIPHY0, 1, 6),
	RK3588_CRU_RESET_OFFSET(SRST_CSIPHY0, 1, 7), // missing in TRM
	RK3588_CRU_RESET_OFFSET(SRST_P_CSIPHY1, 1, 8),
	RK3588_CRU_RESET_OFFSET(SRST_CSIPHY1, 1, 9), // missing in TRM
	RK3588_CRU_RESET_OFFSET(SRST_A_TOP_M500_BIU, 1, 15),

	/* SOFTRST_CON02 */
	RK3588_CRU_RESET_OFFSET(SRST_A_TOP_M400_BIU, 2, 0),
	RK3588_CRU_RESET_OFFSET(SRST_A_TOP_S200_BIU, 2, 1),
	RK3588_CRU_RESET_OFFSET(SRST_A_TOP_S400_BIU, 2, 2),
	RK3588_CRU_RESET_OFFSET(SRST_A_TOP_M300_BIU, 2, 3),
	RK3588_CRU_RESET_OFFSET(SRST_USBDP_COMBO_PHY0_INIT, 2, 8),
	RK3588_CRU_RESET_OFFSET(SRST_USBDP_COMBO_PHY0_CMN, 2, 9),
	RK3588_CRU_RESET_OFFSET(SRST_USBDP_COMBO_PHY0_LANE, 2, 10),
	RK3588_CRU_RESET_OFFSET(SRST_USBDP_COMBO_PHY0_PCS, 2, 11),
	RK3588_CRU_RESET_OFFSET(SRST_USBDP_COMBO_PHY1_INIT, 2, 15),

	/* SOFTRST_CON03 */
	RK3588_CRU_RESET_OFFSET(SRST_USBDP_COMBO_PHY1_CMN, 3, 0),
	RK3588_CRU_RESET_OFFSET(SRST_USBDP_COMBO_PHY1_LANE, 3, 1),
	RK3588_CRU_RESET_OFFSET(SRST_USBDP_COMBO_PHY1_PCS, 3, 2),
	RK3588_CRU_RESET_OFFSET(SRST_DCPHY0, 3, 11), // missing in TRM
	RK3588_CRU_RESET_OFFSET(SRST_P_MIPI_DCPHY0, 3, 14),
	RK3588_CRU_RESET_OFFSET(SRST_P_MIPI_DCPHY0_GRF, 3, 15),

	/* SOFTRST_CON04 */
	RK3588_CRU_RESET_OFFSET(SRST_DCPHY1, 4, 0), // missing in TRM
	RK3588_CRU_RESET_OFFSET(SRST_P_MIPI_DCPHY1, 4, 3),
	RK3588_CRU_RESET_OFFSET(SRST_P_MIPI_DCPHY1_GRF, 4, 4),
	RK3588_CRU_RESET_OFFSET(SRST_P_APB2ASB_SLV_CDPHY, 4, 5),
	RK3588_CRU_RESET_OFFSET(SRST_P_APB2ASB_SLV_CSIPHY, 4, 6),
	RK3588_CRU_RESET_OFFSET(SRST_P_APB2ASB_SLV_VCCIO3_5, 4, 7),
	RK3588_CRU_RESET_OFFSET(SRST_P_APB2ASB_SLV_VCCIO6, 4, 8),
	RK3588_CRU_RESET_OFFSET(SRST_P_APB2ASB_SLV_EMMCIO, 4, 9),
	RK3588_CRU_RESET_OFFSET(SRST_P_APB2ASB_SLV_IOC_TOP, 4, 10),
	RK3588_CRU_RESET_OFFSET(SRST_P_APB2ASB_SLV_IOC_RIGHT, 4, 11),

	/* SOFTRST_CON05 */
	RK3588_CRU_RESET_OFFSET(SRST_P_CRU, 5, 0),
	RK3588_CRU_RESET_OFFSET(SRST_A_CHANNEL_SECURE2VO1USB, 5, 7),
	RK3588_CRU_RESET_OFFSET(SRST_A_CHANNEL_SECURE2CENTER, 5, 8),
	RK3588_CRU_RESET_OFFSET(SRST_H_CHANNEL_SECURE2VO1USB, 5, 14),
	RK3588_CRU_RESET_OFFSET(SRST_H_CHANNEL_SECURE2CENTER, 5, 15),

	/* SOFTRST_CON06 */
	RK3588_CRU_RESET_OFFSET(SRST_P_CHANNEL_SECURE2VO1USB, 6, 0),
	RK3588_CRU_RESET_OFFSET(SRST_P_CHANNEL_SECURE2CENTER, 6, 1),

	/* SOFTRST_CON07 */
	RK3588_CRU_RESET_OFFSET(SRST_H_AUDIO_BIU, 7, 2),
	RK3588_CRU_RESET_OFFSET(SRST_P_AUDIO_BIU, 7, 3),
	RK3588_CRU_RESET_OFFSET(SRST_H_I2S0_8CH, 7, 4),
	RK3588_CRU_RESET_OFFSET(SRST_M_I2S0_8CH_TX, 7, 7),
	RK3588_CRU_RESET_OFFSET(SRST_M_I2S0_8CH_RX, 7, 10),
	RK3588_CRU_RESET_OFFSET(SRST_P_ACDCDIG, 7, 11),
	RK3588_CRU_RESET_OFFSET(SRST_H_I2S2_2CH, 7, 12),
	RK3588_CRU_RESET_OFFSET(SRST_H_I2S3_2CH, 7, 13),

	/* SOFTRST_CON08 */
	RK3588_CRU_RESET_OFFSET(SRST_M_I2S2_2CH, 8, 0),
	RK3588_CRU_RESET_OFFSET(SRST_M_I2S3_2CH, 8, 3),
	RK3588_CRU_RESET_OFFSET(SRST_DAC_ACDCDIG, 8, 4),
	RK3588_CRU_RESET_OFFSET(SRST_H_SPDIF0, 8, 14),

	/* SOFTRST_CON09 */
	RK3588_CRU_RESET_OFFSET(SRST_M_SPDIF0, 9, 1),
	RK3588_CRU_RESET_OFFSET(SRST_H_SPDIF1, 9, 2),
	RK3588_CRU_RESET_OFFSET(SRST_M_SPDIF1, 9, 5),
	RK3588_CRU_RESET_OFFSET(SRST_H_PDM1, 9, 6),
	RK3588_CRU_RESET_OFFSET(SRST_PDM1, 9, 7),

	/* SOFTRST_CON10 */
	RK3588_CRU_RESET_OFFSET(SRST_A_BUS_BIU, 10, 1),
	RK3588_CRU_RESET_OFFSET(SRST_P_BUS_BIU, 10, 2),
	RK3588_CRU_RESET_OFFSET(SRST_A_GIC, 10, 3),
	RK3588_CRU_RESET_OFFSET(SRST_A_GIC_DBG, 10, 4),
	RK3588_CRU_RESET_OFFSET(SRST_A_DMAC0, 10, 5),
	RK3588_CRU_RESET_OFFSET(SRST_A_DMAC1, 10, 6),
	RK3588_CRU_RESET_OFFSET(SRST_A_DMAC2, 10, 7),
	RK3588_CRU_RESET_OFFSET(SRST_P_I2C1, 10, 8),
	RK3588_CRU_RESET_OFFSET(SRST_P_I2C2, 10, 9),
	RK3588_CRU_RESET_OFFSET(SRST_P_I2C3, 10, 10),
	RK3588_CRU_RESET_OFFSET(SRST_P_I2C4, 10, 11),
	RK3588_CRU_RESET_OFFSET(SRST_P_I2C5, 10, 12),
	RK3588_CRU_RESET_OFFSET(SRST_P_I2C6, 10, 13),
	RK3588_CRU_RESET_OFFSET(SRST_P_I2C7, 10, 14),
	RK3588_CRU_RESET_OFFSET(SRST_P_I2C8, 10, 15),

	/* SOFTRST_CON11 */
	RK3588_CRU_RESET_OFFSET(SRST_I2C1, 11, 0),
	RK3588_CRU_RESET_OFFSET(SRST_I2C2, 11, 1),
	RK3588_CRU_RESET_OFFSET(SRST_I2C3, 11, 2),
	RK3588_CRU_RESET_OFFSET(SRST_I2C4, 11, 3),
	RK3588_CRU_RESET_OFFSET(SRST_I2C5, 11, 4),
	RK3588_CRU_RESET_OFFSET(SRST_I2C6, 11, 5),
	RK3588_CRU_RESET_OFFSET(SRST_I2C7, 11, 6),
	RK3588_CRU_RESET_OFFSET(SRST_I2C8, 11, 7),
	RK3588_CRU_RESET_OFFSET(SRST_P_CAN0, 11, 8),
	RK3588_CRU_RESET_OFFSET(SRST_CAN0, 11, 9),
	RK3588_CRU_RESET_OFFSET(SRST_P_CAN1, 11, 10),
	RK3588_CRU_RESET_OFFSET(SRST_CAN1, 11, 11),
	RK3588_CRU_RESET_OFFSET(SRST_P_CAN2, 11, 12),
	RK3588_CRU_RESET_OFFSET(SRST_CAN2, 11, 13),
	RK3588_CRU_RESET_OFFSET(SRST_P_SARADC, 11, 14),

	/* SOFTRST_CON12 */
	RK3588_CRU_RESET_OFFSET(SRST_P_TSADC, 12, 0),
	RK3588_CRU_RESET_OFFSET(SRST_TSADC, 12, 1),
	RK3588_CRU_RESET_OFFSET(SRST_P_UART1, 12, 2),
	RK3588_CRU_RESET_OFFSET(SRST_P_UART2, 12, 3),
	RK3588_CRU_RESET_OFFSET(SRST_P_UART3, 12, 4),
	RK3588_CRU_RESET_OFFSET(SRST_P_UART4, 12, 5),
	RK3588_CRU_RESET_OFFSET(SRST_P_UART5, 12, 6),
	RK3588_CRU_RESET_OFFSET(SRST_P_UART6, 12, 7),
	RK3588_CRU_RESET_OFFSET(SRST_P_UART7, 12, 8),
	RK3588_CRU_RESET_OFFSET(SRST_P_UART8, 12, 9),
	RK3588_CRU_RESET_OFFSET(SRST_P_UART9, 12, 10),
	RK3588_CRU_RESET_OFFSET(SRST_S_UART1, 12, 13),

	/* SOFTRST_CON13 */
	RK3588_CRU_RESET_OFFSET(SRST_S_UART2, 13, 0),
	RK3588_CRU_RESET_OFFSET(SRST_S_UART3, 13, 3),
	RK3588_CRU_RESET_OFFSET(SRST_S_UART4, 13, 6),
	RK3588_CRU_RESET_OFFSET(SRST_S_UART5, 13, 9),
	RK3588_CRU_RESET_OFFSET(SRST_S_UART6, 13, 12),
	RK3588_CRU_RESET_OFFSET(SRST_S_UART7, 13, 15),

	/* SOFTRST_CON14 */
	RK3588_CRU_RESET_OFFSET(SRST_S_UART8, 14, 2),
	RK3588_CRU_RESET_OFFSET(SRST_S_UART9, 14, 5),
	RK3588_CRU_RESET_OFFSET(SRST_P_SPI0, 14, 6),
	RK3588_CRU_RESET_OFFSET(SRST_P_SPI1, 14, 7),
	RK3588_CRU_RESET_OFFSET(SRST_P_SPI2, 14, 8),
	RK3588_CRU_RESET_OFFSET(SRST_P_SPI3, 14, 9),
	RK3588_CRU_RESET_OFFSET(SRST_P_SPI4, 14, 10),
	RK3588_CRU_RESET_OFFSET(SRST_SPI0, 14, 11),
	RK3588_CRU_RESET_OFFSET(SRST_SPI1, 14, 12),
	RK3588_CRU_RESET_OFFSET(SRST_SPI2, 14, 13),
	RK3588_CRU_RESET_OFFSET(SRST_SPI3, 14, 14),
	RK3588_CRU_RESET_OFFSET(SRST_SPI4, 14, 15),

	/* SOFTRST_CON15 */
	RK3588_CRU_RESET_OFFSET(SRST_P_WDT0, 15, 0),
	RK3588_CRU_RESET_OFFSET(SRST_T_WDT0, 15, 1),
	RK3588_CRU_RESET_OFFSET(SRST_P_SYS_GRF, 15, 2),
	RK3588_CRU_RESET_OFFSET(SRST_P_PWM1, 15, 3),
	RK3588_CRU_RESET_OFFSET(SRST_PWM1, 15, 4),
	RK3588_CRU_RESET_OFFSET(SRST_P_PWM2, 15, 6),
	RK3588_CRU_RESET_OFFSET(SRST_PWM2, 15, 7),
	RK3588_CRU_RESET_OFFSET(SRST_P_PWM3, 15, 9),
	RK3588_CRU_RESET_OFFSET(SRST_PWM3, 15, 10),
	RK3588_CRU_RESET_OFFSET(SRST_P_BUSTIMER0, 15, 12),
	RK3588_CRU_RESET_OFFSET(SRST_P_BUSTIMER1, 15, 13),
	RK3588_CRU_RESET_OFFSET(SRST_BUSTIMER0, 15, 15),

	/* SOFTRST_CON16 */
	RK3588_CRU_RESET_OFFSET(SRST_BUSTIMER1, 16, 0),
	RK3588_CRU_RESET_OFFSET(SRST_BUSTIMER2, 16, 1),
	RK3588_CRU_RESET_OFFSET(SRST_BUSTIMER3, 16, 2),
	RK3588_CRU_RESET_OFFSET(SRST_BUSTIMER4, 16, 3),
	RK3588_CRU_RESET_OFFSET(SRST_BUSTIMER5, 16, 4),
	RK3588_CRU_RESET_OFFSET(SRST_BUSTIMER6, 16, 5),
	RK3588_CRU_RESET_OFFSET(SRST_BUSTIMER7, 16, 6),
	RK3588_CRU_RESET_OFFSET(SRST_BUSTIMER8, 16, 7),
	RK3588_CRU_RESET_OFFSET(SRST_BUSTIMER9, 16, 8),
	RK3588_CRU_RESET_OFFSET(SRST_BUSTIMER10, 16, 9),
	RK3588_CRU_RESET_OFFSET(SRST_BUSTIMER11, 16, 10),
	RK3588_CRU_RESET_OFFSET(SRST_P_MAILBOX0, 16, 11),
	RK3588_CRU_RESET_OFFSET(SRST_P_MAILBOX1, 16, 12),
	RK3588_CRU_RESET_OFFSET(SRST_P_MAILBOX2, 16, 13),
	RK3588_CRU_RESET_OFFSET(SRST_P_GPIO1, 16, 14),
	RK3588_CRU_RESET_OFFSET(SRST_GPIO1, 16, 15),

	/* SOFTRST_CON17 */
	RK3588_CRU_RESET_OFFSET(SRST_P_GPIO2, 17, 0),
	RK3588_CRU_RESET_OFFSET(SRST_GPIO2, 17, 1),
	RK3588_CRU_RESET_OFFSET(SRST_P_GPIO3, 17, 2),
	RK3588_CRU_RESET_OFFSET(SRST_GPIO3, 17, 3),
	RK3588_CRU_RESET_OFFSET(SRST_P_GPIO4, 17, 4),
	RK3588_CRU_RESET_OFFSET(SRST_GPIO4, 17, 5),
	RK3588_CRU_RESET_OFFSET(SRST_A_DECOM, 17, 6),
	RK3588_CRU_RESET_OFFSET(SRST_P_DECOM, 17, 7),
	RK3588_CRU_RESET_OFFSET(SRST_D_DECOM, 17, 8),
	RK3588_CRU_RESET_OFFSET(SRST_P_TOP, 17, 9),
	RK3588_CRU_RESET_OFFSET(SRST_A_GICADB_GIC2CORE_BUS, 17, 11),
	RK3588_CRU_RESET_OFFSET(SRST_P_DFT2APB, 17, 12),
	RK3588_CRU_RESET_OFFSET(SRST_P_APB2ASB_MST_TOP, 17, 13),
	RK3588_CRU_RESET_OFFSET(SRST_P_APB2ASB_MST_CDPHY, 17, 14),
	RK3588_CRU_RESET_OFFSET(SRST_P_APB2ASB_MST_BOT_RIGHT, 17, 15),

	/* SOFTRST_CON18 */
	RK3588_CRU_RESET_OFFSET(SRST_P_APB2ASB_MST_IOC_TOP, 18, 0),
	RK3588_CRU_RESET_OFFSET(SRST_P_APB2ASB_MST_IOC_RIGHT, 18, 1),
	RK3588_CRU_RESET_OFFSET(SRST_P_APB2ASB_MST_CSIPHY, 18, 2),
	RK3588_CRU_RESET_OFFSET(SRST_P_APB2ASB_MST_VCCIO3_5, 18, 3),
	RK3588_CRU_RESET_OFFSET(SRST_P_APB2ASB_MST_VCCIO6, 18, 4),
	RK3588_CRU_RESET_OFFSET(SRST_P_APB2ASB_MST_EMMCIO, 18, 5),
	RK3588_CRU_RESET_OFFSET(SRST_A_SPINLOCK, 18, 6),
	RK3588_CRU_RESET_OFFSET(SRST_P_OTPC_NS, 18, 9),
	RK3588_CRU_RESET_OFFSET(SRST_OTPC_NS, 18, 10),
	RK3588_CRU_RESET_OFFSET(SRST_OTPC_ARB, 18, 11),

	/* SOFTRST_CON19 */
	RK3588_CRU_RESET_OFFSET(SRST_P_BUSIOC, 19, 0),
	RK3588_CRU_RESET_OFFSET(SRST_P_PMUCM0_INTMUX, 19, 4),
	RK3588_CRU_RESET_OFFSET(SRST_P_DDRCM0_INTMUX, 19, 5),

	/* SOFTRST_CON20 */
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR_DFICTL_CH0, 20, 0),
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR_MON_CH0, 20, 1),
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR_STANDBY_CH0, 20, 2),
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR_UPCTL_CH0, 20, 3),
	RK3588_CRU_RESET_OFFSET(SRST_TM_DDR_MON_CH0, 20, 4),
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR_GRF_CH01, 20, 5),
	RK3588_CRU_RESET_OFFSET(SRST_DFI_CH0, 20, 6),
	RK3588_CRU_RESET_OFFSET(SRST_SBR_CH0, 20, 7),
	RK3588_CRU_RESET_OFFSET(SRST_DDR_UPCTL_CH0, 20, 8),
	RK3588_CRU_RESET_OFFSET(SRST_DDR_DFICTL_CH0, 20, 9),
	RK3588_CRU_RESET_OFFSET(SRST_DDR_MON_CH0, 20, 10),
	RK3588_CRU_RESET_OFFSET(SRST_DDR_STANDBY_CH0, 20, 11),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR_UPCTL_CH0, 20, 12),
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR_DFICTL_CH1, 20, 13),
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR_MON_CH1, 20, 14),
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR_STANDBY_CH1, 20, 15),

	/* SOFTRST_CON21 */
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR_UPCTL_CH1, 21, 0),
	RK3588_CRU_RESET_OFFSET(SRST_TM_DDR_MON_CH1, 21, 1),
	RK3588_CRU_RESET_OFFSET(SRST_DFI_CH1, 21, 2),
	RK3588_CRU_RESET_OFFSET(SRST_SBR_CH1, 21, 3),
	RK3588_CRU_RESET_OFFSET(SRST_DDR_UPCTL_CH1, 21, 4),
	RK3588_CRU_RESET_OFFSET(SRST_DDR_DFICTL_CH1, 21, 5),
	RK3588_CRU_RESET_OFFSET(SRST_DDR_MON_CH1, 21, 6),
	RK3588_CRU_RESET_OFFSET(SRST_DDR_STANDBY_CH1, 21, 7),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR_UPCTL_CH1, 21, 8),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR01_MSCH0, 21, 13),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR01_RS_MSCH0, 21, 14),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR01_FRS_MSCH0, 21, 15),

	/* SOFTRST_CON22 */
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR01_SCRAMBLE0, 22, 0),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR01_FRS_SCRAMBLE0, 22, 1),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR01_MSCH1, 22, 2),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR01_RS_MSCH1, 22, 3),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR01_FRS_MSCH1, 22, 4),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR01_SCRAMBLE1, 22, 5),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR01_FRS_SCRAMBLE1, 22, 6),
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR01_MSCH0, 22, 7),
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR01_MSCH1, 22, 8),

	/* SOFTRST_CON23 */
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR_DFICTL_CH2, 23, 0),
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR_MON_CH2, 23, 1),
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR_STANDBY_CH2, 23, 2),
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR_UPCTL_CH2, 23, 3),
	RK3588_CRU_RESET_OFFSET(SRST_TM_DDR_MON_CH2, 23, 4),
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR_GRF_CH23, 23, 5),
	RK3588_CRU_RESET_OFFSET(SRST_DFI_CH2, 23, 6),
	RK3588_CRU_RESET_OFFSET(SRST_SBR_CH2, 23, 7),
	RK3588_CRU_RESET_OFFSET(SRST_DDR_UPCTL_CH2, 23, 8),
	RK3588_CRU_RESET_OFFSET(SRST_DDR_DFICTL_CH2, 23, 9),
	RK3588_CRU_RESET_OFFSET(SRST_DDR_MON_CH2, 23, 10),
	RK3588_CRU_RESET_OFFSET(SRST_DDR_STANDBY_CH2, 23, 11),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR_UPCTL_CH2, 23, 12),
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR_DFICTL_CH3, 23, 13),
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR_MON_CH3, 23, 14),
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR_STANDBY_CH3, 23, 15),

	/* SOFTRST_CON24 */
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR_UPCTL_CH3, 24, 0),
	RK3588_CRU_RESET_OFFSET(SRST_TM_DDR_MON_CH3, 24, 1),
	RK3588_CRU_RESET_OFFSET(SRST_DFI_CH3, 24, 2),
	RK3588_CRU_RESET_OFFSET(SRST_SBR_CH3, 24, 3),
	RK3588_CRU_RESET_OFFSET(SRST_DDR_UPCTL_CH3, 24, 4),
	RK3588_CRU_RESET_OFFSET(SRST_DDR_DFICTL_CH3, 24, 5),
	RK3588_CRU_RESET_OFFSET(SRST_DDR_MON_CH3, 24, 6),
	RK3588_CRU_RESET_OFFSET(SRST_DDR_STANDBY_CH3, 24, 7),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR_UPCTL_CH3, 24, 8),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR23_MSCH2, 24, 13),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR23_RS_MSCH2, 24, 14),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR23_FRS_MSCH2, 24, 15),

	/* SOFTRST_CON25 */
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR23_SCRAMBLE2, 25, 0),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR23_FRS_SCRAMBLE2, 25, 1),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR23_MSCH3, 25, 2),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR23_RS_MSCH3, 25, 3),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR23_FRS_MSCH3, 25, 4),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR23_SCRAMBLE3, 25, 5),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR23_FRS_SCRAMBLE3, 25, 6),
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR23_MSCH2, 25, 7),
	RK3588_CRU_RESET_OFFSET(SRST_P_DDR23_MSCH3, 25, 8),

	/* SOFTRST_CON26 */
	RK3588_CRU_RESET_OFFSET(SRST_ISP1, 26, 3),
	RK3588_CRU_RESET_OFFSET(SRST_ISP1_VICAP, 26, 4),
	RK3588_CRU_RESET_OFFSET(SRST_A_ISP1_BIU, 26, 6),
	RK3588_CRU_RESET_OFFSET(SRST_H_ISP1_BIU, 26, 8),

	/* SOFTRST_CON27 */
	RK3588_CRU_RESET_OFFSET(SRST_A_RKNN1, 27, 0),
	RK3588_CRU_RESET_OFFSET(SRST_A_RKNN1_BIU, 27, 1),
	RK3588_CRU_RESET_OFFSET(SRST_H_RKNN1, 27, 2),
	RK3588_CRU_RESET_OFFSET(SRST_H_RKNN1_BIU, 27, 3),

	/* SOFTRST_CON28 */
	RK3588_CRU_RESET_OFFSET(SRST_A_RKNN2, 28, 0),
	RK3588_CRU_RESET_OFFSET(SRST_A_RKNN2_BIU, 28, 1),
	RK3588_CRU_RESET_OFFSET(SRST_H_RKNN2, 28, 2),
	RK3588_CRU_RESET_OFFSET(SRST_H_RKNN2_BIU, 28, 3),

	/* SOFTRST_CON29 */
	RK3588_CRU_RESET_OFFSET(SRST_A_RKNN_DSU0, 29, 3),
	RK3588_CRU_RESET_OFFSET(SRST_P_NPUTOP_BIU, 29, 5),
	RK3588_CRU_RESET_OFFSET(SRST_P_NPU_TIMER, 29, 6),
	RK3588_CRU_RESET_OFFSET(SRST_NPUTIMER0, 29, 8),
	RK3588_CRU_RESET_OFFSET(SRST_NPUTIMER1, 29, 9),
	RK3588_CRU_RESET_OFFSET(SRST_P_NPU_WDT, 29, 10),
	RK3588_CRU_RESET_OFFSET(SRST_T_NPU_WDT, 29, 11),
	RK3588_CRU_RESET_OFFSET(SRST_P_NPU_PVTM, 29, 12),
	RK3588_CRU_RESET_OFFSET(SRST_P_NPU_GRF, 29, 13),
	RK3588_CRU_RESET_OFFSET(SRST_NPU_PVTM, 29, 14),

	/* SOFTRST_CON30 */
	RK3588_CRU_RESET_OFFSET(SRST_NPU_PVTPLL, 30, 0),
	RK3588_CRU_RESET_OFFSET(SRST_H_NPU_CM0_BIU, 30, 2),
	RK3588_CRU_RESET_OFFSET(SRST_F_NPU_CM0_CORE, 30, 3),
	RK3588_CRU_RESET_OFFSET(SRST_T_NPU_CM0_JTAG, 30, 4),
	RK3588_CRU_RESET_OFFSET(SRST_A_RKNN0, 30, 6),
	RK3588_CRU_RESET_OFFSET(SRST_A_RKNN0_BIU, 30, 7),
	RK3588_CRU_RESET_OFFSET(SRST_H_RKNN0, 30, 8),
	RK3588_CRU_RESET_OFFSET(SRST_H_RKNN0_BIU, 30, 9),

	/* SOFTRST_CON31 */
	RK3588_CRU_RESET_OFFSET(SRST_H_NVM_BIU, 31, 2),
	RK3588_CRU_RESET_OFFSET(SRST_A_NVM_BIU, 31, 3),
	RK3588_CRU_RESET_OFFSET(SRST_H_EMMC, 31, 4),
	RK3588_CRU_RESET_OFFSET(SRST_A_EMMC, 31, 5),
	RK3588_CRU_RESET_OFFSET(SRST_C_EMMC, 31, 6),
	RK3588_CRU_RESET_OFFSET(SRST_B_EMMC, 31, 7),
	RK3588_CRU_RESET_OFFSET(SRST_T_EMMC, 31, 8),
	RK3588_CRU_RESET_OFFSET(SRST_S_SFC, 31, 9),
	RK3588_CRU_RESET_OFFSET(SRST_H_SFC, 31, 10),
	RK3588_CRU_RESET_OFFSET(SRST_H_SFC_XIP, 31, 11),

	/* SOFTRST_CON32 */
	RK3588_CRU_RESET_OFFSET(SRST_P_GRF, 32, 1),
	RK3588_CRU_RESET_OFFSET(SRST_P_DEC_BIU, 32, 2),
	RK3588_CRU_RESET_OFFSET(SRST_P_PHP_BIU, 32, 5),
	RK3588_CRU_RESET_OFFSET(SRST_A_PCIE_GRIDGE, 32, 8),
	RK3588_CRU_RESET_OFFSET(SRST_A_PHP_BIU, 32, 9),
	RK3588_CRU_RESET_OFFSET(SRST_A_GMAC0, 32, 10),
	RK3588_CRU_RESET_OFFSET(SRST_A_GMAC1, 32, 11),
	RK3588_CRU_RESET_OFFSET(SRST_A_PCIE_BIU, 32, 12),
	RK3588_CRU_RESET_OFFSET(SRST_PCIE0_POWER_UP, 32, 13),
	RK3588_CRU_RESET_OFFSET(SRST_PCIE1_POWER_UP, 32, 14),
	RK3588_CRU_RESET_OFFSET(SRST_PCIE2_POWER_UP, 32, 15),

	/* SOFTRST_CON33 */
	RK3588_CRU_RESET_OFFSET(SRST_PCIE3_POWER_UP, 33, 0),
	RK3588_CRU_RESET_OFFSET(SRST_PCIE4_POWER_UP, 33, 1),
	RK3588_CRU_RESET_OFFSET(SRST_P_PCIE0, 33, 12),
	RK3588_CRU_RESET_OFFSET(SRST_P_PCIE1, 33, 13),
	RK3588_CRU_RESET_OFFSET(SRST_P_PCIE2, 33, 14),
	RK3588_CRU_RESET_OFFSET(SRST_P_PCIE3, 33, 15),

	/* SOFTRST_CON34 */
	RK3588_CRU_RESET_OFFSET(SRST_P_PCIE4, 34, 0),
	RK3588_CRU_RESET_OFFSET(SRST_A_PHP_GIC_ITS, 34, 6),
	RK3588_CRU_RESET_OFFSET(SRST_A_MMU_PCIE, 34, 7),
	RK3588_CRU_RESET_OFFSET(SRST_A_MMU_PHP, 34, 8),
	RK3588_CRU_RESET_OFFSET(SRST_A_MMU_BIU, 34, 9),

	/* SOFTRST_CON35 */
	RK3588_CRU_RESET_OFFSET(SRST_A_USB3OTG2, 35, 7),

	/* SOFTRST_CON37 */
	RK3588_CRU_RESET_OFFSET(SRST_PMALIVE0, 37, 4),
	RK3588_CRU_RESET_OFFSET(SRST_PMALIVE1, 37, 5),
	RK3588_CRU_RESET_OFFSET(SRST_PMALIVE2, 37, 6),
	RK3588_CRU_RESET_OFFSET(SRST_A_SATA0, 37, 7),
	RK3588_CRU_RESET_OFFSET(SRST_A_SATA1, 37, 8),
	RK3588_CRU_RESET_OFFSET(SRST_A_SATA2, 37, 9),
	RK3588_CRU_RESET_OFFSET(SRST_RXOOB0, 37, 10),
	RK3588_CRU_RESET_OFFSET(SRST_RXOOB1, 37, 11),
	RK3588_CRU_RESET_OFFSET(SRST_RXOOB2, 37, 12),
	RK3588_CRU_RESET_OFFSET(SRST_ASIC0, 37, 13),
	RK3588_CRU_RESET_OFFSET(SRST_ASIC1, 37, 14),
	RK3588_CRU_RESET_OFFSET(SRST_ASIC2, 37, 15),

	/* SOFTRST_CON40 */
	RK3588_CRU_RESET_OFFSET(SRST_A_RKVDEC_CCU, 40, 2),
	RK3588_CRU_RESET_OFFSET(SRST_H_RKVDEC0, 40, 3),
	RK3588_CRU_RESET_OFFSET(SRST_A_RKVDEC0, 40, 4),
	RK3588_CRU_RESET_OFFSET(SRST_H_RKVDEC0_BIU, 40, 5),
	RK3588_CRU_RESET_OFFSET(SRST_A_RKVDEC0_BIU, 40, 6),
	RK3588_CRU_RESET_OFFSET(SRST_RKVDEC0_CA, 40, 7),
	RK3588_CRU_RESET_OFFSET(SRST_RKVDEC0_HEVC_CA, 40, 8),
	RK3588_CRU_RESET_OFFSET(SRST_RKVDEC0_CORE, 40, 9),

	/* SOFTRST_CON41 */
	RK3588_CRU_RESET_OFFSET(SRST_H_RKVDEC1, 41, 2),
	RK3588_CRU_RESET_OFFSET(SRST_A_RKVDEC1, 41, 3),
	RK3588_CRU_RESET_OFFSET(SRST_H_RKVDEC1_BIU, 41, 4),
	RK3588_CRU_RESET_OFFSET(SRST_A_RKVDEC1_BIU, 41, 5),
	RK3588_CRU_RESET_OFFSET(SRST_RKVDEC1_CA, 41, 6),
	RK3588_CRU_RESET_OFFSET(SRST_RKVDEC1_HEVC_CA, 41, 7),
	RK3588_CRU_RESET_OFFSET(SRST_RKVDEC1_CORE, 41, 8),

	/* SOFTRST_CON42 */
	RK3588_CRU_RESET_OFFSET(SRST_A_USB_BIU, 42, 2),
	RK3588_CRU_RESET_OFFSET(SRST_H_USB_BIU, 42, 3),
	RK3588_CRU_RESET_OFFSET(SRST_A_USB3OTG0, 42, 4),
	RK3588_CRU_RESET_OFFSET(SRST_A_USB3OTG1, 42, 7),
	RK3588_CRU_RESET_OFFSET(SRST_H_HOST0, 42, 10),
	RK3588_CRU_RESET_OFFSET(SRST_H_HOST_ARB0, 42, 11),
	RK3588_CRU_RESET_OFFSET(SRST_H_HOST1, 42, 12),
	RK3588_CRU_RESET_OFFSET(SRST_H_HOST_ARB1, 42, 13),
	RK3588_CRU_RESET_OFFSET(SRST_A_USB_GRF, 42, 14),
	RK3588_CRU_RESET_OFFSET(SRST_C_USB2P0_HOST0, 42, 15),

	/* SOFTRST_CON43 */
	RK3588_CRU_RESET_OFFSET(SRST_C_USB2P0_HOST1, 43, 0),
	RK3588_CRU_RESET_OFFSET(SRST_HOST_UTMI0, 43, 1),
	RK3588_CRU_RESET_OFFSET(SRST_HOST_UTMI1, 43, 2),

	/* SOFTRST_CON44 */
	RK3588_CRU_RESET_OFFSET(SRST_A_VDPU_BIU, 44, 4),
	RK3588_CRU_RESET_OFFSET(SRST_A_VDPU_LOW_BIU, 44, 5),
	RK3588_CRU_RESET_OFFSET(SRST_H_VDPU_BIU, 44, 6),
	RK3588_CRU_RESET_OFFSET(SRST_A_JPEG_DECODER_BIU, 44, 7),
	RK3588_CRU_RESET_OFFSET(SRST_A_VPU, 44, 8),
	RK3588_CRU_RESET_OFFSET(SRST_H_VPU, 44, 9),
	RK3588_CRU_RESET_OFFSET(SRST_A_JPEG_ENCODER0, 44, 10),
	RK3588_CRU_RESET_OFFSET(SRST_H_JPEG_ENCODER0, 44, 11),
	RK3588_CRU_RESET_OFFSET(SRST_A_JPEG_ENCODER1, 44, 12),
	RK3588_CRU_RESET_OFFSET(SRST_H_JPEG_ENCODER1, 44, 13),
	RK3588_CRU_RESET_OFFSET(SRST_A_JPEG_ENCODER2, 44, 14),
	RK3588_CRU_RESET_OFFSET(SRST_H_JPEG_ENCODER2, 44, 15),

	/* SOFTRST_CON45 */
	RK3588_CRU_RESET_OFFSET(SRST_A_JPEG_ENCODER3, 45, 0),
	RK3588_CRU_RESET_OFFSET(SRST_H_JPEG_ENCODER3, 45, 1),
	RK3588_CRU_RESET_OFFSET(SRST_A_JPEG_DECODER, 45, 2),
	RK3588_CRU_RESET_OFFSET(SRST_H_JPEG_DECODER, 45, 3),
	RK3588_CRU_RESET_OFFSET(SRST_H_IEP2P0, 45, 4),
	RK3588_CRU_RESET_OFFSET(SRST_A_IEP2P0, 45, 5),
	RK3588_CRU_RESET_OFFSET(SRST_IEP2P0_CORE, 45, 6),
	RK3588_CRU_RESET_OFFSET(SRST_H_RGA2, 45, 7),
	RK3588_CRU_RESET_OFFSET(SRST_A_RGA2, 45, 8),
	RK3588_CRU_RESET_OFFSET(SRST_RGA2_CORE, 45, 9),
	RK3588_CRU_RESET_OFFSET(SRST_H_RGA3_0, 45, 10),
	RK3588_CRU_RESET_OFFSET(SRST_A_RGA3_0, 45, 11),
	RK3588_CRU_RESET_OFFSET(SRST_RGA3_0_CORE, 45, 12),

	/* SOFTRST_CON47 */
	RK3588_CRU_RESET_OFFSET(SRST_H_RKVENC0_BIU, 47, 2),
	RK3588_CRU_RESET_OFFSET(SRST_A_RKVENC0_BIU, 47, 3),
	RK3588_CRU_RESET_OFFSET(SRST_H_RKVENC0, 47, 4),
	RK3588_CRU_RESET_OFFSET(SRST_A_RKVENC0, 47, 5),
	RK3588_CRU_RESET_OFFSET(SRST_RKVENC0_CORE, 47, 6),

	/* SOFTRST_CON48 */
	RK3588_CRU_RESET_OFFSET(SRST_H_RKVENC1_BIU, 48, 2),
	RK3588_CRU_RESET_OFFSET(SRST_A_RKVENC1_BIU, 48, 3),
	RK3588_CRU_RESET_OFFSET(SRST_H_RKVENC1, 48, 4),
	RK3588_CRU_RESET_OFFSET(SRST_A_RKVENC1, 48, 5),
	RK3588_CRU_RESET_OFFSET(SRST_RKVENC1_CORE, 48, 6),

	/* SOFTRST_CON49 */
	RK3588_CRU_RESET_OFFSET(SRST_A_VI_BIU, 49, 3),
	RK3588_CRU_RESET_OFFSET(SRST_H_VI_BIU, 49, 4),
	RK3588_CRU_RESET_OFFSET(SRST_P_VI_BIU, 49, 5),
	RK3588_CRU_RESET_OFFSET(SRST_D_VICAP, 49, 6),
	RK3588_CRU_RESET_OFFSET(SRST_A_VICAP, 49, 7),
	RK3588_CRU_RESET_OFFSET(SRST_H_VICAP, 49, 8),
	RK3588_CRU_RESET_OFFSET(SRST_ISP0, 49, 10),
	RK3588_CRU_RESET_OFFSET(SRST_ISP0_VICAP, 49, 11),

	/* SOFTRST_CON50 */
	RK3588_CRU_RESET_OFFSET(SRST_FISHEYE0, 50, 0),
	RK3588_CRU_RESET_OFFSET(SRST_FISHEYE1, 50, 3),
	RK3588_CRU_RESET_OFFSET(SRST_P_CSI_HOST_0, 50, 4),
	RK3588_CRU_RESET_OFFSET(SRST_P_CSI_HOST_1, 50, 5),
	RK3588_CRU_RESET_OFFSET(SRST_P_CSI_HOST_2, 50, 6),
	RK3588_CRU_RESET_OFFSET(SRST_P_CSI_HOST_3, 50, 7),
	RK3588_CRU_RESET_OFFSET(SRST_P_CSI_HOST_4, 50, 8),
	RK3588_CRU_RESET_OFFSET(SRST_P_CSI_HOST_5, 50, 9),

	/* SOFTRST_CON51 */
	RK3588_CRU_RESET_OFFSET(SRST_CSIHOST0_VICAP, 51, 4),
	RK3588_CRU_RESET_OFFSET(SRST_CSIHOST1_VICAP, 51, 5),
	RK3588_CRU_RESET_OFFSET(SRST_CSIHOST2_VICAP, 51, 6),
	RK3588_CRU_RESET_OFFSET(SRST_CSIHOST3_VICAP, 51, 7),
	RK3588_CRU_RESET_OFFSET(SRST_CSIHOST4_VICAP, 51, 8),
	RK3588_CRU_RESET_OFFSET(SRST_CSIHOST5_VICAP, 51, 9),
	RK3588_CRU_RESET_OFFSET(SRST_CIFIN, 51, 13),

	/* SOFTRST_CON52 */
	RK3588_CRU_RESET_OFFSET(SRST_A_VOP_BIU, 52, 4),
	RK3588_CRU_RESET_OFFSET(SRST_A_VOP_LOW_BIU, 52, 5),
	RK3588_CRU_RESET_OFFSET(SRST_H_VOP_BIU, 52, 6),
	RK3588_CRU_RESET_OFFSET(SRST_P_VOP_BIU, 52, 7),
	RK3588_CRU_RESET_OFFSET(SRST_H_VOP, 52, 8),
	RK3588_CRU_RESET_OFFSET(SRST_A_VOP, 52, 9),
	RK3588_CRU_RESET_OFFSET(SRST_D_VOP0, 52, 13),
	RK3588_CRU_RESET_OFFSET(SRST_D_VOP2HDMI_BRIDGE0, 52, 14),
	RK3588_CRU_RESET_OFFSET(SRST_D_VOP2HDMI_BRIDGE1, 52, 15),

	/* SOFTRST_CON53 */
	RK3588_CRU_RESET_OFFSET(SRST_D_VOP1, 53, 0),
	RK3588_CRU_RESET_OFFSET(SRST_D_VOP2, 53, 1),
	RK3588_CRU_RESET_OFFSET(SRST_D_VOP3, 53, 2),
	RK3588_CRU_RESET_OFFSET(SRST_P_VOPGRF, 53, 3),
	RK3588_CRU_RESET_OFFSET(SRST_P_DSIHOST0, 53, 4),
	RK3588_CRU_RESET_OFFSET(SRST_P_DSIHOST1, 53, 5),
	RK3588_CRU_RESET_OFFSET(SRST_DSIHOST0, 53, 6),
	RK3588_CRU_RESET_OFFSET(SRST_DSIHOST1, 53, 7),
	RK3588_CRU_RESET_OFFSET(SRST_VOP_PMU, 53, 8),
	RK3588_CRU_RESET_OFFSET(SRST_P_VOP_CHANNEL_BIU, 53, 9),

	/* SOFTRST_CON55 */
	RK3588_CRU_RESET_OFFSET(SRST_H_VO0_BIU, 55, 5),
	RK3588_CRU_RESET_OFFSET(SRST_H_VO0_S_BIU, 55, 6),
	RK3588_CRU_RESET_OFFSET(SRST_P_VO0_BIU, 55, 7),
	RK3588_CRU_RESET_OFFSET(SRST_P_VO0_S_BIU, 55, 8),
	RK3588_CRU_RESET_OFFSET(SRST_A_HDCP0_BIU, 55, 9),
	RK3588_CRU_RESET_OFFSET(SRST_P_VO0GRF, 55, 10),
	RK3588_CRU_RESET_OFFSET(SRST_H_HDCP_KEY0, 55, 11),
	RK3588_CRU_RESET_OFFSET(SRST_A_HDCP0, 55, 12),
	RK3588_CRU_RESET_OFFSET(SRST_H_HDCP0, 55, 13),
	RK3588_CRU_RESET_OFFSET(SRST_HDCP0, 55, 15),

	/* SOFTRST_CON56 */
	RK3588_CRU_RESET_OFFSET(SRST_P_TRNG0, 56, 1),
	RK3588_CRU_RESET_OFFSET(SRST_DP0, 56, 8),
	RK3588_CRU_RESET_OFFSET(SRST_DP1, 56, 9),
	RK3588_CRU_RESET_OFFSET(SRST_H_I2S4_8CH, 56, 10),
	RK3588_CRU_RESET_OFFSET(SRST_M_I2S4_8CH_TX, 56, 13),
	RK3588_CRU_RESET_OFFSET(SRST_H_I2S8_8CH, 56, 14),

	/* SOFTRST_CON57 */
	RK3588_CRU_RESET_OFFSET(SRST_M_I2S8_8CH_TX, 57, 1),
	RK3588_CRU_RESET_OFFSET(SRST_H_SPDIF2_DP0, 57, 2),
	RK3588_CRU_RESET_OFFSET(SRST_M_SPDIF2_DP0, 57, 6),
	RK3588_CRU_RESET_OFFSET(SRST_H_SPDIF5_DP1, 57, 7),
	RK3588_CRU_RESET_OFFSET(SRST_M_SPDIF5_DP1, 57, 11),

	/* SOFTRST_CON59 */
	RK3588_CRU_RESET_OFFSET(SRST_A_HDCP1_BIU, 59, 6),
	RK3588_CRU_RESET_OFFSET(SRST_A_VO1_BIU, 59, 8),
	RK3588_CRU_RESET_OFFSET(SRST_H_VOP1_BIU, 59, 9),
	RK3588_CRU_RESET_OFFSET(SRST_H_VOP1_S_BIU, 59, 10),
	RK3588_CRU_RESET_OFFSET(SRST_P_VOP1_BIU, 59, 11),
	RK3588_CRU_RESET_OFFSET(SRST_P_VO1GRF, 59, 12),
	RK3588_CRU_RESET_OFFSET(SRST_P_VO1_S_BIU, 59, 13),

	/* SOFTRST_CON60 */
	RK3588_CRU_RESET_OFFSET(SRST_H_I2S7_8CH, 60, 0),
	RK3588_CRU_RESET_OFFSET(SRST_M_I2S7_8CH_RX, 60, 3),
	RK3588_CRU_RESET_OFFSET(SRST_H_HDCP_KEY1, 60, 4),
	RK3588_CRU_RESET_OFFSET(SRST_A_HDCP1, 60, 5),
	RK3588_CRU_RESET_OFFSET(SRST_H_HDCP1, 60, 6),
	RK3588_CRU_RESET_OFFSET(SRST_HDCP1, 60, 8),
	RK3588_CRU_RESET_OFFSET(SRST_P_TRNG1, 60, 10),
	RK3588_CRU_RESET_OFFSET(SRST_P_HDMITX0, 60, 11),

	/* SOFTRST_CON61 */
	RK3588_CRU_RESET_OFFSET(SRST_HDMITX0_REF, 61, 0),
	RK3588_CRU_RESET_OFFSET(SRST_P_HDMITX1, 61, 2),
	RK3588_CRU_RESET_OFFSET(SRST_HDMITX1_REF, 61, 7),
	RK3588_CRU_RESET_OFFSET(SRST_A_HDMIRX, 61, 9),
	RK3588_CRU_RESET_OFFSET(SRST_P_HDMIRX, 61, 10),
	RK3588_CRU_RESET_OFFSET(SRST_HDMIRX_REF, 61, 11),

	/* SOFTRST_CON62 */
	RK3588_CRU_RESET_OFFSET(SRST_P_EDP0, 62, 0),
	RK3588_CRU_RESET_OFFSET(SRST_EDP0_24M, 62, 1),
	RK3588_CRU_RESET_OFFSET(SRST_P_EDP1, 62, 3),
	RK3588_CRU_RESET_OFFSET(SRST_EDP1_24M, 62, 4),
	RK3588_CRU_RESET_OFFSET(SRST_M_I2S5_8CH_TX, 62, 8),
	RK3588_CRU_RESET_OFFSET(SRST_H_I2S5_8CH, 62, 12),
	RK3588_CRU_RESET_OFFSET(SRST_M_I2S6_8CH_TX, 62, 15),

	/* SOFTRST_CON63 */
	RK3588_CRU_RESET_OFFSET(SRST_M_I2S6_8CH_RX, 63, 2),
	RK3588_CRU_RESET_OFFSET(SRST_H_I2S6_8CH, 63, 3),
	RK3588_CRU_RESET_OFFSET(SRST_H_SPDIF3, 63, 4),
	RK3588_CRU_RESET_OFFSET(SRST_M_SPDIF3, 63, 7),
	RK3588_CRU_RESET_OFFSET(SRST_H_SPDIF4, 63, 8),
	RK3588_CRU_RESET_OFFSET(SRST_M_SPDIF4, 63, 11),
	RK3588_CRU_RESET_OFFSET(SRST_H_SPDIFRX0, 63, 12),
	RK3588_CRU_RESET_OFFSET(SRST_M_SPDIFRX0, 63, 13),
	RK3588_CRU_RESET_OFFSET(SRST_H_SPDIFRX1, 63, 14),
	RK3588_CRU_RESET_OFFSET(SRST_M_SPDIFRX1, 63, 15),

	/* SOFTRST_CON64 */
	RK3588_CRU_RESET_OFFSET(SRST_H_SPDIFRX2, 64, 0),
	RK3588_CRU_RESET_OFFSET(SRST_M_SPDIFRX2, 64, 1),
	RK3588_CRU_RESET_OFFSET(SRST_LINKSYM_HDMITXPHY0, 64, 12),
	RK3588_CRU_RESET_OFFSET(SRST_LINKSYM_HDMITXPHY1, 64, 13),
	RK3588_CRU_RESET_OFFSET(SRST_VO1_BRIDGE0, 64, 14),
	RK3588_CRU_RESET_OFFSET(SRST_VO1_BRIDGE1, 64, 15),

	/* SOFTRST_CON65 */
	RK3588_CRU_RESET_OFFSET(SRST_H_I2S9_8CH, 65, 0),
	RK3588_CRU_RESET_OFFSET(SRST_M_I2S9_8CH_RX, 65, 3),
	RK3588_CRU_RESET_OFFSET(SRST_H_I2S10_8CH, 65, 4),
	RK3588_CRU_RESET_OFFSET(SRST_M_I2S10_8CH_RX, 65, 7),
	RK3588_CRU_RESET_OFFSET(SRST_P_S_HDMIRX, 65, 8),

	/* SOFTRST_CON66 */
	RK3588_CRU_RESET_OFFSET(SRST_GPU, 66, 4),
	RK3588_CRU_RESET_OFFSET(SRST_SYS_GPU, 66, 5),
	RK3588_CRU_RESET_OFFSET(SRST_A_S_GPU_BIU, 66, 8),
	RK3588_CRU_RESET_OFFSET(SRST_A_M0_GPU_BIU, 66, 9),
	RK3588_CRU_RESET_OFFSET(SRST_A_M1_GPU_BIU, 66, 10),
	RK3588_CRU_RESET_OFFSET(SRST_A_M2_GPU_BIU, 66, 11),
	RK3588_CRU_RESET_OFFSET(SRST_A_M3_GPU_BIU, 66, 12),
	RK3588_CRU_RESET_OFFSET(SRST_P_GPU_BIU, 66, 14),
	RK3588_CRU_RESET_OFFSET(SRST_P_GPU_PVTM, 66, 15),

	/* SOFTRST_CON67 */
	RK3588_CRU_RESET_OFFSET(SRST_GPU_PVTM, 67, 0),
	RK3588_CRU_RESET_OFFSET(SRST_P_GPU_GRF, 67, 2),
	RK3588_CRU_RESET_OFFSET(SRST_GPU_PVTPLL, 67, 3),
	RK3588_CRU_RESET_OFFSET(SRST_GPU_JTAG, 67, 4),

	/* SOFTRST_CON68 */
	RK3588_CRU_RESET_OFFSET(SRST_A_AV1_BIU, 68, 1),
	RK3588_CRU_RESET_OFFSET(SRST_A_AV1, 68, 2),
	RK3588_CRU_RESET_OFFSET(SRST_P_AV1_BIU, 68, 4),
	RK3588_CRU_RESET_OFFSET(SRST_P_AV1, 68, 5),

	/* SOFTRST_CON69 */
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR_BIU, 69, 4),
	RK3588_CRU_RESET_OFFSET(SRST_A_DMA2DDR, 69, 5),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR_SHAREMEM, 69, 6),
	RK3588_CRU_RESET_OFFSET(SRST_A_DDR_SHAREMEM_BIU, 69, 7),
	RK3588_CRU_RESET_OFFSET(SRST_A_CENTER_S200_BIU, 69, 10),
	RK3588_CRU_RESET_OFFSET(SRST_A_CENTER_S400_BIU, 69, 11),
	RK3588_CRU_RESET_OFFSET(SRST_H_AHB2APB, 69, 12),
	RK3588_CRU_RESET_OFFSET(SRST_H_CENTER_BIU, 69, 13),
	RK3588_CRU_RESET_OFFSET(SRST_F_DDR_CM0_CORE, 69, 14),

	/* SOFTRST_CON70 */
	RK3588_CRU_RESET_OFFSET(SRST_DDR_TIMER0, 70, 0),
	RK3588_CRU_RESET_OFFSET(SRST_DDR_TIMER1, 70, 1),
	RK3588_CRU_RESET_OFFSET(SRST_T_WDT_DDR, 70, 2),
	RK3588_CRU_RESET_OFFSET(SRST_T_DDR_CM0_JTAG, 70, 3),
	RK3588_CRU_RESET_OFFSET(SRST_P_CENTER_GRF, 70, 5),
	RK3588_CRU_RESET_OFFSET(SRST_P_AHB2APB, 70, 6),
	RK3588_CRU_RESET_OFFSET(SRST_P_WDT, 70, 7),
	RK3588_CRU_RESET_OFFSET(SRST_P_TIMER, 70, 8),
	RK3588_CRU_RESET_OFFSET(SRST_P_DMA2DDR, 70, 9),
	RK3588_CRU_RESET_OFFSET(SRST_P_SHAREMEM, 70, 10),
	RK3588_CRU_RESET_OFFSET(SRST_P_CENTER_BIU, 70, 11),
	RK3588_CRU_RESET_OFFSET(SRST_P_CENTER_CHANNEL_BIU, 70, 12),

	/* SOFTRST_CON72 */
	RK3588_CRU_RESET_OFFSET(SRST_P_USBDPGRF0, 72, 1),
	RK3588_CRU_RESET_OFFSET(SRST_P_USBDPPHY0, 72, 2),
	RK3588_CRU_RESET_OFFSET(SRST_P_USBDPGRF1, 72, 3),
	RK3588_CRU_RESET_OFFSET(SRST_P_USBDPPHY1, 72, 4),
	RK3588_CRU_RESET_OFFSET(SRST_P_HDPTX0, 72, 5),
	RK3588_CRU_RESET_OFFSET(SRST_P_HDPTX1, 72, 6),
	RK3588_CRU_RESET_OFFSET(SRST_P_APB2ASB_SLV_BOT_RIGHT, 72, 7),
	RK3588_CRU_RESET_OFFSET(SRST_P_USB2PHY_U3_0_GRF0, 72, 8),
	RK3588_CRU_RESET_OFFSET(SRST_P_USB2PHY_U3_1_GRF0, 72, 9),
	RK3588_CRU_RESET_OFFSET(SRST_P_USB2PHY_U2_0_GRF0, 72, 10),
	RK3588_CRU_RESET_OFFSET(SRST_P_USB2PHY_U2_1_GRF0, 72, 11),
	RK3588_CRU_RESET_OFFSET(SRST_HDPTX0_ROPLL, 72, 12), // missing in TRM
	RK3588_CRU_RESET_OFFSET(SRST_HDPTX0_LCPLL, 72, 13), // missing in TRM
	RK3588_CRU_RESET_OFFSET(SRST_HDPTX0, 72, 14), // missing in TRM
	RK3588_CRU_RESET_OFFSET(SRST_HDPTX1_ROPLL, 72, 15), // missing in TRM

	/* SOFTRST_CON73 */
	RK3588_CRU_RESET_OFFSET(SRST_HDPTX1_LCPLL, 73, 0), // missing in TRM
	RK3588_CRU_RESET_OFFSET(SRST_HDPTX1, 73, 1), // missing in TRM
	RK3588_CRU_RESET_OFFSET(SRST_HDPTX0_HDMIRXPHY_SET, 73, 2), // missing in TRM
	RK3588_CRU_RESET_OFFSET(SRST_USBDP_COMBO_PHY0, 73, 3), // missing in TRM
	RK3588_CRU_RESET_OFFSET(SRST_USBDP_COMBO_PHY0_LCPLL, 73, 4), // missing in TRM
	RK3588_CRU_RESET_OFFSET(SRST_USBDP_COMBO_PHY0_ROPLL, 73, 5), // missing in TRM
	RK3588_CRU_RESET_OFFSET(SRST_USBDP_COMBO_PHY0_PCS_HS, 73, 6), // missing in TRM
	RK3588_CRU_RESET_OFFSET(SRST_USBDP_COMBO_PHY1, 73, 7), // missing in TRM
	RK3588_CRU_RESET_OFFSET(SRST_USBDP_COMBO_PHY1_LCPLL, 73, 8), // missing in TRM
	RK3588_CRU_RESET_OFFSET(SRST_USBDP_COMBO_PHY1_ROPLL, 73, 9), // missing in TRM
	RK3588_CRU_RESET_OFFSET(SRST_USBDP_COMBO_PHY1_PCS_HS, 73, 10), // missing in TRM
	RK3588_CRU_RESET_OFFSET(SRST_HDMIHDP0, 73, 12),
	RK3588_CRU_RESET_OFFSET(SRST_HDMIHDP1, 73, 13),

	/* SOFTRST_CON74 */
	RK3588_CRU_RESET_OFFSET(SRST_A_VO1USB_TOP_BIU, 74, 1),
	RK3588_CRU_RESET_OFFSET(SRST_H_VO1USB_TOP_BIU, 74, 3),

	/* SOFTRST_CON75 */
	RK3588_CRU_RESET_OFFSET(SRST_H_SDIO_BIU, 75, 1),
	RK3588_CRU_RESET_OFFSET(SRST_H_SDIO, 75, 2),
	RK3588_CRU_RESET_OFFSET(SRST_SDIO, 75, 3),

	/* SOFTRST_CON76 */
	RK3588_CRU_RESET_OFFSET(SRST_H_RGA3_BIU, 76, 2),
	RK3588_CRU_RESET_OFFSET(SRST_A_RGA3_BIU, 76, 3),
	RK3588_CRU_RESET_OFFSET(SRST_H_RGA3_1, 76, 4),
	RK3588_CRU_RESET_OFFSET(SRST_A_RGA3_1, 76, 5),
	RK3588_CRU_RESET_OFFSET(SRST_RGA3_1_CORE, 76, 6),

	/* SOFTRST_CON77 */
	RK3588_CRU_RESET_OFFSET(SRST_REF_PIPE_PHY0, 77, 6),
	RK3588_CRU_RESET_OFFSET(SRST_REF_PIPE_PHY1, 77, 7),
	RK3588_CRU_RESET_OFFSET(SRST_REF_PIPE_PHY2, 77, 8),

	/* PHPTOPCRU_SOFTRST_CON00 */
	RK3588_PHPTOPCRU_RESET_OFFSET(SRST_P_PHPTOP_CRU, 0, 1),
	RK3588_PHPTOPCRU_RESET_OFFSET(SRST_P_PCIE2_GRF0, 0, 2),
	RK3588_PHPTOPCRU_RESET_OFFSET(SRST_P_PCIE2_GRF1, 0, 3),
	RK3588_PHPTOPCRU_RESET_OFFSET(SRST_P_PCIE2_GRF2, 0, 4),
	RK3588_PHPTOPCRU_RESET_OFFSET(SRST_P_PCIE2_PHY0, 0, 5),
	RK3588_PHPTOPCRU_RESET_OFFSET(SRST_P_PCIE2_PHY1, 0, 6),
	RK3588_PHPTOPCRU_RESET_OFFSET(SRST_P_PCIE2_PHY2, 0, 7),
	RK3588_PHPTOPCRU_RESET_OFFSET(SRST_P_PCIE3_PHY, 0, 8),
	RK3588_PHPTOPCRU_RESET_OFFSET(SRST_P_APB2ASB_SLV_CHIP_TOP, 0, 9),
	RK3588_PHPTOPCRU_RESET_OFFSET(SRST_PCIE30_PHY, 0, 10),

	/* PMU1CRU_SOFTRST_CON00 */
	RK3588_PMU1CRU_RESET_OFFSET(SRST_H_PMU1_BIU, 0, 10),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_P_PMU1_BIU, 0, 11),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_H_PMU_CM0_BIU, 0, 12),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_F_PMU_CM0_CORE, 0, 13),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_T_PMU1_CM0_JTAG, 0, 14),

	/* PMU1CRU_SOFTRST_CON01 */
	RK3588_PMU1CRU_RESET_OFFSET(SRST_DDR_FAIL_SAFE, 1, 1),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_P_CRU_PMU1, 1, 2),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_P_PMU1_GRF, 1, 4),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_P_PMU1_IOC, 1, 5),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_P_PMU1WDT, 1, 6),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_T_PMU1WDT, 1, 7),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_P_PMU1TIMER, 1, 8),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_PMU1TIMER0, 1, 10),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_PMU1TIMER1, 1, 11),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_P_PMU1PWM, 1, 12),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_PMU1PWM, 1, 13),

	/* PMU1CRU_SOFTRST_CON02 */
	RK3588_PMU1CRU_RESET_OFFSET(SRST_P_I2C0, 2, 1),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_I2C0, 2, 2),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_S_UART0, 2, 5),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_P_UART0, 2, 6),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_H_I2S1_8CH, 2, 7),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_M_I2S1_8CH_TX, 2, 10),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_M_I2S1_8CH_RX, 2, 13),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_H_PDM0, 2, 14),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_PDM0, 2, 15),

	/* PMU1CRU_SOFTRST_CON03 */
	RK3588_PMU1CRU_RESET_OFFSET(SRST_H_VAD, 3, 0),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_HDPTX0_INIT, 3, 11),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_HDPTX0_CMN, 3, 12),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_HDPTX0_LANE, 3, 13),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_HDPTX1_INIT, 3, 15),

	/* PMU1CRU_SOFTRST_CON04 */
	RK3588_PMU1CRU_RESET_OFFSET(SRST_HDPTX1_CMN, 4, 0),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_HDPTX1_LANE, 4, 1),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_M_MIPI_DCPHY0, 4, 3),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_S_MIPI_DCPHY0, 4, 4),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_M_MIPI_DCPHY1, 4, 5),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_S_MIPI_DCPHY1, 4, 6),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_OTGPHY_U3_0, 4, 7),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_OTGPHY_U3_1, 4, 8),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_OTGPHY_U2_0, 4, 9),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_OTGPHY_U2_1, 4, 10),

	/* PMU1CRU_SOFTRST_CON05 */
	RK3588_PMU1CRU_RESET_OFFSET(SRST_P_PMU0GRF, 5, 3),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_P_PMU0IOC, 5, 4),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_P_GPIO0, 5, 5),
	RK3588_PMU1CRU_RESET_OFFSET(SRST_GPIO0, 5, 6),

	/* SECURECRU_SOFTRST_CON00 */
	RK3588_SECURECRU_RESET_OFFSET(SRST_A_SECURE_NS_BIU, 0, 10),
	RK3588_SECURECRU_RESET_OFFSET(SRST_H_SECURE_NS_BIU, 0, 11),
	RK3588_SECURECRU_RESET_OFFSET(SRST_A_SECURE_S_BIU, 0, 12),
	RK3588_SECURECRU_RESET_OFFSET(SRST_H_SECURE_S_BIU, 0, 13),
	RK3588_SECURECRU_RESET_OFFSET(SRST_P_SECURE_S_BIU, 0, 14),
	RK3588_SECURECRU_RESET_OFFSET(SRST_CRYPTO_CORE, 0, 15),

	/* SECURECRU_SOFTRST_CON01 */
	RK3588_SECURECRU_RESET_OFFSET(SRST_CRYPTO_PKA, 1, 0),
	RK3588_SECURECRU_RESET_OFFSET(SRST_CRYPTO_RNG, 1, 1),
	RK3588_SECURECRU_RESET_OFFSET(SRST_A_CRYPTO, 1, 2),
	RK3588_SECURECRU_RESET_OFFSET(SRST_H_CRYPTO, 1, 3),
	RK3588_SECURECRU_RESET_OFFSET(SRST_KEYLADDER_CORE, 1, 9),
	RK3588_SECURECRU_RESET_OFFSET(SRST_KEYLADDER_RNG, 1, 10),
	RK3588_SECURECRU_RESET_OFFSET(SRST_A_KEYLADDER, 1, 11),
	RK3588_SECURECRU_RESET_OFFSET(SRST_H_KEYLADDER, 1, 12),
	RK3588_SECURECRU_RESET_OFFSET(SRST_P_OTPC_S, 1, 13),
	RK3588_SECURECRU_RESET_OFFSET(SRST_OTPC_S, 1, 14),
	RK3588_SECURECRU_RESET_OFFSET(SRST_WDT_S, 1, 15),

	/* SECURECRU_SOFTRST_CON02 */
	RK3588_SECURECRU_RESET_OFFSET(SRST_T_WDT_S, 2, 0),
	RK3588_SECURECRU_RESET_OFFSET(SRST_H_BOOTROM, 2, 1),
	RK3588_SECURECRU_RESET_OFFSET(SRST_A_DCF, 2, 2),
	RK3588_SECURECRU_RESET_OFFSET(SRST_P_DCF, 2, 3),
	RK3588_SECURECRU_RESET_OFFSET(SRST_H_BOOTROM_NS, 2, 5),
	RK3588_SECURECRU_RESET_OFFSET(SRST_P_KEYLADDER, 2, 14),
	RK3588_SECURECRU_RESET_OFFSET(SRST_H_TRNG_S, 2, 15),

	/* SECURECRU_SOFTRST_CON03 */
	RK3588_SECURECRU_RESET_OFFSET(SRST_H_TRNG_NS, 3, 0),
	RK3588_SECURECRU_RESET_OFFSET(SRST_D_SDMMC_BUFFER, 3, 1),
	RK3588_SECURECRU_RESET_OFFSET(SRST_H_SDMMC, 3, 2),
	RK3588_SECURECRU_RESET_OFFSET(SRST_H_SDMMC_BUFFER, 3, 3),
	RK3588_SECURECRU_RESET_OFFSET(SRST_SDMMC, 3, 4),
	RK3588_SECURECRU_RESET_OFFSET(SRST_P_TRNG_CHK, 3, 5),
	RK3588_SECURECRU_RESET_OFFSET(SRST_TRNG_S, 3, 6),
};

void rk3588_rst_init(struct device_node *np, void __iomem *reg_base)
{
	rockchip_register_softrst_lut(np,
				      rk3588_register_offset,
				      ARRAY_SIZE(rk3588_register_offset),
				      reg_base + RK3588_SOFTRST_CON(0),
				      ROCKCHIP_SOFTRST_HIWORD_MASK);
}
