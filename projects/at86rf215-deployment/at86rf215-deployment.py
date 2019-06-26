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
pwd = os.path.abspath(__file__)
pwd = os.path.dirname(os.path.dirname(os.path.dirname(pwd)))
pwd = os.path.join(pwd, 'python')
sys.path.append(pwd)

import argparse
import signal
import struct
import logging
import time
import random
import string
import json

import MoteSerial
import MqttClient

mqtt_address = "34.244.230.156"
mqtt_port    = 1883
mqtt_topic   = "test"

finished = False

timeout = 0.1

logger = logging.getLogger(__name__)

class MoteSerialImplementation(MoteSerial.MoteSerial):
    def __init__(self, port=None, baudrate=None, mqtt_client=None):
        super().__init__(port=port, baudrate=baudrate)
        self.mqtt_client = mqtt_client

    def run(self):
        print("Starting MoteSerialImplementation at port={} with baudrate={}.".format(self.port, self.baudrate))
        
        # Repeat until finish condition
        while (not finished):
            # Try to receive a Serial message
            message, length = self.serial.receive(timeout = timeout)

            # If we received a message
            if (length > 0):
                try:
                    message = bytearray(bytes(message))
                    eui64, retrans, mode, counter, t, h, p, l, rssi = struct.unpack('>8sbbIhhhhb', message)
                    eui64 = eui64.hex()
                    t = t/10.0
                    h = h/10.0
                    p = p/10.0
                    l = l/10.0
                    logger.info("Address={}, Retrans={}, Mode={}, Counter={}, Temperature={}, Humidity={}, Pressure={}, RSSI={}".format(eui64, retrans, mode, counter, t, h, p, rssi))
                except:
                    logger.error("program: Error unpacking.")

                try:
                    # Create MQTT message
                    mqtt_message = json.dumps({"address": eui64, "retrans": retrans, "mode": mode, "counter": counter, "temp": t, "humidity": h, "pressure": p, "rssi": rssi})

                    # Send MQTT message
                    self.mqtt_client.send_message(mqtt_topic, mqtt_message)
                    logger.info(mqtt_message)
                except:
                    logger.error("program: Error sending MQTT packet.")

    def stop(self):
        print("Stopping program at port={} with baudrate={}.".format(self.port, self.baudrate))

        super().stop()


def signal_handler(sig, frame):
    global finished
    finished = True

def program(baudrate = None):
    global finished

    motes = []

    # Create MQTT client
    mqtt_client = MqttClient.MqttClient(mqtt_address, mqtt_port)
    mqtt_client.start()

    serial_ports = MoteSerial.serial_ports()

    for serial_port in serial_ports:
        m = MoteSerialImplementation(port = serial_port, baudrate = baudrate, mqtt_client = mqtt_client)
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
    parser.add_argument("-b", "--baudrate", type=int, default=115200)

    # Parse arguments
    args = parser.parse_args()

    # Execute program
    program(baudrate = args.baudrate)
    
if __name__ == "__main__":
    main()
