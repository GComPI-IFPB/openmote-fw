'''
@file       At86rf215.py
@author     Pere Tuset-Peiro  (peretuset@openmote.com)
@version    v0.1
@date       February, 2019
@brief      

@copyright  Copyright 2019, OpenMote Technologies, S.L.
            This file is licensed under the GNU General Public License v2.
'''

import enum

class At86rf215_Core(enum.Enum):
    RF09  = 0
    RF24  = 1

class At86rf215_Cfg(enum.Enum):
    FSK_OPTION_1 =  0
    FSK_OPTION_2 =  1
    FSK_OPTION_3 =  2
    OQPSK_RATE_1 =  3
    OQPSK_RATE_2 =  4
    OQPSK_RATE_3 =  5
    OQPSK_RATE_4 =  6
    OQPSK_RATE_5 =  7 # OQPSK-DSSS, 250 kbps
    OFDM_1_MCS_0 =  8 # BPSK,   rate 1/2, 4x repetition,  100 kbps
    OFDM_1_MCS_1 =  9 # BPSK,   rate 1/2, 2x repetition,  200 kbps
    OFDM_1_MCS_2 = 10 # QPSK,   rate 1/2, 2x repetition,  400 kbps
    OFDM_1_MCS_3 = 11 # QPSK,   rate 1/2, 0x repetition,  800 kbps
    OFDM_1_MCS_4 = 12 # QPSK,   rate 3/4, 0x repetition, 1200 kbps
    OFDM_1_MCS_5 = 13 # 16-QAM, rate 1/2, 0x repetition, 1600 kbps
    OFDM_1_MCS_6 = 14 # 16-QAM, rate 3/4, 0x repetition, 2400 kbps
    OFDM_2_MCS_0 = 15 # BPSK,   rate 1/2, 4x repetition,   50 kbps
    OFDM_2_MCS_1 = 16 # BPSK,   rate 1/2, 2x repetition,  100 kbps
    OFDM_2_MCS_2 = 17 # QPSK,   rate 1/2, 2x repetition,  200 kbps
    OFDM_2_MCS_3 = 18 # QPSK,   rate 1/2, 0x repetition,  400 kbps
    OFDM_2_MCS_4 = 19 # QPSK,   rate 3/4, 0x repetition,  600 kbps
    OFDM_2_MCS_5 = 20 # 16-QAM, rate 1/2, 0x repetition,  800 kbps
    OFDM_2_MCS_6 = 21 # 16-QAM, rate 3/4, 0x repetition, 1200 kbps
    OFDM_3_MCS_1 = 22 # BPSK,   rate 1/2, 2x repetition,   50 kbps
    OFDM_3_MCS_2 = 23 # QPSK,   rate 1/2, 2x repetition,  100 kbps
    OFDM_3_MCS_3 = 24 # QPSK,   rate 1/2, 0x repetition,  200 kbps
    OFDM_3_MCS_4 = 25 # QPSK,   rate 3/4, 0x repetition,  300 kbps
    OFDM_3_MCS_5 = 26 # 16-QAM, rate 1/2, 0x repetition,  400 kbps
    OFDM_3_MCS_6 = 27 # 16-QAM, rate 3/4, 0x repetition,  600 kbps
    OFDM_4_MCS_2 = 28 # QPSK,   rate 1/2, 2x repetition,   50 kbps
    OFDM_4_MCS_3 = 29 # QPSK,   rate 1/2, 0x repetition,  100 kbps
    OFDM_4_MCS_4 = 30 # QPSK,   rate 3/4, 0x repetition,  150 kbps
    OFDM_4_MCS_5 = 31 # 16-QAM, rate 1/2, 0x repetition,  200 kbps
    OFDM_4_MCS_6 = 32 # 16-QAM, rate 3/4, 0x repetition,  300 kbps

class At86rf215_Freq(enum.Enum):
    FREQ_FSK_1   = 0
    FREQ_FSK_2   = 1
    FREQ_FSK_3   = 2
    FREQ_OQPSK_1 = 3 # 2000 KHz
    FREQ_OFDM_1  = 4 # 1200 kHz
    FREQ_OFDM_2  = 5 # 800 kHz
    FREQ_OFDM_3  = 6 # 400 kHz
    FREQ_OFDM_4  = 7 # 200 kHz

class At86rf215_Tx_Power(enum.Enum):
    MIN = 0
    MAX = 31

class At86rf215_Cmd(enum.Enum):
    ERROR     = 0
    ON        = 1
    OFF       = 2
    RST       = 3
    CFG       = 4
    RX        = 5
    TX        = 6
    TX_INTERF = 7    

class At86rf215_Result(enum.Enum):
    ERROR   = 0
    SUCCESS = 1

