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

import serial
import signal
import struct
import os


finished = False

def signal_handler(sig, frame):
    global finished
    finished = True

def program():
    global finished
    ser = serial.Serial(port     = "COM49",
                        baudrate = 115200,
                        parity   = serial.PARITY_NONE,
                        stopbits = serial.STOPBITS_ONE,
                        bytesize = serial.EIGHTBITS,
                        xonxoff  = False,
                        rtscts   = False,
                        dsrdtr   = False)
    
    while(not finished):
        line = ser.readline()
        print(line.strip())
        ser.write(line)

def main():
     # Set-up logging back-end
    signal.signal(signal.SIGINT, signal_handler)

    program()
    
if __name__ == "__main__":
    main()
