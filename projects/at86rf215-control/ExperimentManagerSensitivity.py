'''
@file       ExperimentManager.py
@author     Pere Tuset-Peiro  (peretuset@openmote.com)
@version    v0.1
@date       February, 2019
@brief      

@copyright  Copyright 2019, OpenMote Technologies, S.L.
            This file is licensed under the GNU General Public License v2.
'''

import csv
import logging
import math
import threading
import time

from At86rf215 import *

logger = logging.getLogger(__name__)

class ExperimentManagerSensitivity(threading.Thread):
    def __init__(self, transmit = None, receive = None, configuration = None):
        self.transmit      = transmit
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

        tx_settings_tests = self.configuration["tx_settings"]
        tx_power_tests    = self.configuration["tx_power"]
        length_tests      = self.configuration["tx_length"]
        
        # Packet length = {20, 120}
        for packet_length in length_tests:

            # Generate file name
            filename = "sensitivity" + "_" + str(packet_length) + "bytes" + ".csv"
            
            # Open CSV file and writer
            csv_file = open(filename, 'w', newline='')
            csv_writer = csv.writer(csv_file, dialect='excel')
            
            # Write CSV header to file
            csv_header = ["TX POWER"] + [str(pt) for pt in tx_power_tests]
            csv_writer.writerow(csv_header)

            # For each modulation to test
            for tx_settings in tx_settings_tests:

                # Accumulate PDR results
                pdr_results = [tx_settings.name]

                # Transmit power settings
                for tx_power in tx_power_tests:

                    # Update configuration based on current settings
                    self.configuration["tx_settings"] = tx_settings
                    self.configuration["tx_power"]    = tx_power
                    self.configuration["tx_length"]   = packet_length

                    # Reset statistics
                    self.stats = {}

                    # Set statistics dictionary
                    self.receive.set_stats(self.stats)
                    self.transmit.set_stats(self.stats)
                    
                    # Configure transmit, receive and interfere
                    self.transmit.configure(configuration = self.configuration)
                    self.receive.configure(configuration = self.configuration)

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
                    self.transmit.stop()
                    self.receive.stop()
                    
                    # Wait for transmiter and receiver to finish
                    self.transmit.wait()
                    self.receive.wait()

                    # Calculate statistics
                    tx_packets = self.stats["tx_packets"]
                    rx_packets = self.stats["rx_packets"]
                    pdr = str(round(100.0 * (rx_packets / tx_packets), 2))

                    # Print test report
                    print("Running Signal={}, Length={} bytes, TX Power={:.0f}, PDR={}%.".format(
                            tx_settings.name, packet_length, tx_power, pdr))

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
        
        # Wait for threads to finish
        self.transmit.join()
        self.receive.join()

    def finish(self):
        self.is_finish = True