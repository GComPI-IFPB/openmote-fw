'''
@file       BoardInterferer.py
@author     Pere Tuset-Peiro  (peretuset@openmote.com)
@version    v0.1
@date       February, 2019
@brief      

@copyright  Copyright 2019, OpenMote Technologies, S.L.
            This file is licensed under the GNU General Public License v2.
'''

import logging
import time

import Board

from At86rf215 import *

logger = logging.getLogger(__name__)

class BoardInterferer(Board.Board):
    def __init__(self, port = None, baudrate = None, timeout = 0.1):
        super().__init__(port = port, baudrate = baudrate, timeout = timeout)

    def run(self):
        # Start serial
        self.serial.start()

        # Repeat until finish condition
        while (not self.is_finish):
            # Wait to receive start command
            self.acquired = self.exp_run.acquire(False)
            if (self.acquired and self.active):
                # Start the radio
                result = self.send_cmd_message(timeout = 1.0, core = self.core, cmd = At86rf215_Cmd.ON)
                if (result == False):
                    logger.error("BoardInterferer::run Error starting the radio")
                    raise ValueError("BoardInterferer::run Error starting the radio")

                # Configure the radio
                result = self.send_cfg_message(timeout = 1.0, core = self.core, cmd = At86rf215_Cmd.CFG,
                                               settings = self.settings, frequency = self.frequency,
                                               tx_length = self.tx_length, tx_power = self.tx_power)
                if (result == False):
                    logger.error("BoardInterferer::run Error configuring the radio")
                    raise ValueError("BoardInterferer::run Error configuring the radio")
                
                # Start continuous transmission
                result = self.send_cmd_message(timeout = 1.0, core = self.core, cmd = At86rf215_Cmd.TX_INTERF, param = 1)
                if (result == False):
                    logger.error("BoardInterferer::run Error starting continuous transmission")
                    raise ValueError("BoardInterferer::run Error starting continuous transmission")
                    
                # Repeat until finish condition
                while (not self.is_finish and not self.is_stop):
                    time.sleep(0.001)

                # Reset the radio
                result = self.send_cmd_message(timeout = 1.0, core = self.core, cmd = At86rf215_Cmd.RST)

                # Notify we have finished
                self.exp_done.release()

            else:
                time.sleep(0.001)

        # Stop serial
        self.serial.stop()

    def configure(self, configuration = None):
        self.core      = configuration["core"]
        self.active    = configuration["ix_active"]
        self.settings  = configuration["ix_settings"]
        self.frequency = configuration["ix_frequency"]
        self.tx_power  = configuration["ix_power"]
        self.tx_length = configuration["ix_length"]
