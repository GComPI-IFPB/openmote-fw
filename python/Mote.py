# -*- coding: utf-8 -*-

from Serial import Serial




class Mote(object):

    CMD_CONFIG_RADIO = 0x00
    CMD_SEND_DATA = 0x01

    def __init__(self, port=None, baudrate=None):
        self.port     = port
        self.baudrate = baudrate
        self.name     = port
        self.serial   = Serial(name=port, baudrate=baudrate)

    def getName(self):
        return self.name

    def sendData(self,data):
        #send the command indicating we are going to transmit
        self.serial.transmit(self.CMD_SEND_DATA)
        self.serial.transmit(data)

    def configureRadio(self,config):
       self.serial.transmit(self.CMD_CONFIG_RADIO)
       self.serial.transmit(config)

    def close(self):
       self.serial.stop()



