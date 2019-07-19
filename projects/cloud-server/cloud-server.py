'''
@file       clour-server.py
@author     Pere Tuset-Peiro  (peretuset@openmote.com)
@version    v0.1
@date       July, 2019
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

import config
import json
import logging
import queue
import signal
import threading
import time
import hashlib

from dataclasses import dataclass, field

import MqttClient

finished = False

logger = logging.getLogger(__name__)

def signal_handler(sig, frame):
    global finished
    finished = True

@dataclass
class Packet:
    timeout: int
    replicas: list = field(default_factory=list)

class MqttMultiplexer(threading.Thread):
    def __init__(self):
        logger.debug('init: Creating the MqttMultiplexer object.')

        # Call thread constructor
        threading.Thread.__init__(self)

        # Store the MQTT server, port and timeout
        self.mqtt_id      = config.mqtt_config['mqtt_id']
        self.mqtt_address = config.mqtt_config['mqtt_address']
        self.mqtt_port    = config.mqtt_config['mqtt_port']
        self.mqtt_subscribe_topics  = config.mqtt_config['mqtt_subscribe_topics']
        self.mqtt_publish_topic = config.mqtt_config['mqtt_publish_topic']
        self.mqtt_client  = None

        # Calibration configuration
        self.use_calibration = config.calibration_config['use_calibration']

        # Object properties
        self.packet_timeout_ms = config.packet_config['timeout_ms']

        # Message queue
        self.message_queue = queue.Queue()

        # Terminate thread event
        self.is_finished = threading.Event()

        # Dictionary that holds the elements
        self.elements = {}
        self.all_keys = config.packet_config['all_keys']
        self.keep_keys = config.packet_config['keep_keys']

    def start(self):
        logger.debug("start: Starting the MqttMultiplexer.")

        # Create MqttClient
        self.mqtt_client = MqttClient.MqttClient(address=self.mqtt_address, port=self.mqtt_port)

        # Call thread start
        threading.Thread.start(self)

    def run(self):
        logger.debug("run: Running the MqttMultiplexer.")

        # Register MQTT topics
        for topic in self.mqtt_subscribe_topics:
            self.mqtt_client.add_topic(topic=topic, callback=self.on_message)

        # Start MQTT client
        self.mqtt_client.start()

        # Run until finish condition is set
        while (not self.is_finished.isSet()):
            keys_expired = []

            # Number of elements
            length = len(self.elements.keys())

            # If there are items to be processed
            if (length > 0):
                # Calculate current time
                current_time = time.time() * 1000

                # Go through all dictionary elements
                for key in self.elements.keys():
                    # If the element has expired
                    if (current_time > self.elements[key].timeout):
                        # Add key to expired keys
                        keys_expired.append(key)

            # Consolidate all elements that have expired
            for key in keys_expired:
                # Recover element from dictionary
                p = self.elements[key]

                # Consolidate element (i.e., remove duplicates)
                payload = self.__consolidate_replicas(p)

                # If the packet exists, send it
                if (payload is not None):
                    self.mqtt_client.send_message(topic=self.mqtt_publish_topic, message=payload)

            # Remove all keys that have expired
            for key in keys_expired:
                # Calculate number of pcakets
                length = len(self.elements[key].replicas)
                logger.debug("Removing key={} with {} replicas from dictionary.".format(key, length))
                del self.elements[key]

            time.sleep(0.01)

        # Stop the MQTT thread
        self.mqtt_client.stop()

        logger.debug("run: Stopped the MqttMultiplexer.")

    def stop(self):
        logger.debug("stop: Stopping the MqttMultiplexer.")

        # Terminates the thread
        self.is_finished.set()

    def on_message(self, topic=None, message=None):
        logger.debug("on_message: Received message.")

        try:
            # Recover unique identifier, timeout and payload
            key, timeout, payload = self.__create_packet(message)

            # If key exists in dictionary
            if key in self.elements.keys():
                # Append the payload to the list
                self.elements[key].replicas.append(payload)
            else:
                # Create a new packet with timeout and payload
                p = Packet(timeout=timeout)
                p.replicas.append(payload)

                # Add the packet to the dictionary
                self.elements[key] = p
        except Exception as e:
            logger.error(e)
            logger.error("on_message: Error parsing message!")

    def __create_packet(self, message=None):
        # Get packet timestamp
        timestamp_ms = time.time() * 1000.0
        packet_timeout_ms = timestamp_ms + self.packet_timeout_ms

        # Convert packet to JSON
        message = json.loads(message)

        t = message['node_id'].encode() + str(message['pkt_count']).encode()

        # Calculate unique identifier
        uid = hashlib.md5(t).hexdigest()

        # Return list
        return (uid, packet_timeout_ms, message)

    def __consolidate_replicas(self, packet=None):
        # Count number of replicas
        replicas = len(packet.replicas)

        # If there are replicas, process them
        if (replicas > 0):
            output = {}

            # Number of replicas
            output['replicas'] = replicas

            # Use first replica to extract sensor data
            payload = packet.replicas[0]

            # Keep only required fields
            for key in self.keep_keys:
                output[key] = payload[key]

            # Compensate sensors offset
            if (self.use_calibration):
                output = self.__compensate_offset(message=output)

            # Convert payload to JSON string
            payload = json.dumps(output)
        else:
            payload = None

        return payload

    def __compensate_offset(self, message=None):
            node_id = message['node_id']

            temp_offset = config.calibration_values[node_id]['temp']
            rhum_offset = config.calibration_values[node_id]['rhum']
            pres_offset = config.calibration_values[node_id]['pres']
            lght_offset = config.calibration_values[node_id]['lght']

            message['temp'] = message['temp'] - temp_offset
            message['rhum'] = message['rhum'] - rhum_offset
            message['pres'] = message['pres'] - pres_offset
            message['lght'] = message['lght'] - lght_offset

            return message

def main():
    global finished

    # Set-up logging back-end
    logging.basicConfig(level=logging.DEBUG)

    # Set up SIGINT signal
    signal.signal(signal.SIGINT, signal_handler)

    # Create MQTT multiplexer
    multiplexer = MqttMultiplexer()
    multiplexer.start()

    # Repeat until finish condition
    while (not finished):
        time.sleep(0.1)

    # Stop the multiplexer
    multiplexer.stop()
    
if __name__ == "__main__":
    main()
