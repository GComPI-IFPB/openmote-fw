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

def ping(port = None, baudrate = None, uncertain = None, length = None, sleep = None, duration = None, report = None):
    global finished

    # Calculate duration in miliseconds
    if (duration > 0):
        duration = 1000 * duration
    else:
        duration = -1

    # Generate random message
    if (uncertain == True):
        message     = "".join(random.choice(string.ascii_letters) for x in range(length))
    else:
        message     = "".join("a" for x in range(length))
    tx_length = len(message)

    # Create and start Serial manager
    serial = Serial.Serial(name = port, baudrate = baudrate, timeout = timeout)
    serial.start()

    if (uncertain == True):
        print("Starting ping at port {} with {} bauds, RANDOM message length {} bytes and {} sleep for {} ms.".format(port, baudrate, tx_length, sleep, duration))
    else:
        print("Starting ping at port {} with {} bauds, message length {} bytes and {} sleep for {} ms.".format(port, baudrate, tx_length, sleep, duration))

    # Prepare statistics
    bytes_sent       = 0
    bytes_received   = 0
    packets_sent     = 0
    packets_received = 0

    # Store start time
    start_time = time.time()
    total_time = 0

    # Transmit message 
    serial.transmit('0')

    time.sleep(0.1)

    # Transmit message 
    serial.transmit(message)

    # Account for sent packets and bytes
    bytes_sent += tx_length
    packets_sent += 1

    # Repeat until finish condition
    while (not finished):
        # Try to receive a Serial message
        received, rx_length = serial.receive(timeout = timeout)

        # If we received a message
        if (rx_length > 0):
            # Convert message to string
            received = [chr(c) for c in received]

            # Account for received packets and bytes
            bytes_received += rx_length
            packets_received += 1

            # Generate a new random message to be sent
            if (uncertain == True):
                message = "".join(random.choice(string.ascii_letters) for x in range(tx_length))

            # Transmit message 
            serial.transmit(message)

            # Account for sent messages
            bytes_sent += tx_length
            packets_sent += 1
        
        # If sleep
        if (sleep > 0.001):
            time.sleep(sleep)

        # Calculate elapsed time
        current_time = time.time()
        elapsed_time = 1000 * (current_time - start_time)

        # If elapsed time is above report
        if (elapsed_time > report):
            tx_bandwidth = bytes_sent * 8 / elapsed_time
            rx_bandwidth = bytes_received * 8 / elapsed_time
            print("Sent {} packets ({} bytes) and received {} packets ({} bytes) in the last {:.2f} ms ({:.2f}/{:.2f} kbps)".format(packets_sent, bytes_sent,
                                                                                                                                 packets_received, bytes_received,
                                                                                                                                 elapsed_time, tx_bandwidth, rx_bandwidth))

            # Restore start time
            start_time = current_time

            # Restore statistics
            packets_sent     = 0
            packets_received = 0
            bytes_sent       = 0
            bytes_received   = 0

            # Account for elapsed time
            total_time  += elapsed_time

        # If total time is above duration
        if (duration > 0 and total_time > duration):
            # Signal the script to finish
            finished = True
    
    # Stop the serial port
    serial.stop()

def pong(port = None, baudrate = None, length = None, report = None):
    global finished

    # Create and start Serial manager
    serial = Serial.Serial(name = port, baudrate = baudrate, timeout = timeout)
    serial.start()

    print("Starting pong at port {} with bauds {}.".format(port, baudrate))
    
    # Prepare statistics
    bytes_sent       = 0
    bytes_received   = 0
    packets_sent     = 0
    packets_received = 0

    # Store start time
    start_time = time.time()
    total_time = 0

    # Transmit message 
    serial.transmit('1')

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

            # Transmit message 
            serial.transmit(received)

            # Account for sent messages
            bytes_sent   += rx_length
            packets_sent += 1
        
        # Calculate elapsed time
        current_time = time.time()
        elapsed_time = 1000 * (current_time - start_time)

        # If elapsed time is above report time
        if (elapsed_time > report):
            tx_bandwidth = bytes_sent * 8 / elapsed_time
            rx_bandwidth = bytes_received * 8 / elapsed_time
            print("Received {} packets ({} bytes) and sent {} packets ({} bytes) in the last {:.2f} ms ({:.2f}/{:.2f} kbps)".format(packets_received, bytes_received,
                                                                                                                                    packets_sent, bytes_sent,
                                                                                                                                    elapsed_time, tx_bandwidth, rx_bandwidth))

            # Restore start time
            start_time = current_time

            # Restore statistics
            packets_sent     = 0
            packets_received = 0
            bytes_sent       = 0
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
    parser.add_argument("-m", "--mode", choices=["ping", "pong"], type=str, default="ping")
    parser.add_argument("-p", "--port", type=str, required=True)
    parser.add_argument("-b", "--baudrate", type=int, default=115200)
    parser.add_argument("-l", "--length", type=int, default=100)
    parser.add_argument("-d", "--duration", type=int, default=0)
    parser.add_argument("-r", "--report", type=int, default=1000)
    parser.add_argument("-s", "--sleep", type=float, default=0.1)
    parser.add_argument("-u", "--uncertain", type=lambda s: (str(s).lower() in ['true', 'on', 'yes', '1']), default=False)

    # Parse arguments
    args = parser.parse_args()

    # Execute program
    if (args.mode == "ping"):
        ping(port = args.port, baudrate=args.baudrate, uncertain=args.uncertain, length=args.length, sleep=args.sleep, duration=args.duration, report=args.report)
    elif (args.mode == "pong"):
        pong(port = args.port, baudrate=args.baudrate, length=args.length, report=args.report)
    
if __name__ == "__main__":
    main()
