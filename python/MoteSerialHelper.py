# -*- coding: utf-8 -*-

# Import Python libraries
import logging
import serial
import sys
import time
import glob

# Import OpenMote libraries
from Serial import Serial
from Mote import Mote

# Import logging configuration
logger = logging.getLogger(__name__)

class MoteSerialHelper(object):
    HELLO_MESSAGE = "OneRing2RuleThemAll"

    def __init__(self):
        logger.info('init: Creating the MoteSerialHelper object.')
        #mote dictionary
        self.motes = {}
        self.baudrate = 115200
        self.serialInterface = None

    def configureSerial(self, baudrate):
        self.baudrate = baudrate

    # Discover Motes
    def discoverMotes(self):
        logger.info('discoverMotes: scanning serial.')
        # for each serial port test serial
        ports = self.serial_ports()

        start_time = time.time()

        for port in ports:
            #create the serial connection
            logger.info("discoverMotes: trying port {}.".format(port))
            self.serialInterface = Serial(name = port, baudrate = self.baudrate, timeout = 0.001)
            self.serialInterface.start()
            #send the hello message
            self.serialInterface.transmit(self.HELLO_MESSAGE)
            #wait for a response.. at most 1s
            finished = False

            while (not finished):
                # Try to receive a Serial message
                message, length = self.serialInterface.receive(timeout= 0.01)

                # If we received a message
                if (length > 0):
                    logger.info("discoverMotes: Received message with %d bytes.", length)
                    finished = True
                #compute wait time
                current_time = time.time()
                elapsed_time = 1000 * (current_time - start_time)
                if (elapsed_time > 1000):
                    #print(elapsed_time)
                    start_time = time.time()
                    finished = True

            if (self.HELLO_MESSAGE in message):
                print("Openmote Found! at {}".format(port))
                self.motes [port] = Mote(port,self.baudrate)
            else:
                print("No OpenMotes found :( ")
            #close this serial
            self.serialInterface.stop()
        #print(self.motes)
        return self.motes

    def serial_ports(self):
        """ Lists serial port names
            :raises EnvironmentError:
                On unsupported or unknown platforms
            :returns:
                A list of the serial ports available on the system
        """
        if sys.platform.startswith('win'):
            ports = ['COM%s' % (i + 1) for i in range(256)]
        elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
            # this excludes your current terminal "/dev/tty"
            ports = glob.glob('/dev/tty[A-Za-z]*')
        elif sys.platform.startswith('darwin'):
            ports = glob.glob('/dev/tty.*')
        else:
            raise EnvironmentError('Unsupported platform')

        result = []
        for port in ports:
            try:
                s = serial.Serial(port)
                s.close()
                result.append(port)
            except (OSError, serial.SerialException):
                pass
        return result

