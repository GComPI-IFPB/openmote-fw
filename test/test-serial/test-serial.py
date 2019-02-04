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
import serial
import signal
import struct
import sys

def signal_handler(sig, frame):
        print('You pressed Ctrl+C!')
        sys.exit(0)

def program():
    ser = serial.Serial(port     = 'COM49',
                        baudrate = 4000000,
                        parity   = serial.PARITY_NONE,
                        stopbits = serial.STOPBITS_ONE,
                        bytesize = serial.EIGHTBITS,
                        xonxoff  = False,
                        rtscts   = False,
                        dsrdtr   = False, 
                        timeout  = 0.1)
    
    while (True):
        buff = ser.read(1)
        if (len(buff) > 0):
            data = struct.unpack("1b", buff)[0]
            print(hex(data))
            ser.write(buff)

def main():
    signal.signal(signal.SIGINT, signal_handler)
    program()
    
if __name__ == "__main__":
    main()
