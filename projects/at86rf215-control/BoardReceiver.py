'''
@file       BoardReceiver.py
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

class BoardReceiver(Board.Board):
    def __init__(self, port = None, baudrate = None, timeout = 0.1):
        super().__init__(port = port, baudrate = baudrate, timeout = timeout)
        
        self.elapsed_time = 0
        self.rx_packets   = 0
        self.rx_timeout   = 0

    def run (self):
        # Start serial
        self.serial.start()

        # Repeat until finish condition
        while (not self.is_finish):
            # Wait to receive start command
            self.acquired = self.exp_run.acquire(False)

            # Wait to receive start command
            if (self.acquired):
                logger.info("BoardReceiver::run")

                # Start the radio
                result = self.send_cmd_message(timeout = 1.0, core = self.core, cmd = At86rf215_Cmd.ON)
                if (result == False):
                    logger.error("BoardReceiver::run Error starting the radio")
                    raise ValueError("BoardReceiver::run Error starting the radio")
                
                # Configure the radio
                result = self.send_cfg_message(timeout = 1.0, core = self.core, cmd = At86rf215_Cmd.CFG,
                                               settings = self.settings, frequency = self.frequency,
                                               tx_length = self.tx_length, tx_power = 0)
                if (result == False):
                    logger.error("BoardReceiver::run Error configuring the radio")
                    raise ValueError("BoardReceiver::run Error configuring the radio")
                
                self.elapsed_time = 0
                self.rx_packets = 0
                self.rx_timeout = 0

                # Set start time
                start_time = time.time()

                while (not self.is_finish and not self.is_stop):
                    # Try to receive a packet
                    result = self.send_cmd_message(timeout = 1.0, core = self.core, cmd = At86rf215_Cmd.RX, param = 25)
                    
                    if (result):
                        logger.info("BoardReceiver::run Received packet from the radio")
                        self.rx_packets += 1
                    else:
                        self.rx_timeout += 1
                        logger.info("BoardReceiver::run Timeout receiving packet from the radio")

                # Set end time
                end_time = time.time()

                # Calculate elapsed time
                self.elapsed_time = 1000 * (end_time - start_time)

                # Update the statistics
                self.stats["rx_time"] = self.elapsed_time
                self.stats["rx_packets"] = self.rx_packets
                self.stats["rx_timeout"] = self.rx_timeout

                # Reset the radio
                result = self.send_cmd_message(timeout = 1.0, core = self.core, cmd = At86rf215_Cmd.RST)

                # Notify we have finished
                self.exp_done.release()
            else:
                time.sleep(0.001)

        # Stop serial
        self.serial.stop()

    def configure(self, configuration = None):
        self.core = configuration["core"]
        self.settings = configuration["tx_settings"]
        self.frequency = configuration["tx_frequency"]
        self.tx_length = configuration["tx_length"]
