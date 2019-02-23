#!/usr/bin/python

'''
@file       at86rf215-ping-pong.py
@author     Pere Tuset-Peiro  (peretuset@openmote.com)
@version    v0.1
@date       February, 2019
@brief      

@copyright  Copyright 2019, OpenMote Technologies, S.L.
            This file is licensed under the GNU General Public License v2.
'''

import os
import sys
pwd = os.path.abspath(__file__)
pwd = os.path.dirname(os.path.dirname(os.path.dirname(pwd)))
pwd = os.path.join(pwd, 'python')
sys.path.append(pwd)

import argparse
import csv
import enum
import logging
import random
import signal
import struct
import string
import threading
import time

import Serial

logger = logging.getLogger(__name__)

class At86rf215_Core(enum.Enum):
    RF09  = 0
    RF24  = 1

class At86rf215_Cfg(enum.Enum):
    FSK_OPTION_1 =  0
    FSK_OPTION_2 =  1
    FSK_OPTION_3 =  2
    OQPSK_RATE_1 =  3
    OQPSK_RATE_2 =  4
    OQPSK_RATE_3 =  5
    OQPSK_RATE_4 =  6
    OQPSK_RATE_5 =  7 # OQPSK, DSSS, 250 kbps
    OFDM_1_MCS_0 =  8 # BPSK, rate 1/2, 4x repetition, 100 kbps
    OFDM_1_MCS_1 =  9 # BPSK, rate 1/2, 2x repetition, 200 kbps
    OFDM_1_MCS_2 = 10 # QPSK, rate 1/2, 2x repetition, 400 kbps
    OFDM_1_MCS_3 = 11 # QPSK, rate 1/2, 0x repetition, 800 kbps
    OFDM_2_MCS_0 = 12 # BPSK, rate 1/2, 4x repetition,  50 kbps
    OFDM_2_MCS_1 = 13 # BPSK, rate 1/2, 2x repetition, 100 kbps
    OFDM_2_MCS_2 = 14 # QPSK, rate 1/2, 2x repetition, 200 kbps
    OFDM_2_MCS_3 = 15 # QPSK, rate 1/2, 0x repetition, 400 kbps
    OFDM_2_MCS_4 = 16 # QPSK, rate 3/4, 0x repetition, 600 kbps
    OFDM_2_MCS_5 = 17 # QAM,  rate 1/2, 0x repetition, 800 kbps
    OFDM_3_MCS_1 = 18 # BPSK, rate 1/2, 2x repetition,  50 kbps
    OFDM_3_MCS_2 = 19 # QPSK, rate 1/2, 2x repetition, 100 kbps
    OFDM_3_MCS_3 = 20 # QPSK, rate 1/2, 0x repetition, 200 kbps
    OFDM_3_MCS_4 = 21 # QPSK, rate 3/4, 0x repetition, 300 kbps
    OFDM_3_MCS_5 = 22 # QAM,  rate 1/2, 0x repetition, 400 kbps
    OFDM_3_MCS_6 = 23 # QAM,  rate 3/4, 0x repetition, 600 kbps
    OFDM_4_MCS_2 = 24 # QPSK, rate 1/2, 2x repetition,  50 kbps
    OFDM_4_MCS_3 = 25 # QPSK, rate 1/2, 0x repetition, 100 kbps
    OFDM_4_MCS_4 = 26 # QPSK, rate 3/4, 0x repetition, 150 kbps
    OFDM_4_MCS_5 = 27 # QAM,  rate 1/2, 0x repetition, 200 kbps
    OFDM_4_MCS_6 = 28 # QAM,  rate 3/4, 0x repetition, 300 kbps

class At86rf215_Freq(enum.Enum):
    FREQ_FSK_1   = 0
    FREQ_FSK_2   = 1
    FREQ_FSK_3   = 2
    FREQ_OQPSK_1 = 3 # 2000 KHz
    FREQ_OFDM_1  = 4 # 1200 kHz
    FREQ_OFDM_2  = 5 # 800 kHz
    FREQ_OFDM_3  = 6 # 400 kHz
    FREQ_OFDM_4  = 7 # 200 kHz

class At86rf215_Tx_Power(enum.Enum):
    MIN = 0
    MAX = 31

class Serial_Cmd(enum.Enum):
    ERROR     = 0
    ON        = 1
    OFF       = 2
    RST       = 3
    CFG       = 4
    RX        = 5
    TX        = 6
    TX_INTERF = 7    

class Serial_Result(enum.Enum):
    ERROR   = 0
    SUCCESS = 1

class ExperimentManager(threading.Thread):
    def __init__(self, transmit = None, interfere = None, receive = None, configuration = None):
        self.transmit      = transmit
        self.interfere     = interfere
        self.receive       = receive
        self.configuration = configuration
        
        self.duration_ms = configuration["duration_ms"]
        self.elapsed_ms  = 0 
        
        self.stats = {}

        self.is_finish  = False
        
        # Call constructor
        threading.Thread.__init__(self)

    def run(self):
        # Start transmit, receive and interfere threads
        self.transmit.start()
        self.receive.start()
        self.interfere.start()

        tx_settings_tests = self.configuration["tx_settings"]
        ix_settings_tests = self.configuration["ix_settings"]
        length_tests      = self.configuration["tx_length"]
        power_tests       = self.configuration["ix_power"]

        # Interfence settings = {}
        for ix_settings in ix_settings_tests:
            
            # Packet length = {20, 120}
            for packet_length in length_tests:
                # Generate file name
                filename = ix_settings.name + "_" + str(packet_length) + "bytes" + ".csv"
                
                # Open CSV file and writer
                csv_file = open(filename, 'w', newline='')
                csv_writer = csv.writer(csv_file, dialect='excel')
                
                # Write CSV header to file
                csv_header = ["SNR"] + [str(pt) for pt in power_tests]
                csv_writer.writerow(csv_header)

                # For each modulation to test
                for tx_settings in tx_settings_tests:
                    
                    # Accumulate PDR results
                    pdr_results = [tx_settings.name]

                    # For each transmit power = {0...15}
                    for tx_power in power_tests:
                        print("Running noise={} with signal={}, SINR= {} dB and packet length {} bytes.".format(ix_settings.name, tx_settings.name,
                                                                                                                tx_power, packet_length))

                        # Configure interference power
                        if (tx_power == 0):
                            ix_active = False
                        else:
                            ix_active = True

                        # Update configuration based on current settings
                        self.configuration["tx_settings"] = tx_settings
                        self.configuration["ix_settings"] = ix_settings
                        self.configuration["ix_power"]    = tx_power
                        self.configuration["ix_active"]   = ix_active
                        self.configuration["tx_length"]   = packet_length

                        # Reset statistics
                        self.stats = {}

                        # Set statistics dictionary
                        self.receive.set_stats(self.stats)
                        self.transmit.set_stats(self.stats)
                        
                        # Configure transmit, receive and interfere
                        self.transmit.configure(configuration = self.configuration)
                        self.receive.configure(configuration = self.configuration)
                        self.interfere.configure(configuration = self.configuration)

                        # Start interference and delay 10 ms to allow to start
                        self.interfere.execute()
                        time.sleep(0.01)

                        # Start receiving and delay 10 ms to allow to start
                        self.receive.execute()
                        time.sleep(0.01)
                        
                        # Start transmitting
                        self.transmit.execute()

                        # Reset control variables
                        self.elapsed_ms = 0
                        self.is_finish  = False

                        # Record start time
                        start_time = time.time()

                        # Iterate while test not finished
                        while (not self.is_finish):
                            # Calculate elapsed time and update start time
                            current_time = time.time()
                            elapsed_time = 1000 * (current_time - start_time)
                            self.elapsed_ms += elapsed_time
                            start_time = current_time

                            # If the transmitter has stopped
                            self.tx_timeout = self.transmit.is_stopped()

                            # If we have exceeded the duration time, finish
                            if (self.tx_timeout or self.elapsed_ms > self.duration_ms):
                                self.is_finish = True
                                break

                            # Sleep until next iteration
                            time.sleep(0.001)

                        # Stop transmit, receive and interfere
                        self.interfere.stop()
                        self.transmit.stop()
                        self.receive.stop()
                        
                        # Wait for transmiter and receiver to finish
                        self.transmit.wait()
                        self.receive.wait()

                        # Calculate statistics
                        tx_packets = self.stats["tx_packets"]
                        rx_packets = self.stats["rx_packets"]
                        pdr = str(round(100.0 * (rx_packets / tx_packets), 2))

                        # Append results to list
                        pdr_results.append(pdr)

                    # Print PDR results
                    print(pdr_results)

                    # Write results to list
                    csv_writer.writerow(pdr_results)

                    print("********************************************************************************")

                # Close CSV file
                csv_file.close()

        # Notify threads to finish
        self.transmit.finish()                        
        self.receive.finish()
        self.interfere.finish()
        
        # Wait for threads to finish
        self.transmit.join()
        self.receive.join()
        self.interfere.join()

    def finish(self):
        self.is_finish = True

class BoardControl(threading.Thread):
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

class ReceiveBoard(BoardControl):
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
                logger.info("ReceiveBoard::run")

                # Start the radio
                result = self.send_cmd_message(timeout = 1.0, core = self.core, cmd = Serial_Cmd.ON)
                if (result == False):
                    logger.error("ReceiveBoard::run Error starting the radio")
                    raise ValueError("ReceiveBoard::run Error starting the radio")
                
                # Configure the radio
                result = self.send_cfg_message(timeout = 1.0, core = self.core, cmd = Serial_Cmd.CFG,
                                               settings = self.settings, frequency = self.frequency,
                                               tx_length = self.tx_length, tx_power = 0)
                if (result == False):
                    logger.error("ReceiveBoard::run Error configuring the radio")
                    raise ValueError("ReceiveBoard::run Error configuring the radio")
                
                self.elapsed_time = 0
                self.rx_packets = 0
                self.rx_timeout = 0

                # Set start time
                start_time = time.time()

                while (not self.is_finish and not self.is_stop):
                    # Try to receive a packet
                    result = self.send_cmd_message(timeout = 1.0, core = self.core, cmd = Serial_Cmd.RX, param = 25)
                    
                    if (result):
                        logger.info("ReceiveBoard::run Received packet from the radio")
                        self.rx_packets += 1
                    else:
                        self.rx_timeout += 1
                        logger.info("ReceiveBoard::run Timeout receiving packet from the radio")

                # Set end time
                end_time = time.time()

                # Calculate elapsed time
                self.elapsed_time = 1000 * (end_time - start_time)

                # Update the statistics
                self.stats["rx_time"] = self.elapsed_time
                self.stats["rx_packets"] = self.rx_packets
                self.stats["rx_timeout"] = self.rx_timeout

                # Reset the radio
                result = self.send_cmd_message(timeout = 1.0, core = self.core, cmd = Serial_Cmd.RST)

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

class TransmitBoard(BoardControl):
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
                logger.info("TransmitBoard::run")

                # Start the radio
                result = self.send_cmd_message(timeout = 1.0, core = self.core, cmd = Serial_Cmd.ON)
                if (result == False):
                    logger.error("TransmitBoard::run Error starting the radio")
                    raise ValueError("TransmitBoard::run Error starting the radio")

                # Configure the radio
                result = self.send_cfg_message(timeout = 1.0, core = self.core, cmd = Serial_Cmd.CFG,
                                               settings = self.settings, frequency = self.frequency,
                                               tx_length = self.tx_length, tx_power = self.tx_power)
                if (result == False):
                    logger.error("TransmitBoard::run Error configuring the radio")
                    raise ValueError("TransmitBoard::run Error configuring the radio")
                
                iterations = self.packet_count
                self.tx_packets = 0

                # Set end time
                start_time = time.time()
                
                while (not self.is_finish and not self.is_stop and iterations > 0):
                    # Try to send a packet
                    result = self.send_cmd_message(timeout = 1.0, core = At86rf215_Core.RF09, cmd = Serial_Cmd.TX)
                    
                    if (result):
                        logger.info("TransmitBoard::run Transmitted packet to the radio")
                        self.tx_packets += 1
                    else:
                        logger.error("TransmitBoard::run Error sending packet to the radio")
                        raise ValueError("TransmitBoard::run Error sending packet to the radio")

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
                result = self.send_cmd_message(timeout = 1.0, core = self.core, cmd = Serial_Cmd.RST)
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

class InterfereBoard(BoardControl):
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
                result = self.send_cmd_message(timeout = 1.0, core = self.core, cmd = Serial_Cmd.ON)
                if (result == False):
                    logger.error("InterfereBoard::run Error starting the radio")
                    raise ValueError("InterfereBoard::run Error starting the radio")

                # Configure the radio
                result = self.send_cfg_message(timeout = 1.0, core = self.core, cmd = Serial_Cmd.CFG,
                                               settings = self.settings, frequency = self.frequency,
                                               tx_length = self.tx_length, tx_power = self.tx_power)
                if (result == False):
                    logger.error("InterfereBoard::run Error reseting the radio")
                    raise ValueError("InterfereBoard::run Error reseting the radio")
                
                # Start continuous transmission
                result = self.send_cmd_message(timeout = 1.0, core = self.core, cmd = Serial_Cmd.TX_INTERF, param = 1)
                if (result == False):
                    logger.error("InterfereBoard::run Error starting continuous transmission")
                    raise ValueError("InterfereBoard::run Error starting continuous transmission")
                    
                # Repeat until finish condition
                while (not self.is_finish and not self.is_stop):
                    time.sleep(0.001)

                # Reset the radio
                result = self.send_cmd_message(timeout = 1.0, core = self.core, cmd = Serial_Cmd.RST)

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

################################################################################

finished = False

def signal_handler(sig, frame):
    global finished
    finished = True

def main():
    global finished 

    # Set-up logging back-end
    logging.basicConfig(level=logging.ERROR)

    transmit_uart  = "COM25"
    receive_uart   = "COM55"
    interfere_uart = "COM29"
    baudrate       = 1267200

    configuration = {"core": At86rf215_Core.RF09,
                     "tx_settings": [At86rf215_Cfg.OFDM_1_MCS_1, At86rf215_Cfg.OFDM_2_MCS_2, At86rf215_Cfg.OFDM_3_MCS_3, At86rf215_Cfg.OFDM_4_MCS_5, At86rf215_Cfg.OQPSK_RATE_5],
                     "tx_frequency": At86rf215_Freq.FREQ_OQPSK_1, 
                     "tx_power": 12,
                     "tx_length": [20, 120],
                     "ix_settings": [At86rf215_Cfg.OFDM_1_MCS_1, At86rf215_Cfg.OFDM_2_MCS_2, At86rf215_Cfg.OFDM_3_MCS_3, At86rf215_Cfg.OFDM_4_MCS_5, At86rf215_Cfg.OQPSK_RATE_5],
                     "ix_frequency": At86rf215_Freq.FREQ_OQPSK_1, 
                     "ix_power": [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15],
                     "ix_length": 123,
                     "packet_count": 100,
                     "packet_delay": 0.005,
                     "duration_ms": 30000}

    # Set up SIGINT signal
    signal.signal(signal.SIGINT, signal_handler)

    transmit  = TransmitBoard(port = transmit_uart, baudrate = baudrate)
    interfere = InterfereBoard(port = interfere_uart, baudrate = baudrate)
    receive   = ReceiveBoard(port = receive_uart, baudrate = baudrate)

    # Create experiment manager
    em = ExperimentManager(transmit = transmit, receive = receive, interfere = interfere, configuration = configuration)
    
    # Start experiment manager
    em.start()

    while (not finished):
        time.sleep(0.1)

    # Stop experiment manager
    em.finish()
    em.join()

if __name__ == "__main__":
    main()