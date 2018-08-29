# -*- coding: utf-8 -*-
#!/usr/bin/python

import sys
sys.path.append("../../python")

import json
import logging
import os
import signal
import struct
import time

import Serial
import MqttClient
import Bootload

logger = logging.getLogger(__name__)

uart_name = 'COM22'
uart_speed = 115200

mqtt_address = "34.247.253.111"
mqtt_port    = 1883
mqtt_topic   = "test"

bsl_file     = "demo-rx.bin"

finished = False

def signal_handler(sig, frame):
    global finished
    finished = True

def program():
    global finished 

    bootload = Bootload.Bootload(uart_name, uart_speed, bsl_file)
    result = bootload.start()

    if (not result):
        sys.exit()

    time.sleep(2)

    # Create Serial manager
    serial = Serial.Serial(uart_name, uart_speed)
    serial.start()

    # Creat MQTT client
    mqtt = MqttClient.MqttClient(mqtt_address, mqtt_port)
    mqtt.start()

    while(not finished):
        # Try to receive a Serial message
        message, length = serial.receive()

        # If we received a message
        if (length > 0):
            logger.info("program: Received message with %d bytes.", length)

            # Try to decode message
            try:
                address, counter, t, h, p, rssi = struct.unpack('>6sIhhhb', message)
                address = address.hex()
                t = t / 10.0
                h = h / 10.0
                p = p / 10.0
            except:
                logger.error("program: Error unpacking.")

            try:
                # Create MQTT message
                mqtt_message = json.dumps({"address": address, "counter": counter, "temp": t, "humidity": h, "pressure": p, "rssi": rssi})

                # Send MQTT message
                mqtt.send_message(mqtt_topic, mqtt_message)
            except: 
                logger.error("program: Error sending MQTT packet.")

        # Check for finished condition
        if (finished):
            serial.stop()

        # Execute every 100 milliseconds
        time.sleep(0.1)

def main():
    # Set-up logging back-end
    logging.basicConfig(filename='demo-rx.txt', filemode='w', level=logging.DEBUG)

    # Set-up signal handler
    signal.signal(signal.SIGINT, signal_handler)

    # Run program
    program()
    
if __name__ == "__main__":
    main()
