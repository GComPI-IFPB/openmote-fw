#!/usr/bin/python

# Import libraries
import os
import serial
import sys

# Import library
library_path = os.path.abspath('../../python/library')
sys.path.append(library_path)

# Import classes from library
from MoteUtils import find_serial_ports
from Hdlc import Hdlc

class TestSniffer():
    serial_ports    = find_serial_ports()
    hdlc            = Hdlc()
    default_channel = 20
    
    def __init__(self):
        pass
    
    def run(self):       
        if (self.serial_ports == None):
            sys.exit()
        else:
            port      = self.serial_ports[0][0]
            baudrate  = self.serial_ports[0][1]
            print("- Serial: Listening to port " + port + " at " + str(baudrate) + " bps.")
            ser =  serial.Serial(port = port, baudrate = baudrate)
        
        print("- Radio: Listening to channel " + str(self.default_channel) + ".")
        
        while(True):
            # Empty buffer
            buffer = []
            
            # Read the new channel
            channel = int(raw_input("Insert the new channel number (11-26): "))
            
            if (channel < 11 or channel > 26):
                print("- Channel without bounds.")
            else:
                print("- Changing to channel " + str(channel) + ".")
                
                # Append the command to the buffer
                buffer.append(chr(0xCC))
                
                # Append the channel to the buffer
                buffer.append(chr(channel))
                
                # HDLCify the buffer
                data = self.hdlc.hdlcify(buffer)
                
                # Write to the serial port
                ser.write(data)
                
def main():
    testSniffer = TestSniffer()
    testSniffer.run()
    
if __name__ == "__main__":
    main()
