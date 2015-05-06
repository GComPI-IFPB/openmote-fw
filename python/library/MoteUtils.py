# -*- encoding: utf-8 -*-

'''
@file       MoteUtils.py
@author     Pere Tuset-Peiro  (peretuset@openmote.com)
@version    v0.1
@date       May, 2014
@brief      
'''

import sys as sys
import glob as glob

PLATFORM_WINDOWS = 'WINDOWS'
PLATFORM_MAC = 'MACINTOSH'
PLATFORM_LINUX = 'LINUX'

BAUD_RATE = 115200

def find_serial_ports():
    # Enumerate the serial ports
    serial_ports = None
    while(not serial_ports):
        serial_ports = enumerate_serial_ports()
        if (not serial_ports):
            out = raw_input("No serial port found! Retry (Y/N)? ")
            if (out == 'N' or out == 'n'):
                return
            else:
                continue
    return serial_ports

def enumerate_serial_ports():
    serial_ports = []
    
    # Find the current operating system
    platform = _find_operating_system()
    
    # Find serial ports based on current operating system
    if (platform == PLATFORM_WINDOWS):
        import _winreg as winreg
        path = 'HARDWARE\\DEVICEMAP\\SERIALCOMM'
        key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, path)
        for i in range(winreg.QueryInfoKey(key)[1]):
            try:
                val = winreg.EnumValue(key,i)
            except:
                pass
            else:
                if   val[0].find('VCP') > -1:
                    serial_ports.append((str(val[1]),BAUD_RATE))
    elif (platform == PLATFORM_MAC):
        serial_ports = [(s, BAUD_RATE) for s in glob.glob('/dev/tty.usbserial*')]
    elif(platform == PLATFORM_LINUX):
        serial_ports = [(s, BAUD_RATE) for s in glob.glob('/dev/ttyUSB*')]
                         
    return serial_ports

def _find_operating_system():
    import platform
    platform = platform.system()
    
    if (platform == 'Windows'):
        return PLATFORM_WINDOWS
    elif (platform == 'Darwin'):
        return PLATFORM_MAC
    elif (platform == 'Linux'):
        return PLATFORM_LINUX
    else:
        error = "Error: Unsuported platform"
        print error
