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

finished = False

logger = logging.getLogger(__name__)

class MoteSerialImplementation(MoteSerial.MoteSerial):

    # Recover network configuration
    pan_id = config.network_config["pan_id"]

    # Reocver message configuration
    message_id        = config.message_config["message_id"]
    message_topic     = config.message_config["message_topic"]
    message_fields    = config.message_config["message_fields"]
    message_structure = config.message_config["message_structure"]

    def __init__(self, serial_port=None, serial_baudrate=None, serial_timeout=None, mqtt_client=None):
        super().__init__(serial_port=serial_port, serial_baudrate=serial_baudrate, serial_timeout = serial_timeout)
        self.mqtt_client = mqtt_client

    def run(self):
        logger.info("Starting MoteSerialImplementation at port={} with baudrate={}.".format(self.serial_port, self.serial_baudrate))

        # Repeat until finish condition
        while (not finished):
            success = False

            # Try to receive a Serial message
            message, length = self.serial.receive(timeout = self.serial_timeout)

            # If we received a message
            if (length > 0):
                try:
                    # Convert message to bytearray
                    message = bytearray(bytes(message))

                    # Unpack the message according to its structure
                    message_items = struct.unpack(self.message_structure, message)

                    # Convert to dictionary
                    result = dict(zip(self.message_fields, message_items))

                    # Check that packet comes from a known network
                    if (result["pan_id"].hex() == self.pan_id):

                        # Process data in dictionary
                        result["gw_id"]   = self.message_id
                        result["pan_id"]  = result["pan_id"].hex()
                        result["node_id"] = result["node_id"].hex()
                        result["temp"]    = result["temp"] / 10.0
                        result["rhum"]    = result["rhum"] / 10.0
                        result["pres"]    = result["pres"] / 10.0
                        result["lght"]    = result["lght"] / 10.0

                        # Mark packet conversion as successful
                        success = True
                    else:
                        logger.error("program: Received pan_id={} does not match local pan_id={}".format(result["pan_id"], self.pan_id))
                except Exception as e:
                    logger.error("program: Error unpacking at port={}.".format(self.serial_port))

            # If the message was parsed successfully
            if (success is True):
                try:
                    # Create MQTT message
                    mqtt_message = json.dumps(result)

                    # Send MQTT message
                    self.mqtt_client.send_message(self.message_topic, mqtt_message)

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

def main():
    global finished

    # List to hold serial_motes objects
    serial_motes = []

    # Set up logging back-end
    logging.basicConfig(level=logging.ERROR)

    # Set up SIGINT signal
    signal.signal(signal.SIGINT, signal_handler)

    # Recover serial configuration
    serial_devices  = config.serial_config['serial_devices']
    serial_baudrate = config.serial_config['serial_baudrate']
    serial_timeout  = config.serial_config['serial_timeout']

    # Recover MQTT configuration
    mqtt_address = config.mqtt_config['mqtt_address']
    mqtt_port    = config.mqtt_config['mqtt_port']

    # Create MQTT client
    mqtt_client = MqttClient.MqttClient(address = mqtt_address, port = mqtt_port)
    mqtt_client.start()

    # Recover serial_ports available
    serial_ports = MoteSerial.serial_ports()
    serial_length = len(serial_ports)

    logger.info("main: Operating with {} of {} devices!".format(serial_length, serial_devices))

    # Create MoteSerialImplementation objects
    for serial_port in serial_ports:
        serial_mote = MoteSerialImplementation(serial_port = serial_port, serial_baudrate = serial_baudrate,
                                               serial_timeout = serial_timeout, mqtt_client = mqtt_client)
        serial_motes.append(serial_mote)

    # Start MoteSerialImplementation objects
    for serial_mote in serial_motes:
        serial_mote.start()

    # Wait until finished
    while (not finished):
        time.sleep(0.1)

    # Stop MoteSerialImplementation objects
    for serial_mote in serial_motes:
        serial_mote.stop()
        serial_mote.join()

    # Stop MQTT client
    mqtt_client.stop()
    mqtt_client.join()

if __name__ == "__main__":
    main()
