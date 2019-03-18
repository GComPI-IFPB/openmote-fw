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

import Serial

finished = False

timeout = 0.1

def signal_handler(sig, frame):
    global finished
    finished = True

def program(port = None, baudrate = None):
    global finished

    # Create and start Serial manager
    serial = Serial.Serial(name = port, baudrate = baudrate, timeout = timeout)
    serial.start()

    print("Starting program at port {} with bauds {}.".format(port, baudrate))

    # Repeat until finish condition
    while (not finished):
        # Try to receive a Serial message
        message, length = serial.receive(timeout = timeout)

        # If we received a message
        if (length > 0):
            message = bytearray(bytes(message))
            eui48, counter, t, h, p, l, rssi = struct.unpack('>6sIhhhhb', message)
            t = t/10.0
            h = h/10.0
            p = p/10.0
            l = l/10.0
            print("Counter={}, Temperature={}, Humidity={}, Pressure={}, RSSI={}".format(counter, t, h, p, rssi))
            

    # Stop the serial port
    serial.stop()

def main():
    # Set up SIGINT signal
    signal.signal(signal.SIGINT, signal_handler)

    # Create argument parser
    parser = argparse.ArgumentParser(description="")
    parser.add_argument("-p", "--port", type=str, required=True)
    parser.add_argument("-b", "--baudrate", type=int, default=115200)

    # Parse arguments
    args = parser.parse_args()

    # Execute program
    program(port = args.port, baudrate=args.baudrate)
    
if __name__ == "__main__":
    main()
