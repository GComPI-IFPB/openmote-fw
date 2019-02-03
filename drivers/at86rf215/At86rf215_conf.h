/**
 * @file       At86rf215_conf.hpp
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2016
 * @brief
 *
 * @copyright  Copyright 2016, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef AT86RF215_CONF_HPP_
#define AT86RF215_CONF_HPP_

#include <stdint.h>

typedef struct
{
  uint16_t address;
  uint8_t data;
} register_t;

typedef struct
{
  const register_t* registers;
  uint16_t elements;
} radio_settings_t;

typedef struct
{
  uint16_t channel_spacing;
  uint32_t frequency0;
  uint16_t channel;
} frequency_settings_t;

/**
 * IEEE 802.15.4g MR-OFDM Reference
 * http://rfmw.em.keysight.com/wireless/helpfiles/n7610b/Content/Main/802.15.4g%20MR_OFDM.htm
 */
enum {
  CONFIG_FSK_OPTION_1 = 0,
  CONFIG_FSK_OPTION_2,
  CONFIG_FSK_OPTION_3,
  CONFIG_OQPSK_RATE_1,
  CONFIG_OQPSK_RATE_2,
  CONFIG_OQPSK_RATE_3,
  CONFIG_OQPSK_RATE_4,
  CONFIG_OQPSK_RATE_5,
  CONFIG_OFDM_1_MCS_0, /* BPSK, rate 1/2, 4x repetition, 100 kbps */
  CONFIG_OFDM_1_MCS_1, /* BPSK, rate 1/2, 2x repetition, 200 kbps */
  CONFIG_OFDM_1_MCS_2, /* QPSK, rate 1/2, 2x repetition, 400 kbps */
  CONFIG_OFDM_1_MCS_3, /* QPSK, rate 1/2, 0x repetition, 400 kbps */
  CONFIG_OFDM_2_MCS_0, /* BPSK, rate 1/2, 4x repetition,  50 kbps */
  CONFIG_OFDM_2_MCS_1, /* BPSK, rate 1/2, 2x repetition, 100 kbps */
  CONFIG_OFDM_2_MCS_2, /* QPSK, rate 1/2, 2x repetition, 200 kbps */
  CONFIG_OFDM_2_MCS_3, /* QPSK, rate 1/2, 0x repetition, 400 kbps */
  CONFIG_OFDM_2_MCS_4, /* QPSK, rate 3/4, 0x repetition, 600 kbps */
  CONFIG_OFDM_2_MCS_5, /* QAM,  rate 1/2, 0x repetition, 800 kbps */
  CONFIG_OFDM_3_MCS_1, /* BPSK, rate 1/2, 2x repetition,  50 kbps */
  CONFIG_OFDM_3_MCS_2, /* QPSK, rate 1/2, 2x repetition, 100 kbps */
  CONFIG_OFDM_3_MCS_3, /* QPSK, rate 1/2, 0x repetition, 200 kbps */
  CONFIG_OFDM_3_MCS_4, /* QPSK, rate 3/4, 0x repetition, 300 kbps */
  CONFIG_OFDM_3_MCS_5, /* QAM,  rate 1/2, 0x repetition, 400 kbps */
  CONFIG_OFDM_3_MCS_6, /* QAM,  rate 3/4, 0x repetition, 600 kbps */
  CONFIG_OFDM_4_MCS_2, /* QPSK, rate 1/2, 2x repetition,  50 kbps */
  CONFIG_OFDM_4_MCS_3, /* QPSK, rate 1/2, 0x repetition, 100 kbps */
  CONFIG_OFDM_4_MCS_4, /* QPSK, rate 3/4, 0x repetition, 150 kbps */
  CONFIG_OFDM_4_MCS_5, /* QAM,  rate 1/2, 0x repetition, 200 kbps */
  CONFIG_OFDM_4_MCS_6, /* QAM,  rate 3/4, 0x repetition, 300 kbps */
  CONFIG_ELEMENTS
};

enum {
  FREQUENCY_FSK_1 = 0, /* FSK Mode 1,   200 kHz */
  FREQUENCY_FSK_2,     /* FSK Mode 2,   400 kHz */
  FREQUENCY_FSK_3,     /* FSK Mode 3,   400 kHz */
  FREQUENCY_OQPSK_1,   /* OQPSK,        600 kHz */
  FREQUENCY_OFDM_1,    /* OFDM Mode 1, 1200 kHz */
  FREQUENCY_OFDM_2,    /* OFDM Mode 2,  800 kHz */
  FREQUENCY_OFDM_3,    /* OFDM Mode 3,  400 kHz */
  FREQUENCY_OFDM_4,    /* OFDM Mode 4,  200 kHz */
  FREQUENCY_ELEMENTS
};

extern const radio_settings_t radio_settings[CONFIG_ELEMENTS];
extern const frequency_settings_t frequency_settings[FREQUENCY_ELEMENTS];

#endif /* AT86RF215_CONF_HPP_ */
