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
import struct
import os

def program():
    ser = serial.Serial(port     = '/dev/ttyUSB0',
                        baudrate = 115200,
                        parity   = serial.PARITY_NONE,
                        stopbits = serial.STOPBITS_ONE,
                        bytesize = serial.EIGHTBITS,
                        xonxoff  = False,
                        rtscts   = False,
                        dsrdtr   = False)
    
    while(True):
        line = ser.readline()
        print(line.strip())

def main():
    program()
    
if __name__ == "__main__":
    main()
