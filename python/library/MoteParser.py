# -*- encoding: utf-8 -*-

'''
@file       MoteParser.py
@author     Pere Tuset-Peiro  (peretuset@openmote.com)
@version    v0.1
@date       May, 2014
@brief      
'''

from pydispatch import dispatcher as dispatcher

class MoteParser(object):
    PARSER_PC2MOTE_START = 'A'
    PARSER_PC2MOTE_STOP = 'O'
    
    PARSER_PC2MOTE_ADDRESS = ['\x00', '\x00']
    
    PARSER_MOTE2PC_DATA = 'D'
    PARSER_MOTE2PC_RESET = 'R'
    
    mac_type = ""
    mac_slots = 0
    mac_duration = 0
    
    dictionary = None
    parser = None
    parser_writer = None
    stats = None

    def __init__(self, mote_connector = None):
        # Module name
        self.moduleName = 'moteParser'

        # Module signals
        self.from_MoteConnector = 'from_MoteConnector'
        self.to_MoteConnector = 'to_MoteConnector'
        self.to_MoteParser = 'to_MoteParser'
        self.from_MoteParser = 'from_MoteParser'
        self.to_Application = 'to_Applications'
        
        # Mote connector
        self.mote_connector = mote_connector

        # Connects the MoteProbe with the MoteConnector
        dispatcher.connect(self._from_MoteConnector, sender = self.mote_connector, signal = self.to_MoteParser)
    
    def set_signals(self, data = None, reset = None):
        self.to_MainFrameData = data
        self.to_MainFrameReset = reset 
    
    def start(self, payload = None):        
        command = []
        
        # Extend the START command with the command
        command.extend(self.PARSER_PC2MOTE_START)
        
        # Extend the START command with the address
        command.extend(self.PARSER_PC2MOTE_ADDRESS)
        
        # Extend the START command with the payload
        if (payload != None):
            command.extend(payload)
        
        # Send the start command
        self._to_MoteConnector(command)
        
    def stop(self, payload = None):
        command = []
        
        # Extend the STOP command with the address
        command.extend(self.PARSER_PC2MOTE_STOP)
        
        # Extend the STOP command with the address
        command.extend(self.PARSER_PC2MOTE_ADDRESS)
        
        # Extend the STOP command with the payload
        command.extend(payload)
        
        # Send the start command
        self._to_MoteConnector(command)
    
    def _to_MoteConnector(self, data = None):
        dispatcher.send(signal = self.from_MoteParser, sender = self.moduleName, data = data)
    
    def _from_MoteConnector(self, signal = None, data = None):
        # Unpack the data        
        command, address, payload = data
        
        # Send the data to the application
        self._to_Application(command = command, address = address, data = payload)

    def _to_Application(self, command = None, address = None, data = None):
        if (command == self.PARSER_MOTE2PC_RESET):
            dispatcher.send(signal = self.to_MainFrameReset, sender = self.moduleName)
        elif (command == self.PARSER_MOTE2PC_DATA):
            dispatcher.send(signal = self.to_MainFrameData, sender = self.moduleName, data = data)
        else:
            print "Not implemented!"
