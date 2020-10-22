from influxdb import InfluxDBClient
from datetime import datetime
from struct import unpack
import serial
import sys
import os

serialPort = serial.Serial(port=sys.argv[1], baudrate=115200, timeout=0.1)

file = open(f'data{serialPort}.csv', "a")

client = InfluxDBClient(host='127.0.0.1', port=8086, database='testeOpenmote')

while True:
    message = serialPort.read(1024)

    if len(message) >= 37:
        eui48, counter, txMode, txCounter = unpack(">6sQbb", message[1:17])
        rssi, _ = unpack(">bb", message[33:35])

        data = [
            {
                "measurement": "transmissionData",
                "tags": {
                    "openmoteID": str(eui48),
                    "location": "A"
                },
                "fields": {
                    "counter": counter,
                    "txMode": txMode,
                    "txCounter": txCounter,
                    "rssi": rssi,
                }
            }
        ]

        print(data)

        done = client.write_points(data, protocol="json")

        data = f'{str(datetime.now())}, {str(eui48)}, {str(counter)}, {str(txMode)}, {str(txCounter)}, {str(rssi)}, {str(len(message))}\n'
        file.write(data)
