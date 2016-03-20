'''
@file       ieee802154-sniffer.py
@author     Pere Tuset-Peiro  (peretuset@openmote.com)
@version    v0.1
@date       May, 2015
@brief      

@copyright  Copyright 2015, OpenMote Technologies, S.L.
            This file is licensed under the GNU General Public License v2.
'''

#!/usr/bin/python

import getopt
import logging
import logging.config
import os
import serial
import subprocess
import sys
import time

# Define the supported boards for bootloading
bsl_boards = ["openbase", "openrpi", "openbbb", "openusb"]

# Define the relative path to the project home
openmote_bsl_path = "../../tools/openmote-bsl"

# Define logging file name, path and configuration
log_name = "openmote-bsl.cfg"
log_path = os.path.join(openmote_bsl_path, log_name)
logging.config.fileConfig(log_path, disable_existing_loggers = False)
logger = logging.getLogger(__name__)

# Define cc2538_bsl name and path
cc2538_bsl_name = "cc2538-bsl.py"
cc2538_bsl_path = "cc2538-bsl"
cc2538_bsl_path = os.path.join(openmote_bsl_path, cc2538_bsl_path, cc2538_bsl_name)
cc2538_bsl_bin  = os.path.abspath(cc2538_bsl_path)

class OpenBBB():
    try:
        import Adafruit_BBIO.GPIO as gpio
    except:
        logger.error("Adafruit_BBIO.GPIO module not installed.")
        
    cc2538_bsl_params = ['-p', '/dev/ttyO1', '-e', '-w', '-b', '115200']
    
    BSL   = "P9_25"
    RESET = "P9_23"
    
    def __init__(self):
        self.gpio.setup(self.BSL,   self.gpio.OUT)
        self.gpio.setup(self.RESET, self.gpio.OUT)
        
    def run(self, bsl_file = None):
        self.bsl_start()
        self.bsl_flash(bsl_file = bsl_file)
        self.bsl_stop()
        
    def bsl_start(self):
        self.gpio.output(self.RESET, self.gpio.HIGH)
        self.gpio.output(self.BSL,   self.gpio.HIGH)
        self.gpio.output(self.RESET, self.gpio.LOW)
        self.gpio.output(self.BSL,   self.gpio.LOW)
        self.gpio.output(self.RESET, self.gpio.HIGH)
            
    def bsl_flash(self, bsl_file = None):
        script = [cc2538_bsl_bin] + self.cc2538_bsl_params + [bsl_file]
        process = subprocess.call(script, shell=False)

    def bsl_stop(self):
        self.gpio.output(self.RESET, self.gpio.LOW)
        self.gpio.output(self.BSL,   self.gpio.HIGH)
        self.gpio.output(self.RESET, self.gpio.HIGH)            

class OpenBase():
    cc2538_bsl_params = ['-e', '-w', '-b', '115200']
    
    def __init__(self):
        pass
    
    def run(self, bsl_file = None):
        self.bsl_start()
        self.bsl_flash(bsl_file = bsl_file)
        self.bsl_stop()
        
    def bsl_start(self):
        pass
            
    def bsl_flash(self, bsl_file = None):
        script = [cc2538_bsl_bin] + self.cc2538_bsl_params + [bsl_file]
        process = subprocess.call(script, shell=False)

    def bsl_stop(self):
        pass

class OpenRPi():   
    try:
        import RPi.GPIO as gpio
    except:
        logger.error("RPi.GPIO module not installed.")
    
    cc2538_bsl_params = ['-e', '-w', '-b', '115200']
    
    BSL   = 23
    RESET = 24

    HIGH = True
    LOW  = False
    
    def __init__(self):
        self.gpio.setwarnings(False)
        self.gpio.setmode(self.gpio.BCM)
        self.gpio.setup(self.BSL,   self.gpio.OUT)
        self.gpio.setup(self.RESET, self.gpio.OUT)
    
    def run(self):
        self.bsl_start()
        self.bsl_flash()
        self.bsl_stop()
        
    def bsl_start(self):
        self.gpio.output(self.RESET, self.HIGH)
        self.gpio.output(self.BSL,   self.HIGH)
        self.gpio.output(self.RESET, self.LOW)
        self.gpio.output(self.BSL,   self.LOW)
        self.gpio.output(self.RESET, self.HIGH)
            
    def bsl_flash(self, bsl_file = None):
        script = [os.path.join(cc2538_bsl_path, cc2538_bsl_name), self.cc2538_bsl_params]
        process = subprocess.call(script, shell=False)

    def bsl_stop(self):
        self.gpio.output(self.RESET, self.LOW)
        self.gpio.output(self.BSL,   self.HIGH)
        self.gpio.output(self.RESET, self.HIGH)
            
class OpenUsb():
    cc2538_bsl_params = ['-e', '-w', '-b', '115200', '--bsl']
    
    High = True
    Low  = False

    def __init__(self):
        pass
    
    def run(self, bsl_file = None):
        self.bsl_start()
        self.bsl_flash(bsl_file = bsl_file)
        self.bsl_stop()
        
    def bsl_start(self):
        pass
            
    def bsl_flash(self, bsl_file = None):
        script = [cc2538_bsl_bin] + self.cc2538_bsl_params + [bsl_file]
        process = subprocess.call(script, shell=False)

    def bsl_stop(self):
        pass


def parse_config(config = None, arguments = None):
    assert config    != None, logger.error("Config not defined.")
    assert arguments != None, logger.error("Arguments not defined.")

    try:
        opts, args = getopt.getopt(arguments, ":v", ["board="])
    except getopt.GetoptError as error:
        print(str(error))
        sys.exit(1)

    # Parse command line options
    for option, value in opts:
        if option == "--board":
            if value in bsl_boards:
                config['bsl_board'] = value
        else:
            assert False, logger.error("Unhandled options while parsing the command line arguments.")
       
    return config

def main(argv):
    default_config = {
        'bsl_board' : 'none'
    }
    
    # Parse the command line arguments
    bsl_file   = sys.argv[1]
    other_args = sys.argv[2:]
    config     = parse_config(default_config, other_args)
    
    if config['bsl_board'] == "openbase":
        bsl = OpenBase()
    elif config['bsl_board'] == "openrpi":
        bsl = OpenRPi()
    elif config['bsl_board'] == "openusb":
        bsl = OpenUsb()
    elif config['bsl_board'] == "openbbb":
        bsl = OpenBBB()
    else:
        assert False, logger.error("Unknown board, please check the bsl_boards variable.")
    
    bsl.run(bsl_file = bsl_file)

if __name__ == "__main__":
    main(sys.argv)
