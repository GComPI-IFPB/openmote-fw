#!/usr/bin/python

'''
@file       at86rf215-control.py
@author     Pere Tuset-Peiro  (peretuset@openmote.com)
@version    v0.1
@date       February, 2019
@brief      

@copyright  Copyright 2019, OpenMote Technologies, S.L.
            This file is licensed under the GNU General Public License v2.
'''

import os
import sys
pwd = os.path.abspath(__file__)
pwd = os.path.dirname(os.path.dirname(os.path.dirname(pwd)))
pwd = os.path.join(pwd, 'python')
sys.path.append(pwd)

import argparse
import csv
import enum
import logging
import random
import signal
import struct
import string
import time

from At86rf215 import *
from BoardTransmitter import *
from BoardReceiver import *
from BoardInterferer import *
from ExperimentManager import *

import Serial

logger = logging.getLogger(__name__)

finished = False

def signal_handler(sig, frame):
    global finished
    finished = True

def main():
    global finished 

    # Set-up logging back-end
    logging.basicConfig(level=logging.ERROR)

    interfere_uart = "COM39"
    baudrate       = 1267200

    # Create the experiment configuration
    configuration = {"core": At86rf215_Core.RF09,
                     "tx_settings": [At86rf215_Cfg.OFDM_1_MCS_1, At86rf215_Cfg.OFDM_2_MCS_2, At86rf215_Cfg.OFDM_3_MCS_3, At86rf215_Cfg.OFDM_4_MCS_5, At86rf215_Cfg.OQPSK_RATE_5],
                     "tx_frequency": At86rf215_Freq.FREQ_OQPSK_1, 
                     "tx_power": 12,
                     "tx_length": [20, 120],
                     "ix_settings": At86rf215_Cfg.OFDM_1_MCS_6,
                     "ix_active": True,
                     "ix_frequency": At86rf215_Freq.FREQ_OQPSK_1, 
                     "ix_power": 20,
                     "ix_length": 123,
                     "packet_count": 100,
                     "packet_delay": 0.005,
                     "duration_ms": 30000}

    # Set up SIGINT signal
    signal.signal(signal.SIGINT, signal_handler)

    # Create the transmit, interfere and receive objects
    interfere = BoardInterferer(port = interfere_uart, baudrate = baudrate)

    interfere.start()
    interfere.configure(configuration = configuration)
    interfere.execute()

    # Wait until experiment finishes
    while (not finished):
        time.sleep(0.1)

    interfere.finish()

if __name__ == "__main__":
    main()