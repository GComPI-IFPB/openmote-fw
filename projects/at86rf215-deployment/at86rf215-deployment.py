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

# Append library to system path
pwd = os.path.abspath(__file__)
pwd = os.path.dirname(os.path.dirname(os.path.dirname(pwd)))
pwd = os.path.join(pwd, 'python')
sys.path.append(pwd)

import argparse
import config
import json
import logging
import random
import signal
import string
import struct
import time

import MoteSerial
import MqttClient

mqtt_id      = config.mqtt_config['mqtt_id']
mqtt_address = config.mqtt_config['mqtt_address']
mqtt_port    = config.mqtt_config['mqtt_port']
mqtt_topic   = config.mqtt_config['mqtt_topic']

serial_baudrate = config.serial_config['serial_baudrate']
serial_timeout = config.serial_config['serial_timeout']

finished = False

logger = logging.getLogger(__name__)

class MoteSerialImplementation(MoteSerial.MoteSerial):
    def __init__(self, serial_port=None, serial_baudrate=None, serial_timeout=None, mqtt_client=None):
        super().__init__(serial_port=serial_port, serial_baudrate=serial_baudrate, serial_timeout = serial_timeout)
        self.mqtt_client = mqtt_client

    def run(self):
        logger.info("Starting MoteSerialImplementation at port={} with baudrate={}.".format(self.serial_port, self.serial_baudrate))

        # Recover message structure from config
        message_structure = config.message_config['message_structure']
        message_fields = config.message_config['message_fields']
        
        # Repeat until finish condition
        while (not finished):
            # Try to receive a Serial message
            message, length = self.serial.receive(timeout = self.serial_timeout)

            # If we received a message
            if (length > 0):
                try:
                    # Convert message to bytearray
                    message = bytearray(bytes(message))
                    
                    # Unpack the message according to its structure
                    message_items = struct.unpack(message_structure, message)
                    
                    # Convert to dictionary
                    result = dict(zip(message_fields, message_items))

                    # Process data in dictionary
                    result['gateway_id'] = mqtt_id
                    result['node_id'] = result['node_id'].hex()
                    result['temperature'] = result['temperature'] / 10.0
                    result['humidity'] = result['humidity'] / 10.0
                    result['pressure'] = result['pressure'] / 10.0
                    result['light'] = result['light'] / 10.0

                    logger.info(result)
                except:
                    logger.error("program: Error unpacking.")

                try:
                    # Create MQTT message
                    mqtt_message = json.dumps(result)

                    # Send MQTT message
                    self.mqtt_client.send_message(mqtt_topic, mqtt_message)

                    logger.info(mqtt_message)
                except:
                    logger.error("program: Error sending MQTT packet.")

    def stop(self):
        logger.info("Stopping program at port={} with baudrate={}.".format(self.serial_port, self.serial_baudrate))

        # Call parent stop
        super().stop()


def signal_handler(sig, frame):
    global finished
    finished = True

def program(serial_baudrate = None):
    global finished

    motes = []

    # Create MQTT client
    mqtt_client = MqttClient.MqttClient(mqtt_address, mqtt_port)
    mqtt_client.start()

    serial_ports = MoteSerial.serial_ports()

    for serial_port in serial_ports:
        m = MoteSerialImplementation(serial_port = serial_port, serial_baudrate = serial_baudrate, 
                                     serial_timeout = serial_timeout, mqtt_client = mqtt_client)
        motes.append(m)
        
    for mote in motes:
        mote.start()

    while(not finished):
        time.sleep(0.1)

    for mote in motes:
        mote.stop()
        mote.join()

    # Stop MQTT client
    mqtt_client.stop()

def main():
    # Set up logging back-end 
    logging.basicConfig(level=logging.INFO)

    # Set up SIGINT signal
    signal.signal(signal.SIGINT, signal_handler)

    # Create argument parser
    parser = argparse.ArgumentParser(description="")
    parser.add_argument("-b", "--baudrate", type=int, default=0)

    # Parse arguments
    args = parser.parse_args()

    # Overrite 
    if (args.baudrate == 0 or not serial_baudrate):
        args.baudrate = 115200
    else:
        args.baudrate = serial_baudrate

    # Execute program
    program(serial_baudrate = args.baudrate)
    
if __name__ == "__main__":
    main()
