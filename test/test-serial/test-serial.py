# -*- coding: utf-8 -*-
#!/usr/bin/python

'''
@file       test-serial.py
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
print(pwd)
sys.path.append(pwd)

import logging
import os
import signal
import struct
import sys
import time

import Serial

logger = logging.getLogger(__name__)

uart_name = 'COM27'
uart_speed = 576000

finished = False

def signal_handler(sig, frame):
    global finished
    finished = True

def program():
    global finished

    # Create and start Serial manager
    serial = Serial.Serial(name = uart_name, baudrate = uart_speed, timeout = 0.001)
    serial.start()

    while (not finished):
        #
        serial.transmit("Hello world!")

        # Try to receive a Serial message
        message, length = serial.receive(timeout = 100000)

        # If we received a message
        if (length > 0):
            logging.info("program: Received message with {} bytes.".format(length))
        else:
            logging.info("program: Receive timeout.")

    print(elapsed_time)
    print(serial.get_statistics())

    # Check for finished condition
    if (finished):
        serial.stop()

def main():
    # Set-up logging back-end
    logging.basicConfig(level=logging.ERROR,
                        handlers=[
                            logging.FileHandler("main.log"),
                            logging.StreamHandler()
                        ]
                       )
    # Set up SIGINT signal
    signal.signal(signal.SIGINT, signal_handler)
    
    # Execute program
    program()
    
if __name__ == "__main__":
    main()
