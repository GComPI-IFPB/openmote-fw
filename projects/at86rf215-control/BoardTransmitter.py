'''
@file       BoardTransmitter.py
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

class BoardTransmitter(Board.Board):
    def __init__(self, port = None, baudrate = None, timeout = 0.1):
        super().__init__(port = port, baudrate = baudrate, timeout = timeout)
        
        self.packet_count = 0
        self.packet_delay = 0
        self.tx_packets   = 0

    def run(self):
        # Start serial
        self.serial.start()

        # Repeat until finish condition
        while (not self.is_finish):
            # Wait to receive start command
            self.acquired = self.exp_run.acquire(False)
            
            # Wait to receive start command
            if (self.acquired):
                logger.info("BoardTransmitter::run")

                # Start the radio
                result = self.send_cmd_message(timeout = 1.0, core = self.core, cmd = At86rf215_Cmd.ON)
                if (result == False):
                    logger.error("BoardTransmitter::run Error starting the radio")
                    raise ValueError("BoardTransmitter::run Error starting the radio")

                # Configure the radio
                result = self.send_cfg_message(timeout = 1.0, core = self.core, cmd = At86rf215_Cmd.CFG,
                                               settings = self.settings, frequency = self.frequency,
                                               tx_length = self.tx_length, tx_power = self.tx_power)
                if (result == False):
                    logger.error("BoardTransmitter::run Error configuring the radio")
                    raise ValueError("BoardTransmitter::run Error configuring the radio")
                
                iterations = self.packet_count
                self.tx_packets = 0

                # Set end time
                start_time = time.time()
                
                while (not self.is_finish and not self.is_stop and iterations > 0):
                    # Try to send a packet
                    result = self.send_cmd_message(timeout = 1.0, core = At86rf215_Core.RF09, cmd = At86rf215_Cmd.TX)
                    
                    if (result):
                        logger.info("BoardTransmitter::run Transmitted packet to the radio")
                        self.tx_packets += 1
                    else:
                        logger.error("BoardTransmitter::run Error sending packet to the radio")
                        raise ValueError("BoardTransmitter::run Error sending packet to the radio")

                    # Decrement number of packets
                    iterations -= 1
                    
                    # Wait until next packet
                    if (iterations > 0):
                        time.sleep(self.packet_delay)

                    if (iterations == 0):
                        self.is_stop = True

                # Set end time
                end_time = time.time()

                # Calculate elapsed time
                self.elapsed_time = 1000 * (end_time - start_time)

                # Update the statistics
                self.stats["tx_time"] = self.elapsed_time
                self.stats["tx_packets"] = self.tx_packets

                # Reset the radio
                result = self.send_cmd_message(timeout = 1.0, core = self.core, cmd = At86rf215_Cmd.RST)

                # Notify we have finished
                self.exp_done.release()
            else:
                time.sleep(0.001)

        # Stop serial
        self.serial.stop()

    def configure(self, configuration = None):
        self.core         = configuration["core"]
        self.settings     = configuration["tx_settings"]
        self.frequency    = configuration["tx_frequency"]
        self.tx_power     = configuration["tx_power"]
        self.tx_length    = configuration["tx_length"]
        self.packet_count = configuration["packet_count"]
        self.packet_delay = configuration["packet_delay"]
