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

import Serial
import MqttClient

mqtt_address = "34.244.230.156"
mqtt_port    = 1883
mqtt_topic   = "test"

finished = False

timeout = 0.1

def signal_handler(sig, frame):
    global finished
    finished = True

def program(port = None, baudrate = None):
    global finished

    # Create and start Serial manager
    serial = Serial.Serial(name = port, baudrate = baudrate, timeout = timeout)
    serial.start()

    # Create MQTT client
    mqtt = MqttClient.MqttClient(mqtt_address, mqtt_port)
    mqtt.start()

    print("Starting program at port {} with bauds {}.".format(port, baudrate))

    # Repeat until finish condition
    while (not finished):
        # Try to receive a Serial message
        message, length = serial.receive(timeout = timeout)

        # If we received a message
        if (length > 0):
            try:
                message = bytearray(bytes(message))
                eui48, counter, t, h, p, l, rssi = struct.unpack('>6sIhhhhb', message)
                t = t/10.0
                h = h/10.0
                p = p/10.0
                l = l/10.0
                print("Counter={}, Temperature={}, Humidity={}, Pressure={}, RSSI={}".format(counter, t, h, p, rssi))
            except:
                logger.error("program: Error unpacking.")

            try:
                # Create MQTT message
                mqtt_message = json.dumps({"address": eui48, "counter": counter, "temp": t, "humidity": h, "pressure": p, "rssi": rssi})

                # Send MQTT message
                mqtt.send_message(mqtt_topic, mqtt_message)
            except:
                logger.error("program: Error sending MQTT packet.")
            
    if (finished):
        # Stop the serial port
        serial.stop()

def main():
    # Set-up logging back-end
    logging.basicConfig(level=logging.DEBUG)

    # Set up SIGINT signal
    signal.signal(signal.SIGINT, signal_handler)

    # Create argument parser
    parser = argparse.ArgumentParser(description="")
    parser.add_argument("-p", "--port", type=str, required=True)
    parser.add_argument("-b", "--baudrate", type=int, default=115200)

    # Parse arguments
    args = parser.parse_args()

    # Execute program
    program(port = args.port, baudrate=args.baudrate)
    
if __name__ == "__main__":
    main()
