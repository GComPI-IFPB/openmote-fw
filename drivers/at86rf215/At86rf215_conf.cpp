/**
 * @file       At86rf215_conf.cpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2016
 * @brief
 *
 * @copyright  Copyright 2016, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

/*================================ include ==================================*/

#include "At86rf215_conf.h"
#include "At86rf215_regs.h"

/*================================ define ===================================*/

/*================================ typedef ==================================*/

/*=============================== variables =================================*/

/*=============================== prototypes ================================*/

/*================================= public ==================================*/

/*=============================== protected =================================*/

/*================================ private ==================================*/

const register_t fsk_option1[] = 
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x00},
  {RF09_RXDFE,     0x2A},
  {RF09_AGCC,      0x01},
  {RF09_AGCS,      0x37},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0xC0},
  {RF09_TXDFE,     0x98},
  {RF09_PAC,       0x7F},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x15},
  {BBC0_FSKDM,     0x01},
  {BBC0_FSKC0,     0xD6},
  {BBC0_FSKC1,     0x00},
  {BBC0_FSKC2,     0x41},
  {BBC0_FSKC3,     0x85},
  {BBC0_FSKC4,     0x0A},
  {BBC0_FSKPE0,    0x02},
  {BBC0_FSKPE1,    0x03},
  {BBC0_FSKPE2,    0xFC},
  {BBC0_FSKPHRTX,  0x00},
};
const uint16_t fsk_option1_size = sizeof(fsk_option1) / sizeof(register_t);
const radio_settings_t fsk_option1_cfg = {fsk_option1, fsk_option1_size};

const register_t fsk_option2[] =
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x03},
  {RF09_RXDFE,     0x15},
  {RF09_AGCC,      0x01},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x83},
  {RF09_TXDFE,     0x94},
  {RF09_PAC,       0x7F},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x1D},
  {BBC0_FSKDM,     0x03},
  {BBC0_FSKC0,     0xD6},
  {BBC0_FSKC1,     0x01},
  {BBC0_FSKC2,     0x40},
  {BBC0_FSKC3,     0x85},
  {BBC0_FSKC4,     0x0A},
  {BBC0_FSKPE0,    0x13},
  {BBC0_FSKPE1,    0x29},
  {BBC0_FSKPE2,    0xC7},
  {BBC0_FSKPHRTX,  0x00},
};
const uint16_t fsk_option2_size = sizeof(fsk_option2) / sizeof(register_t);
const radio_settings_t fsk_option2_cfg = {fsk_option2, fsk_option2_size};

const register_t fsk_option3[] =
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x03},
  {RF09_RXDFE,     0x24},
  {RF09_AGCC,      0x01},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x84},
  {RF09_TXDFE,     0x12},
  {RF09_PAC,       0x64},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x15},
  {BBC0_FSKDM,     0x03},
  {BBC0_FSKC0,     0xC1},
  {BBC0_FSKC1,     0x03},
  {BBC0_FSKC2,     0x00},
  {BBC0_FSKC3,     0x85},
  {BBC0_FSKC4,     0x00},
  {BBC0_FSKPE0,    0x74},
  {BBC0_FSKPE1,    0x7F},
  {BBC0_FSKPE2,    0x80},
  {BBC0_FSKPHRTX,  0x00},
};
const uint16_t fsk_option3_size = sizeof(fsk_option3) / sizeof(register_t);
const radio_settings_t fsk_option3_cfg = {fsk_option3, fsk_option3_size};

const register_t oqpsk_rate1[] =
{
  {BBC0_PC,         0x17},
  {BBC0_OQPSKPHRTX, 0x00},
  {BBC0_OQPSKC0,    0x10},
  {BBC0_IRQM,       0x13},
  {BBC1_IRQM,       0x00},
  {RF09_IRQM,       0x00},
  {RF24_IRQM,       0x00},
  {RF09_RXBWC,      0x00},
  {RF09_RXDFE,      0x2A},
  {RF09_AGCC,       0x21},
  {RF09_EDD,        0x2B},
  {RF09_AGCS,       0x77},
  {RF09_TXCUTC,     0xC7},
  {RF09_TXDFE,      0x7A},
  {RF09_PAC,        0x64},
};
const uint16_t oqpsk_rate1_size = sizeof(oqpsk_rate1) / sizeof(register_t);
const radio_settings_t oqpsk_rate1_cfg = {oqpsk_rate1, oqpsk_rate1_size};

const register_t oqpsk_rate2[] =
{
  {BBC0_PC,         0x17},
  {BBC0_OQPSKPHRTX, 0x02},
  {BBC0_OQPSKC0,    0x10},
  {BBC0_IRQM,       0x13},
  {BBC1_IRQM,       0x00},
  {RF09_IRQM,       0x00},
  {RF24_IRQM,       0x00},
  {RF09_RXBWC,      0x00},
  {RF09_RXDFE,      0x2A},
  {RF09_AGCC,       0x21},
  {RF09_EDD,        0x2B},
  {RF09_AGCS,       0x77},
  {RF09_TXCUTC,     0xC7},
  {RF09_TXDFE,      0x7A},
  {RF09_PAC,        0x64},
};
const uint16_t oqpsk_rate2_size = sizeof(oqpsk_rate2) / sizeof(register_t);
const radio_settings_t oqpsk_rate2_cfg = {oqpsk_rate2, oqpsk_rate2_size};

const register_t oqpsk_rate3[] =
{
  {BBC0_PC,         0x17},
  {BBC0_OQPSKPHRTX, 0x04},
  {BBC0_OQPSKC0,    0x10},
  {BBC0_IRQM,       0x13},
  {BBC1_IRQM,       0x00},
  {RF09_IRQM,       0x00},
  {RF24_IRQM,       0x00},
  {RF09_RXBWC,      0x00},
  {RF09_RXDFE,      0x2A},
  {RF09_AGCC,       0x21},
  {RF09_EDD,        0x2B},
  {RF09_AGCS,       0x77},
  {RF09_TXCUTC,     0xC7},
  {RF09_TXDFE,      0x7A},
  {RF09_PAC,        0x64},
};
const uint16_t oqpsk_rate3_size = sizeof(oqpsk_rate3) / sizeof(register_t);
const radio_settings_t oqpsk_rate3_cfg = {oqpsk_rate3, oqpsk_rate3_size};

const register_t oqpsk_rate4[] =
{
  {BBC0_PC,         0x17},
  {BBC0_OQPSKPHRTX, 0x06},
  {BBC0_OQPSKC0,    0x10},
  {BBC0_IRQM,       0x13},
  {BBC1_IRQM,       0x00},
  {RF09_IRQM,       0x00},
  {RF24_IRQM,       0x00},
  {RF09_RXBWC,      0x00},
  {RF09_RXDFE,      0x2A},
  {RF09_AGCC,       0x21},
  {RF09_EDD,        0x2B},
  {RF09_AGCS,       0x77},
  {RF09_TXCUTC,     0xC7},
  {RF09_TXDFE,      0x7A},
  {RF09_PAC,        0x64}, 
};
const uint16_t oqpsk_rate4_size = sizeof(oqpsk_rate4) / sizeof(register_t);
const radio_settings_t oqpsk_rate4_cfg = {oqpsk_rate4, oqpsk_rate4_size};

const register_t oqpsk_250kbps[] =
{
  {BBC1_PC,         0x1F},
  {BBC1_OQPSKPHRTX, 0x09},
  {BBC1_OQPSKC0,    0x03},
  {BBC1_OQPSKC1,    0x47},
  {BBC1_OQPSKC2,    0x05},
  {BBC1_OQPSKC3,    0x00},
  {BBC0_IRQM,       0x00},
  {BBC1_IRQM,       0x1F},
  {RF09_IRQM,       0x00},
  {RF24_IRQM,       0x00},
  {RF24_RXBWC,      0x0B},
  {RF24_RXDFE,      0x41},
  {RF24_AGCC,       0x01},
  {RF24_EDD,        0x13},
  {RF24_AGCS,       0x77},
  {RF24_TXCUTC,     0x0B},
  {RF24_TXDFE,      0x81},
  {RF24_PAC,        0x7F},
};
uint16_t oqpsk_250kbps_size = sizeof(oqpsk_250kbps) / sizeof(const register_t);
radio_settings_t oqpsk_250kbps_cfg = {oqpsk_250kbps, oqpsk_250kbps_size};

const register_t ofdm_1_mcs0[] =
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x00},
  {RF09_RXDFE,     0x83},
  {RF09_AGCC,      0x11},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x0A},
  {RF09_TXDFE,     0x83},
  {RF09_PAC,       0x64},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x16},
  {BBC0_OFDMC,     0x00},
  {BBC0_OFDMPHRTX, 0x00},
};
uint16_t ofdm_1_mcs0_size = sizeof(ofdm_1_mcs0) / sizeof(register_t);
radio_settings_t ofdm_1_mcs0_cfg = {ofdm_1_mcs0, ofdm_1_mcs0_size};

const register_t ofdm_1_mcs1[] =
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x19},
  {RF09_RXDFE,     0x83},
  {RF09_AGCC,      0x11},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x0A},
  {RF09_TXDFE,     0x83},
  {RF09_PAC,       0x64},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x16},
  {BBC0_OFDMC,     0x00},
  {BBC0_OFDMPHRTX, 0x01},
};
uint16_t ofdm_1_mcs1_size = sizeof(ofdm_1_mcs1) / sizeof(register_t);
radio_settings_t ofdm_1_mcs1_cfg = {ofdm_1_mcs1, ofdm_1_mcs1_size};

const register_t ofdm_1_mcs2[] = {
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x19},
  {RF09_RXDFE,     0x83},
  {RF09_AGCC,      0x11},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x0A},
  {RF09_TXDFE,     0x83},
  {RF09_PAC,       0x64},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x16},
  {BBC0_OFDMC,     0x00},
  {BBC0_OFDMPHRTX, 0x02},
};
uint16_t ofdm_1_mcs2_size = sizeof(ofdm_1_mcs2) / sizeof(register_t);
radio_settings_t ofdm_1_mcs2_cfg = {ofdm_1_mcs2, ofdm_1_mcs2_size};

const register_t ofdm_1_mcs3[] = {
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x19},
  {RF09_RXDFE,     0x83},
  {RF09_AGCC,      0x11},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x0A},
  {RF09_TXDFE,     0x83},
  {RF09_PAC,       0x7F},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x16},
  {BBC0_OFDMC,     0x00},
  {BBC0_OFDMPHRTX, 0x03},
};
const uint16_t ofdm_1_mcs3_size = sizeof(ofdm_1_mcs3) / sizeof(register_t);
const radio_settings_t ofdm_1_mcs3_cfg = {ofdm_1_mcs3, ofdm_1_mcs3_size};

const register_t ofdm_2_mcs0[] =
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x17},
  {RF09_RXDFE,     0x43},
  {RF09_AGCC,      0x11},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x08},
  {RF09_TXDFE,     0x63},
  {RF09_PAC,       0x64},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x16},
  {BBC0_OFDMC,     0x01},
  {BBC0_OFDMPHRTX, 0x00},
};
const uint16_t ofdm_2_mcs0_size = sizeof(ofdm_2_mcs0) / sizeof(register_t);
const radio_settings_t ofdm_2_mcs0_cfg = {ofdm_2_mcs0, ofdm_2_mcs0_size};

const register_t ofdm_2_mcs1[] =
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x17},
  {RF09_RXDFE,     0x43},
  {RF09_AGCC,      0x11},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x08},
  {RF09_TXDFE,     0x63},
  {RF09_PAC,       0x64},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x16},
  {BBC0_OFDMC,     0x01},
  {BBC0_OFDMPHRTX, 0x01},
};
const uint16_t ofdm_2_mcs1_size = sizeof(ofdm_2_mcs1) / sizeof(register_t);
const radio_settings_t ofdm_2_mcs1_cfg = {ofdm_2_mcs1, ofdm_2_mcs1_size};

const register_t ofdm_2_mcs2[] =
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x17},
  {RF09_RXDFE,     0x43},
  {RF09_AGCC,      0x11},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x08},
  {RF09_TXDFE,     0x63},
  {RF09_PAC,       0x64},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x16},
  {BBC0_OFDMC,     0x01},
  {BBC0_OFDMPHRTX, 0x02},
};
const uint16_t ofdm_2_mcs2_size = sizeof(ofdm_2_mcs2) / sizeof(register_t);
const radio_settings_t ofdm_2_mcs2_cfg = {ofdm_2_mcs2, ofdm_2_mcs2_size};

const register_t ofdm_2_mcs3[] =
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x17},
  {RF09_RXDFE,     0x43},
  {RF09_AGCC,      0x11},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x08},
  {RF09_TXDFE,     0x63},
  {RF09_PAC,       0x64},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x16},
  {BBC0_OFDMC,     0x01},
  {BBC0_OFDMPHRTX, 0x03},
};
const uint16_t ofdm_2_mcs3_size = sizeof(ofdm_2_mcs3) / sizeof(register_t);
const radio_settings_t ofdm_2_mcs3_cfg = {ofdm_2_mcs3, ofdm_2_mcs3_size};

const register_t ofdm_2_mcs4[] =
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x17},
  {RF09_RXDFE,     0x43},
  {RF09_AGCC,      0x11},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x08},
  {RF09_TXDFE,     0x63},
  {RF09_PAC,       0x64},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x16},
  {BBC0_OFDMC,     0x01},
  {BBC0_OFDMPHRTX, 0x04},
};
const uint16_t ofdm_2_mcs4_size = sizeof(ofdm_2_mcs4) / sizeof(register_t);
const radio_settings_t ofdm_2_mcs4_cfg = {ofdm_2_mcs4, ofdm_2_mcs4_size};

const register_t ofdm_2_mcs5[] =
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x17},
  {RF09_RXDFE,     0x43},
  {RF09_AGCC,      0x11},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x08},
  {RF09_TXDFE,     0x63},
  {RF09_PAC,       0x64},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x16},
  {BBC0_OFDMC,     0x01},
  {BBC0_OFDMPHRTX, 0x05},
};
const uint16_t ofdm_2_mcs5_size = sizeof(ofdm_2_mcs5) / sizeof(register_t);
const radio_settings_t ofdm_2_mcs5_cfg = {ofdm_2_mcs5, ofdm_2_mcs5_size};

const register_t ofdm_3_mcs1[] =
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x04},
  {RF09_RXDFE,     0x46},
  {RF09_AGCC,      0x11},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x05},
  {RF09_TXDFE,     0x66},
  {RF09_PAC,       0x64},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x16},
  {BBC0_OFDMC,     0x02},
  {BBC0_OFDMPHRTX, 0x01},
};
const uint16_t ofdm_3_mcs1_size = sizeof(ofdm_3_mcs1) / sizeof(register_t);
const radio_settings_t ofdm_3_mcs1_cfg = {ofdm_3_mcs1, ofdm_3_mcs1_size};

const register_t ofdm_3_mcs2[] =
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x04},
  {RF09_RXDFE,     0x46},
  {RF09_AGCC,      0x11},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x05},
  {RF09_TXDFE,     0x66},
  {RF09_PAC,       0x64},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x16},
  {BBC0_OFDMC,     0x02},
  {BBC0_OFDMPHRTX, 0x02},
};
const uint16_t ofdm_3_mcs2_size = sizeof(ofdm_3_mcs2) / sizeof(register_t);
const radio_settings_t ofdm_3_mcs2_cfg = {ofdm_3_mcs2, ofdm_3_mcs2_size};

const register_t ofdm_3_mcs3[] =
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x04},
  {RF09_RXDFE,     0x46},
  {RF09_AGCC,      0x11},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x05},
  {RF09_TXDFE,     0x66},
  {RF09_PAC,       0x64},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x16},
  {BBC0_OFDMC,     0x02},
  {BBC0_OFDMPHRTX, 0x03},
};
const uint16_t ofdm_3_mcs3_size = sizeof(ofdm_3_mcs3) / sizeof(register_t);
const radio_settings_t ofdm_3_mcs3_cfg = {ofdm_3_mcs3, ofdm_3_mcs3_size};

const register_t ofdm_3_mcs4[] =
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x04},
  {RF09_RXDFE,     0x46},
  {RF09_AGCC,      0x11},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x05},
  {RF09_TXDFE,     0x66},
  {RF09_PAC,       0x64},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x16},
  {BBC0_OFDMC,     0x02},
  {BBC0_OFDMPHRTX, 0x04},
};
const uint16_t ofdm_3_mcs4_size = sizeof(ofdm_3_mcs4) / sizeof(register_t);
const radio_settings_t ofdm_3_mcs4_cfg = {ofdm_3_mcs4, ofdm_3_mcs4_size};

const register_t ofdm_3_mcs5[] =
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x04},
  {RF09_RXDFE,     0x46},
  {RF09_AGCC,      0x11},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x05},
  {RF09_TXDFE,     0x66},
  {RF09_PAC,       0x64},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x16},
  {BBC0_OFDMC,     0x02},
  {BBC0_OFDMPHRTX, 0x05},
};
const uint16_t ofdm_3_mcs5_size = sizeof(ofdm_3_mcs5) / sizeof(register_t);
const radio_settings_t ofdm_3_mcs5_cfg = {ofdm_3_mcs5, ofdm_3_mcs5_size};

const register_t ofdm_3_mcs6[] =
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x04},
  {RF09_RXDFE,     0x46},
  {RF09_AGCC,      0x11},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x05},
  {RF09_TXDFE,     0x66},
  {RF09_PAC,       0x64},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x16},
  {BBC0_OFDMC,     0x02},
  {BBC0_OFDMPHRTX, 0x06},
};
const uint16_t ofdm_3_mcs6_size = sizeof(ofdm_3_mcs6) / sizeof(register_t);
const radio_settings_t ofdm_3_mcs6_cfg = {ofdm_3_mcs6, ofdm_3_mcs6_size};

const register_t ofdm_4_mcs2[] =
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x12},
  {RF09_RXDFE,     0x26},
  {RF09_AGCC,      0x11},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x03},
  {RF09_TXDFE,     0x46},
  {RF09_PAC,       0x64},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x16},
  {BBC0_OFDMC,     0x03},
  {BBC0_OFDMPHRTX, 0x02},
};
const uint16_t ofdm_4_mcs2_size = sizeof(ofdm_4_mcs2) / sizeof(register_t);
const radio_settings_t ofdm_4_mcs2_cfg = {ofdm_4_mcs2, ofdm_4_mcs2_size};

const register_t ofdm_4_mcs3[] =
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x12},
  {RF09_RXDFE,     0x26},
  {RF09_AGCC,      0x11},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x03},
  {RF09_TXDFE,     0x46},
  {RF09_PAC,       0x64},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x16},
  {BBC0_OFDMC,     0x03},
  {BBC0_OFDMPHRTX, 0x03},
};
const uint16_t ofdm_4_mcs3_size = sizeof(ofdm_4_mcs3) / sizeof(register_t);
const radio_settings_t ofdm_4_mcs3_cfg = {ofdm_4_mcs3, ofdm_4_mcs3_size};

const register_t ofdm_4_mcs4[] =
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x12},
  {RF09_RXDFE,     0x26},
  {RF09_AGCC,      0x11},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x03},
  {RF09_TXDFE,     0x46},
  {RF09_PAC,       0x64},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x16},
  {BBC0_OFDMC,     0x03},
  {BBC0_OFDMPHRTX, 0x04},
};
const uint16_t ofdm_4_mcs4_size = sizeof(ofdm_4_mcs4) / sizeof(register_t);
const radio_settings_t ofdm_4_mcs4_cfg = {ofdm_4_mcs4, ofdm_4_mcs4_size};

const register_t ofdm_4_mcs5[] =
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x12},
  {RF09_RXDFE,     0x26},
  {RF09_AGCC,      0x11},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x03},
  {RF09_TXDFE,     0x46},
  {RF09_PAC,       0x64},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x16},
  {BBC0_OFDMC,     0x03},
  {BBC0_OFDMPHRTX, 0x05},
};
const uint16_t ofdm_4_mcs5_size = sizeof(ofdm_4_mcs5) / sizeof(register_t);
const radio_settings_t ofdm_4_mcs5_cfg = {ofdm_4_mcs5, ofdm_4_mcs5_size};

const register_t ofdm_4_mcs6[] =
{
  {RF09_CMD,       0x02},
  {RF09_IRQM,      0x00},
  {RF24_IRQM,      0x00},
  {RF09_RXBWC,     0x12},
  {RF09_RXDFE,     0x26},
  {RF09_AGCC,      0x11},
  {RF09_EDD,       0x7A},
  {RF09_TXCUTC,    0x03},
  {RF09_TXDFE,     0x46},
  {RF09_PAC,       0x64},
  {BBC0_IRQM,      0x1F},
  {BBC1_IRQM,      0x00},
  {BBC0_PC,        0x16},
  {BBC0_OFDMC,     0x03},
  {BBC0_OFDMPHRTX, 0x06},
};
const uint16_t ofdm_4_mcs6_size = sizeof(ofdm_4_mcs6) / sizeof(register_t);
const radio_settings_t ofdm_4_mcs6_cfg = {ofdm_4_mcs6, ofdm_4_mcs6_size};

const radio_settings_t radio_settings[CONFIG_ELEMENTS] =
{
  fsk_option1_cfg,
  fsk_option2_cfg,
  fsk_option3_cfg,
  oqpsk_rate1_cfg,
  oqpsk_rate2_cfg,
  oqpsk_rate3_cfg,
  oqpsk_rate4_cfg,
  oqpsk_250kbps_cfg,
  ofdm_1_mcs0_cfg,
  ofdm_1_mcs1_cfg,
  ofdm_1_mcs2_cfg,
  ofdm_1_mcs3_cfg,
  ofdm_2_mcs0_cfg,
  ofdm_2_mcs1_cfg,
  ofdm_2_mcs2_cfg,
  ofdm_2_mcs3_cfg,
  ofdm_2_mcs4_cfg,
  ofdm_2_mcs5_cfg,
  ofdm_3_mcs1_cfg,
  ofdm_3_mcs2_cfg,
  ofdm_3_mcs3_cfg,
  ofdm_3_mcs4_cfg,
  ofdm_3_mcs5_cfg,
  ofdm_3_mcs6_cfg,
  ofdm_4_mcs2_cfg,
  ofdm_4_mcs3_cfg,
  ofdm_4_mcs4_cfg,
  ofdm_4_mcs5_cfg,
  ofdm_4_mcs6_cfg
};

const frequency_settings_t frequency_settings[FREQUENCY_ELEMENTS] =
{
  {200,  863125, 0},
  {400,  863225, 0},
  {400,  863225, 0},
  {600,  868300, 0},
  {1200, 863625, 0},
  {800,  863425, 0},
  {400,  863225, 0},
  {200,  863125, 0},
};