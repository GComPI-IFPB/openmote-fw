#!/usr/bin/python

'''
@file       at86rf215-ping-pong.py
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
import signal
import struct
import time
import random
import string

import enum

import Serial

finished = False

timeout = 0.001

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
    OQPSK_RATE_5 =  7
    OFDM_1_MCS_0 =  8 # BPSK, rate 1/2, 4x repetition, 100 kbps
    OFDM_1_MCS_1 =  9 # BPSK, rate 1/2, 2x repetition, 200 kbps
    OFDM_1_MCS_2 = 10 # QPSK, rate 1/2, 2x repetition, 400 kbps
    OFDM_1_MCS_3 = 11 # QPSK, rate 1/2, 0x repetition, 800 kbps
    OFDM_2_MCS_0 = 12 # BPSK, rate 1/2, 4x repetition,  50 kbps
    OFDM_2_MCS_1 = 13 # BPSK, rate 1/2, 2x repetition, 100 kbps
    OFDM_2_MCS_2 = 14 # QPSK, rate 1/2, 2x repetition, 200 kbps
    OFDM_2_MCS_3 = 15 # QPSK, rate 1/2, 0x repetition, 400 kbps
    OFDM_2_MCS_4 = 16 # QPSK, rate 3/4, 0x repetition, 600 kbps
    OFDM_2_MCS_5 = 17 # QAM,  rate 1/2, 0x repetition, 800 kbps
    OFDM_3_MCS_1 = 18 # BPSK, rate 1/2, 2x repetition,  50 kbps
    OFDM_3_MCS_2 = 19 # QPSK, rate 1/2, 2x repetition, 100 kbps
    OFDM_3_MCS_3 = 20 # QPSK, rate 1/2, 0x repetition, 200 kbps
    OFDM_3_MCS_4 = 21 # QPSK, rate 3/4, 0x repetition, 300 kbps
    OFDM_3_MCS_5 = 22 # QAM,  rate 1/2, 0x repetition, 400 kbps
    OFDM_3_MCS_6 = 23 # QAM,  rate 3/4, 0x repetition, 600 kbps
    OFDM_4_MCS_2 = 24 # QPSK, rate 1/2, 2x repetition,  50 kbps
    OFDM_4_MCS_3 = 25 # QPSK, rate 1/2, 0x repetition, 100 kbps
    OFDM_4_MCS_4 = 26 # QPSK, rate 3/4, 0x repetition, 150 kbps
    OFDM_4_MCS_5 = 27 # QAM,  rate 1/2, 0x repetition, 200 kbps
    OFDM_4_MCS_6 = 28 # QAM,  rate 3/4, 0x repetition, 300 kbps

class At86rf215_Freq(enum.Enum):
    FREQ_FSK_1   = 0
    FREQ_FSK_2   = 1
    FREQ_FSK_3   = 2
    FREQ_OQPSK_1 = 3
    FREQ_OFDM_1  = 4
    FREQ_OFDM_2  = 5
    FREQ_OFDM_3  = 6
    FREQ_OFDM_4  = 7

class At86rf215_Tx_Power(enum.Enum):
    MIN = 0
    MAX = 31

class Serial_Cmd(enum.Enum):
    ERROR   = 0
    ON      = 1
    OFF     = 2
    RST     = 3
    CFG     = 4
    RX      = 5
    TX      = 6
    TX_CONT = 7    

class Serial_Result(enum.Enum):
    ERROR   = 0
    SUCCESS = 1

def signal_handler(sig, frame):
    global finished
    finished = True

def parse_result(message):
    result = message[0]
    if (result == Serial_Result.SUCCESS.value):
        return True
    else:
        return False

def send_message(serial = None, timeout = 0.1, message = None):
    # Transmit message 
    serial.transmit(message)

    # Receive response
    message, length = serial.receive(timeout = timeout)
    if (length > 0):
        result = parse_result(message)
    else:
        result = False

    return result

def send_cmd_message(serial = None, timeout = 0.1, cmd = None, core = None):
    result = False
    message = [chr(b) for b in [cmd.value, core.value] if b != None]
    result = send_message(serial = serial, timeout = timeout, message = message)
    return result

def send_cfg_message(serial = None, timeout = 0.1, cmd = None, core = None, settings = None, frequency = None, power = None):
    result = False
    message = [chr(b) for b in [cmd.value, core.value, settings.value, frequency.value, power] if b != None]    
    result = send_message(serial = serial, timeout = timeout, message = message)
    return result

def send_set_message(serial = None, timeout = 0.1, cmd = None, core = None, enable = None):
    result = False
    message = [chr(b) for b in [cmd.value, core.value, enable] if b != None]
    result = send_message(serial = serial, timeout = timeout, message = message)
    return result

def transmit(port = None, baudrate = None):
    global finished

    # Create and start Serial manager
    serial = Serial.Serial(name = port, baudrate = baudrate, timeout = timeout)
    serial.start()
    
    result = send_cmd_message(serial = serial, timeout = 1.0, cmd = Serial_Cmd.ON, core = At86rf215_Core.RF09)
    if (result):
        result = send_cfg_message(serial = serial, timeout = 1.0, cmd = Serial_Cmd.CFG, core = At86rf215_Core.RF09, 
                                  settings = At86rf215_Cfg.OQPSK_RATE_5, frequency = At86rf215_Freq.FREQ_OQPSK_1, power = At86rf215_Tx_Power.MIN)

    # Repeat until finish condition
    while (not finished):
        result = send_cmd_message(serial = serial, timeout = 1.0, cmd = Serial_Cmd.TX, core = At86rf215_Core.RF09)
        if (not result):
            finished = True

        time.sleep(0.1)
        
    # Stop the serial port
    serial.stop()

def receive(port = None, baudrate = None):
    global finished

    # Create and start Serial manager
    serial = Serial.Serial(name = port, baudrate = baudrate, timeout = timeout)
    serial.start()
    
    result = send_cmd_message(serial = serial, timeout = 1.0, cmd = Serial_Cmd.ON, core = At86rf215_Core.RF09)
    if (result):
        result = send_cfg_message(serial = serial, timeout = 1.0, cmd = Serial_Cmd.CFG, core = At86rf215_Core.RF09, 
                                  settings = At86rf215_Cfg.OQPSK_RATE_5, frequency = At86rf215_Freq.FREQ_OQPSK_1, power = At86rf215_Tx_Power.MIN)
    
    received_packets = 0
    start_time = time.time()

    # Repeat until finish condition
    while (not finished):
        result = send_cmd_message(serial = serial, timeout = 1.0, cmd = Serial_Cmd.RX, core = At86rf215_Core.RF09)
        if (not result):
            finished = True
        else:
            received_packets += 1
        
        current_time = time.time()
        elapsed_time = 1000 * (current_time - start_time)
        if (elapsed_time > 1000):
            start_time = current_time
            print(received_packets)
            received_packets = 0

    # Stop the serial port
    serial.stop()

def interfere(port = None, baudrate = None, tx_power = None):
    global finished

    if (tx_power <= At86rf215_Tx_Power.MIN.value):
        tx_power = At86rf215_Tx_Power.MIN.value
    elif (tx_power >= At86rf215_Tx_Power.MAX.value):
        tx_power = At86rf215_Tx_Power.MAX.value

    # Create and start Serial manager
    serial = Serial.Serial(name = port, baudrate = baudrate, timeout = timeout)
    serial.start()
    
    result = send_cmd_message(serial = serial, timeout = 1.0, cmd = Serial_Cmd.ON, core = At86rf215_Core.RF09)
    if (result):
        result = send_cfg_message(serial = serial, timeout = 1.0, cmd = Serial_Cmd.CFG, core = At86rf215_Core.RF09, 
                                  settings = At86rf215_Cfg.OQPSK_RATE_5, frequency = At86rf215_Freq.FREQ_OQPSK_1, power = tx_power)
    
    if (result):
        result = send_set_message(serial = serial, timeout = 1.0, cmd = Serial_Cmd.TX_CONT, core = At86rf215_Core.RF09, enable = 1)

    # Repeat until finish condition
    while (not finished):
        time.sleep(0.1)

    result = send_cmd_message(serial = serial, timeout = 1.0, cmd = Serial_Cmd.RST, core = At86rf215_Core.RF09)

    # Stop the serial port
    serial.stop()

def main():
    # Set up SIGINT signal
    signal.signal(signal.SIGINT, signal_handler)

    # Create argument parser
    parser = argparse.ArgumentParser(description="")
    parser.add_argument("-m", "--mode", choices=["transmit", "receive", "interfere"], type=str, required=True)
    parser.add_argument("-p", "--port", type=str, required=True)
    parser.add_argument("-b", "--baudrate", type=int, default=115200)
    parser.add_argument("-tx", "--tx_power", type=int, default=0)

    # Parse arguments
    args = parser.parse_args()

    # Execute program
    if (args.mode == "transmit"):
        transmit(port = args.port, baudrate = args.baudrate)
    elif (args.mode == "receive"):
        receive(port = args.port, baudrate = args.baudrate)
    elif (args.mode == "interfere"):
        interfere(port = args.port, baudrate = args.baudrate, tx_power = args.tx_power)
    
if __name__ == "__main__":
    main()
