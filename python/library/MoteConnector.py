'''
@file       MoteConnector.py
@author     Pere Tuset-Peiro  (peretuset@openmote.com)
@version    v0.1
@date       May, 2015
@brief      

@copyright  Copyright 2015, OpenMote Technologies, S.L.
            This file is licensed under the GNU General Public License v2.
'''

import MoteProbe as MoteProbe
import MoteParser as MoteParser

from pydispatch import dispatcher

class MoteConnector(object):

    def __init__(self, serial_ports = None):
        
        # Module name
        self.moduleName = 'moteConnector'
        
        # Module signals
        self.to_MoteProbe = 'to_MoteProbe'
        self.from_MoteProbe = 'from_MoteProbe'
        self.to_MoteParser = 'to_MoteParser'
        self.from_MoteParser = 'from_MoteParser'
        
        # Store the serial ports
        self.serial_ports = serial_ports
        
        # List to store all moteProbe objects
        self.moteProbes = []
        
        for serial_port in self.serial_ports:
            # Creates the MoteProbe
            moteProbe = MoteProbe.MoteProbe(mote_connector = self.moduleName, serial_port = serial_port)
            
            # Appends the MoteProbe to the MoteProbe list
            self.moteProbes.append(moteProbe)
            
            # Connects the MoteProbe with the MoteConnector
            dispatcher.connect(self._from_MoteProbe, sender = moteProbe.moduleName, signal = moteProbe.from_MoteProbe)
            
            # Starts the current MoteProbe
            moteProbe.start()
        
        # Create the MoteParser object
        self.moteParser = MoteParser.MoteParser(mote_connector = self.moduleName)
        
        # Connects the MoteParser with the MoteConnector
        dispatcher.connect(self._from_MoteParser, sender = self.moteParser.moduleName, signal = self.from_MoteParser)
    
    def get_parser(self):
        return self.moteParser
    
    def _to_MoteParser(self, data = None):
        dispatcher.send(sender = self.moduleName, signal = self.to_MoteParser, data = data)
        
    def _to_MoteProbe(self, data = None):
        dispatcher.send(sender = self.moduleName, signal = self.to_MoteProbe, data = data)
        
    def _from_MoteParser(self, sender = None, data = None):
        self._to_MoteProbe(data)
    
    def _from_MoteProbe(self, sender = None, data = None):        
        # Parse the payload 
        command = data[0]
        address = data[1:2]
        payload = data[3:]
        
        # Create the new data
        data = (command, address, payload)
        
        # Send to MoteParser
        self._to_MoteParser(data = data)
        
