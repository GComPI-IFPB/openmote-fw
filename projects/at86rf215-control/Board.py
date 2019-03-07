'''
@file       Board.py
@author     Pere Tuset-Peiro  (peretuset@openmote.com)
@version    v0.1
@date       February, 2019
@brief      

@copyright  Copyright 2019, OpenMote Technologies, S.L.
            This file is licensed under the GNU General Public License v2.
'''

import threading

import Serial

class Board(threading.Thread):
    def __init__(self, port = None, baudrate = None, timeout = 0.1):
        self.port     = port
        self.baudrate = baudrate
        self.timeout  = timeout

        self.core      = None
        self.settings  = None
        self.tx_power  = None
        self.tx_length = None
        
        self.exp_run   = threading.Semaphore(0)
        self.exp_done  = threading.Semaphore(0)
        self.acquired  = False
        self.is_finish = False
        self.is_stop   = False

        self.stats     = None

        # Call constructor
        threading.Thread.__init__(self)

        # Create Serial manager
        self.serial = Serial.Serial(name = self.port, baudrate = self.baudrate, timeout = self.timeout)

    def configure(self, configuration = None):
        pass 

    def set_stats(self, stats = None):
        self.stats = stats

    def is_finished(self):
        return self.is_finish

    def is_stopped(self):
        return self.is_stop

    def wait(self):
        self.exp_done.acquire()

    def execute(self):
        self.is_finish = False
        self.is_stop = False
        self.exp_run.release()

    def stop(self):
        self.is_stop = True

    def finish(self):
        self.is_stop = True
        self.is_finish = True

    def send_cmd_message(self, timeout = 0.1, core = None, cmd = None):
        message = [chr(b) for b in [cmd.value, core.value] if b != None]
        result = self.__send_message(timeout = timeout, message = message)
        return result

    def send_cmd_message(self, timeout = 0.1, core = None, cmd = None, param = None):
        message = [chr(b) for b in [cmd.value, core.value, param] if b != None]
        result = self.__send_message(timeout = timeout, message = message)
        return result

    def send_cfg_message(self, timeout = 0.1, core = None, cmd = None, settings = None, frequency = None, tx_length = None, tx_power = None):
        message = [chr(b) for b in [cmd.value, core.value, settings.value, frequency.value, tx_length, tx_power] if b != None]    
        result = self.__send_message(timeout = timeout, message = message)
        return result

    def __send_message(self, timeout = 0.1, message = None):
        # Transmit message 
        self.serial.transmit(message)

        # Receive response
        message, length = self.serial.receive(timeout = timeout)
        if (length > 0):
            result = self.__parse_result(message)
        else:
            result = False

        return result

    def __parse_result(self, message):
        result = message[0]
        if (result == Serial_Result.SUCCESS.value):
            return True
        else:
            return False
