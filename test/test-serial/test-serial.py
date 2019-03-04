#!/usr/bin/python

'''
@file       test-uart.py
@author     Pere Tuset-Peiro  (peretuset@openmote.com)
@version    v0.1
@date       May, 2015
@brief      

@copyright  Copyright 2015, OpenMote Technologies, S.L.
            This file is licensed under the GNU General Public License v2.
'''

import os
import sys
pwd = os.path.abspath(__file__)
pwd = os.path.dirname(os.path.dirname(os.path.dirname(pwd)))
pwd = os.path.join(pwd, 'python')
sys.path.append(pwd)

import signal
import struct
import time

import Serial

finished = False

def signal_handler(sig, frame):
    global finished
    finished = True

def program():
    global finished

    port        = "COM49"
    baudrate    = 2304000
    timeout     = 0.1
    count       = 101
    bits        = 10 # start + byte + stop
    message     = count * "0123456789"
    length      = len(message)
    report_time = 1000
    finish_time = 10000

    # Create and start Serial manager
    serial = Serial.Serial(name = port, baudrate = baudrate, timeout = timeout)
    serial.start()

    print("Starting test with {} bytes/message at {} kbps.".format(length, baudrate))
    
    # Prepare statistics
    total_sent     = 0
    total_received = 0

    # Store start time
    start_time = time.time()
    total_time = 0

    # Repeat until finish condition
    while (not finished):
        # Transmit message 
        serial.transmit(message)

        # Account for sent messages
        total_sent += 1

        # Try to receive a Serial message
        received, length = serial.receive(timeout = 0.1)

        # If we received a message
        if (length > 0):
            # Account for received messages
            total_received += 1
        
        # Calculate elapsed time
        current_time = time.time()
        elapsed_time = 1000 * (current_time - start_time)

        # If elapsed time is above report time
        if (elapsed_time > report_time):
            bandwidth = total_received * length * bits / elapsed_time
            print("{} out of {} messages received in the last {:.2f} ms ({:.2f} kbps)".format(total_received, total_sent, elapsed_time, bandwidth))

            # Restore start time
            start_time = current_time

            # Restore statistics
            total_sent = 0
            total_received = 0

            # Account for elapsed time
            total_time  += elapsed_time

        # If total time is above total time
        if (total_time > finish_time):
            # Signal the script to finish
            finished = True

    # Stop the serial port
    serial.stop()

def main():
    # Set up SIGINT signal
    signal.signal(signal.SIGINT, signal_handler)

    # Execute program
    program()
    
if __name__ == "__main__":
    main()
