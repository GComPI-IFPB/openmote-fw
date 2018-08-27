# -*- coding: utf-8 -*-

# Import Python libraries
import serial
import threading
import time
import logging
import binascii

# Import OpenMote libraries
from Hdlc import Hdlc

# Import logging configuration
logger = logging.getLogger(__name__)

class Serial(threading.Thread):
    
    def __init__(self, serial_name = None, baud_rate = None):
        assert serial_name != None, logger.error("Serial port not defined.")
        assert baud_rate   != None, logger.error("Serial baudrate not defined.")
        
        logger.info('init: Creating the Serial object.')
        
        # Call constructor
        threading.Thread.__init__(self)
        
        # Set thread as daemon
        self.setDaemon(False)
        
        # Terminate thread event
        self.stop_event = threading.Event()
        
        # Serial port
        self.serial_port = None
        self.serial_name = serial_name
        self.baud_rate   = baud_rate
        self.time_out    = 0.1
                
        # HDLC driver
        self.hdlc = Hdlc()
        
        # Receive variables
        self.receive_buffer    = []
        self.receive_message   = []
        self.receive_condition = threading.Condition()
        self.is_receiving      = False
        self.rx_byte           = ''
        self.last_rx_byte      = ''
        
        # Transmit variables 
        self.transmit_buffer    = []
        self.transmit_message   = []
        self.transmit_condition = threading.Condition()
        
        try:
            logger.info('init: Opening the serial port on %s at %s bps.', self.serial_name, self.baud_rate)
            # Open the serial port
            self.serial_port = serial.Serial(port = self.serial_name, 
                                             baudrate = self.baud_rate,
                                             timeout = self.time_out)
        except:
            logger.error('init: Error while opening the serial port on %s.', self.serial_name)
            raise Exception
        else:
            logger.info('init: Serial object created.')
    
    # Runs the thread
    def run(self):
        logger.info("run: Starting the Serial.")
        
        if (self.serial_port == None):
            return
        
        # Flush the serial input/ouput               
        self.serial_port.flushInput()
        self.serial_port.flushOutput() 
        
        # Execute while thread is alive
        while (not self.stop_event.isSet()): 
            try:
                # Try to receive a byte from the serial port (blocking)
                rx = self.serial_port.read(size = 1)
                if (len(rx) > 0):
                    self.rx_byte = rx
            except:
                logger.error('run: Error while receiving from the serial port on %s.', self.serial_port)
                # Terminate the thread
                self.stop()
                # Break the loop
                break

            # Start of frame
            if ((not self.is_receiving) and 
                (self.last_rx_byte == self.hdlc.HDLC_FLAG) and
                (self.rx_byte != self.hdlc.HDLC_FLAG)):
                logger.debug('Start of HDLC frame.')
                
                self.is_receiving = True
                self.receive_buffer = []
                self.receive_buffer.append(self.hdlc.HDLC_FLAG)
                self.receive_buffer.append(self.rx_byte)
                
            # Middle of HDLC frame
            elif ((self.is_receiving) and
                  (self.rx_byte != self.hdlc.HDLC_FLAG)):
                logger.debug('Middle of HDLC frame.')
                
                self.receive_buffer.append(self.rx_byte)
                
            # End of HDLC frame 
            elif ((self.is_receiving) and
                  (self.rx_byte == self.hdlc.HDLC_FLAG)):
                logger.debug('End of HDLC frame.')

                # Receive the last byte
                self.receive_buffer.append(self.rx_byte)
                
                # Reset the variables
                self.is_receiving = False
                self.last_rx_byte = ''
                self.rx_byte = ''
                
                try:
                    logger.debug('run: Received an HDLC frame from the Serial port, now de-HDLCifying it.')
                    self.receive_message = self.hdlc.dehdlcify(self.receive_buffer)
                except:
                    logger.error('run: Error while de-HDLCifying the frame received from the Serial port.')
                    self.receive_buffer  = []
                    self.receive_message = []
                else:
                     # Acquire the transmit condition
                    self.receive_condition.acquire()                   
                    
                     # Notify the transmit condition
                    self.receive_condition.notify()
                    
                     # Release the transmit condition
                    self.receive_condition.release()
                    
                    # Reset the receive buffer
                    self.receive_buffer = []
            
            else:
                # Acquire the transmit condition
                self.transmit_condition.acquire()
                
                # Check if there is something to transmit
                if (not self.is_receiving and self.transmit_message):
                    logger.debug('run: HDLCifying the transmit buffer.')
                    
                    # HDLCify the message
                    self.transmit_buffer = self.hdlc.hdlcify(self.transmit_message)
                    
                    logger.debug('run: Now transmitting the message.')
                    
                    # Send the message through the serial port (blocking)
                    self.serial_port.write(self.transmit_buffer)
                    
                    # Empty the transmit message and buffer
                    self.transmit_message = []
                
                # Release the transmit condition
                self.transmit_condition.release()
                
            # Always save the last received byte
            self.last_rx_byte = self.rx_byte
    
    # Stops the thread
    def stop(self):
        logger.info('stop: Stopping the Serial port.')

        # Terminates the thread
        self.stop_event.set()
    
    # Receive a message
    def receive(self):
        status  = True
        message = []
        length  = 0
        
        # Acquire the receive condition
        self.receive_condition.acquire()
        
        # Try to receive a message with timeout
        self.receive_condition.wait(0.5)

        # If we really got a message, copy it!
        if (not self.is_receiving and self.receive_message):
            message = b''.join(self.receive_message)
            length  = len(message)
            logger.info('receive: Received a message with %d bytes.', length)
        
        # Reset the receive message
        self.receive_message = []
        
        # Release the receive condition
        self.receive_condition.release()
        
        # Get the status of the thread
        status = self.stop_event.isSet()
        
        # Return the received message and length
        return (message, length)
    
    # Transmit a message
    def transmit(self, message):
        logger.info('transmit: Got a message to transmit with %d bytes.', len(message))
        
        # Acquire the transmit condition
        self.transmit_condition.acquire()
        
        # Copy the message
        self.transmit_message = message

        # Release the transmit condition
        self.transmit_condition.release()