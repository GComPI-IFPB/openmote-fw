#!/usr/bin/python

# Import libraries
import os
import serial
import sys
import getopt

# Import library
library_path = os.path.abspath('../../python/library')
sys.path.append(library_path)

# Import classes from library
from MoteUtils import find_serial_ports, select_serial_port
from Hdlc import Hdlc

#from scapy.all import *

class PacketSniffer():
    def __init__(self):
        pass
    
    def run(self):
        sniff(iface="eth0", prn=self.pkt_callback, filter="6lowpan", store=0)
        
    def pkt_callback(self, pkt):
        pkt.show()

class TestSniffer():
    default_channel    = 20
    cmd_change_channel = chr(0xCC)
    
    serial_port     = None
    serial_baudrate = None
    
    def __init__(self, serial_port = None, serial_baudrate = None):
        assert serial_port != None, "Serial port not defined."
        assert serial_baudrate != None, "Serial baudrate not defined."
        
        self.serial_port = serial_port
        self.serial_baudrate = serial_baudrate
        
        print("- Serial: Listening to port " + self.serial_port + " at " + str(self.serial_baudrate) + " bps.")
    
    def run(self):    
        serial_port = None
        hdlc = Hdlc()
        
        # Open the serial port
        self.serial_port = serial.Serial(port = self.serial_port, 
                                         baudrate = self.serial_baudrate,
                                         timeout = 1)
        
        print("- Radio: Listening to IEEE 802.15.4 default channel (" + str(self.default_channel) + ").")
        
        while(True):
            # Empty buffer
            buffer = []
            
            # Read the new channel
            channel = int(raw_input("Insert the new IEEE 802.15.4 channel number (11-26): "))

            # Check channel bounds
            if (channel < 11 or channel > 26):
                print("- Channel without bounds.")
            else:
                print("- Changing to IEEE 802.15.4 channel " + str(channel) + ".")
                
                # Append the command to the buffer
                buffer.append(self.cmd_change_channel)
                
                # Append the channel to the buffer
                buffer.append(chr(channel))
                
                # HDLCify the buffer
                data = hdlc.hdlcify(buffer)
                
                # Write to the serial port
                self.serial_port.write(data)
                
                while(True):
                    bytes = self.serial_port.read(size = 255)
                    if (bytes != None):
                        print("Read " + str(len(bytes)) + " bytes.")

def parse_config(config = None, arguments = None):
    assert config != None, "Config not defined."
    assert arguments != None, "Arguments not defined."

    try:
        opts, args = getopt.getopt(arguments, "p:b:")
    except getopt.GetoptError as error:
        print(str(error))
        sys.exit(1)
    
    # Parse command line options
    for option, value in opts:
        if option == '-p':
            config['port'] = value
        elif option == '-b':
            config['baudrate'] = value
        else:
            assert False, "Unhandled option."
    
    # If no serial port is passed, notify the user
    if config['port'] == 'none':
        serial_ports = find_serial_ports()
        if (serial_ports):
            serial_port = select_serial_port(serial_ports)
            if (serial_port):
                config['port'] = serial_port
        else:
            print("No serial port was found in the computer.")
            sys.exit(2)
    
    return config
    
def main():
    default_config = {
        'port'     : 'none',
        'baudrate' : 115200
    }
    
    arguments = sys.argv[1:]
    config    = parse_config(default_config, arguments)
    
    testSniffer = TestSniffer(serial_port     = config['port'],
                              serial_baudrate = config['baudrate'])
    testSniffer.run()
    
if __name__ == "__main__":
    main()
