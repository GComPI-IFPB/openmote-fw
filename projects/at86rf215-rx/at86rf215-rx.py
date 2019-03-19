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

timeout = 0.001

def signal_handler(sig, frame):
    global finished
    finished = True

def program(port = None, baudrate = None, report = None):
    global finished

    # Create and start Serial manager
    serial = Serial.Serial(name = port, baudrate = baudrate, timeout = timeout)
    serial.start()

    print("Starting program at port {} with bauds {}.".format(port, baudrate))
    
    # Prepare statistics
    bytes_received   = 0
    packets_received = 0

    # Store start time
    start_time = time.time()
    total_time = 0

    # Repeat until finish condition
    while (not finished):
        # Try to receive a Serial message
        received, rx_length = serial.receive(timeout = timeout)

        # If we received a message
        if (rx_length > 0):
            # Convert message to string
            received  = [chr(c) for c in received]
            rx_length = len(received)

            # Account for received messages
            bytes_received   += rx_length
            packets_received += 1
        
        # Calculate elapsed time
        current_time = time.time()
        elapsed_time = 1000 * (current_time - start_time)

        # If elapsed time is above report time
        if (elapsed_time > report):
            print("Received {} packets in the last {:.2f} ms".format(packets_received, elapsed_time))

            # Restore start time
            start_time = current_time

            # Restore statistics
            packets_received = 0
            bytes_received   = 0

            # Account for elapsed
            total_time  += elapsed_time
    
    # Stop the serial port
    serial.stop()

def main():
    # Set up SIGINT signal
    signal.signal(signal.SIGINT, signal_handler)

    # Create argument parser
    parser = argparse.ArgumentParser(description="")
    parser.add_argument("-p", "--port", type=str, required=True)
    parser.add_argument("-b", "--baudrate", type=int, default=115200)
    parser.add_argument("-r", "--report", type=int, default=1000)

    # Parse arguments
    args = parser.parse_args()

    # Execute program
    program(port = args.port, baudrate=args.baudrate, report=args.report)
    
if __name__ == "__main__":
    main()
