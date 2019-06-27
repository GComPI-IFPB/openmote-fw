# -*- coding: utf-8 -*-

import logging
import threading
import time
import queue

import paho.mqtt.client as mqtt

from collections import OrderedDict

# Import logging configuration
logger = logging.getLogger(__name__)

class MqttClient(threading.Thread):
    def __init__(self, address = "127.0.0.1", port = 1883):
        logger.info('init: Creating the MQTTClient object.')

        # Call thread constructor
        threading.Thread.__init__(self)

        # Lock to wait for connection
        self.lock = threading.Lock()
        self.lock.acquire()

        # Message queue
        self.message_queue = queue.Queue()

        # Store the MQTT server, port and timeout
        self.mqtt_address = address
        self.mqtt_port    = port
        self.mqtt_timeout = 30

        self.mqtt_client = None
        self.mqtt_subscriptions = {}

        # Terminate thread event
        self.stop_event = threading.Event()

    def mqtt_on_connect(self, client, data, flags, rc):
        logger.info("mqtt_on_connect: connected.")
        # Release lock to signal connection
        self.lock.release()

    def mqtt_on_message(self, client, userdata, msg):
        logger.info("mqtt_on_message: message received.")
        topic = msg.topic
        payload = msg.payload.decode('utf-8')

        self.mqtt_subscriptions[topic](topic, payload)

    def start(self):
        logger.info("start: Starting the MQTTClient.")

        # Create MQTT client
        self.mqtt_client = mqtt.Client()
        self.mqtt_client.on_connect = self.mqtt_on_connect
        self.mqtt_client.on_message = self.mqtt_on_message

        # Call thread start
        threading.Thread.start(self)

    def run(self):
        logger.info("run: Running the MQTTClient.")

        # Connect the MQTT client to the broker
        self.mqtt_client.connect(self.mqtt_address, self.mqtt_port, self.mqtt_timeout)

        # Start the MQTT client
        self.mqtt_client.loop_start()

        # Wait for connection
        self.lock.acquire()

        while (not self.stop_event.isSet()):
            try:
                # Recover element from queue
                element = self.message_queue.get(block=True, timeout=0.1)

                # If we received an element
                if (element is not None):
                    # Recover topic and message from element
                    topic = element['topic']
                    message = element['message']

                    # Log the message that has been send
                    logger.info("run: Sending topic={}, message={}.".format(topic, message))

                    # Publish the JSON message to the MQTT broker
                    self.mqtt_client.publish(topic, message)
            except queue.Empty:
                pass

        # Stop the MQTT thread
        self.mqtt_client.loop_stop()

        logger.info("run: Stopped the MQTTClient.")

    def stop(self):
        logger.info("stop: Stopping the MQTTClient.")
        
        # Terminates the thread
        self.stop_event.set()

    def add_topic(self, topic=None, callback=None):
        logger.info("add_topic: Subscribed to topic={}.".format(topic))

        self.mqtt_client.subscribe(topic)

        self.mqtt_subscriptions[topic] = callback

    def send_message(self, mqtt_topic, mqtt_message):
        # Create message
        element = {"topic": mqtt_topic, "message": mqtt_message}

        # Send element to message queue
        self.message_queue.put(element)

        


    