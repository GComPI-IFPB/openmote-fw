'''
@file       ExperimentManager.py
@author     Pere Tuset-Peiro  (peretuset@openmote.com)
@version    v0.1
@date       February, 2019
@brief      

@copyright  Copyright 2019, OpenMote Technologies, S.L.
            This file is licensed under the GNU General Public License v2.
'''

import threading

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

        # Interfence settings
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

                    # For each transmit power
                    for tx_power in power_tests:
                        print("Running noise={} with signal={}, SINR={} dB, Length={} bytes.".format(ix_settings.name, tx_settings.name,
                                                                                                                tx_power, packet_length))

                        # Configure interference power
                        if (tx_power == None):
                            ix_active = False
                        else:
                            ix_active = True

                        # Normalize power when using OQPSK-DSSS
                        if (ix_settings == At86rf215_Cfg.OQPSK_RATE_5 and tx_power != None):
                            tx_power -= 3

                        if (tx_settings == At86rf215_Cfg.OQPSK_RATE_5):
                            self.configuration["tx_power"] = self.configuration["tx_power"] - 3

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

                        # Recover transmit power settings
                        if (tx_settings == At86rf215_Cfg.OQPSK_RATE_5):
                            self.configuration["tx_power"] = self.configuration["tx_power"] + 3

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