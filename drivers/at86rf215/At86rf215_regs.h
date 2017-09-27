/**
 * @file       At86rf215_regs.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2016
 * @brief
 *
 * @copyright  Copyright 2016, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef AT86RF215_REGS_H_
#define AT86RF215_REGS_H_

#define RF09_IRQS_ADDR				( 0x0000 )
#define RF24_IRQS_ADDR				( 0x0001 )
#define BBC0_IRQS_ADDR				( 0x0002 )
#define BBC1_IRQS_ADDR				( 0x0003 )
#define RF_RST_ADDR					( 0x0005 )
#define RF_CFG_ADDR					( 0x0006 )
#define RF_CLK0_ADDR				( 0x0007 )
#define RF_BMDVC_ADDR				( 0x0008 )
#define RF_XOC_ADDR					( 0x0009 )
#define RF_IQIFC0_ADDR				( 0x000A )
#define RF_IQIFC1_ADDR				( 0x000B )
#define RF_IQIFC2_ADDR				( 0x000C )
#define RF_PN_ADDR					( 0x000D )
#define RF_VN_ADDR					( 0x000E )

#define RF09_IRQM_ADDR				( 0x0100 )
#define RF09_AUXS_ADDR				( 0x0101 )
#define RF09_STATE_ADDR				( 0x0102 )
#define RF09_CMD_ADDR				( 0x0103 )
#define RF09_CS_ADDR				( 0x0104 )
#define RF09_CCF0L_ADDR				( 0x0105 )
#define RF09_CCF0H_ADDR				( 0x0106 )
#define RF09_CNL_ADDR				( 0x0107 )
#define RF09_CNM_ADDR				( 0x0108 )
#define RF09_RXBWC_ADDR				( 0x0109 )
#define RF09_RXDFE_ADDR				( 0x010A )
#define RF09_AGCC_ADDR				( 0x010B )
#define RF09_AGCS_ADDR				( 0x010C )
#define RF09_RSSI_ADDR				( 0x010D )
#define RF09_EDC_ADDR				( 0x010E )
#define RF09_EDD_ADDR				( 0x010F )
#define RF09_EDV_ADDR				( 0x0110 )
#define RF09_RNDV_ADDR				( 0x0111 )
#define RF09_TXCUTC_ADDR			( 0x0112 )
#define RF09_TXDFE_ADDR				( 0x0113 )
#define RF09_PAC_ADDR				( 0x0114 )
#define RF09_PADFE_ADDR				( 0x0116 )
#define RF09_PLL_ADDR				( 0x0121 )
#define RF09_PLLCF_ADDR				( 0x0122 )
#define RF09_TXCI_ADDR				( 0x0125 )
#define RF09_TXCQ_ADDR				( 0x0126 )
#define RF09_TXDACI_ADDR			( 0x0127 )
#define RF09_TXDACQ_ADDR			( 0x0128 )

#define RF24_IRQM_ADDR				( 0x0200 )
#define RF24_AUXS_ADDR				( 0x0201 )
#define RF24_STATE_ADDR				( 0x0202 )
#define RF24_CMD_ADDR				( 0x0203 )
#define RF24_CS_ADDR				( 0x0204 )
#define RF24_CCF0L_ADDR				( 0x0205 )
#define RF24_CCF0H_ADDR				( 0x0206 )
#define RF24_CNL_ADDR				( 0x0207 )
#define RF24_CNM_ADDR				( 0x0208 )
#define RF24_RXBWC_ADDR				( 0x0209 )
#define RF24_RXDFE_ADDR				( 0x020A )
#define RF24_AGCC_ADDR				( 0x020B )
#define RF24_AGCS_ADDR				( 0x020C )
#define RF24_RSSI_ADDR				( 0x020D )
#define RF24_EDC_ADDR				( 0x020E )
#define RF24_EDD_ADDR				( 0x020F )
#define RF24_EDV_ADDR				( 0x0210 )
#define RF24_RNDV_ADDR				( 0x0211 )
#define RF24_TXCUTC_ADDR			( 0x0212 )
#define RF24_TXDFE_ADDR				( 0x0213 )
#define RF24_PAC_ADDR				( 0x0214 )
#define RF24_PADFE_ADDR				( 0x0216 )
#define RF24_PLL_ADDR				( 0x0221 )
#define RF24_PLLCF_ADDR				( 0x0222 )
#define RF24_TXCI_ADDR				( 0x0225 )
#define RF24_TXCQ_ADDR				( 0x0226 )
#define RF24_TXDACI_ADDR			( 0x0227 )
#define RF24_TXDACQ_ADDR			( 0x0228 )

#endif /* AT86RF215_REGS_H_7*/