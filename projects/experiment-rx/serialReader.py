from influxdb import InfluxDBClient
from datetime import datetime
from struct import unpack
import serial
import sys

serialPort = serial.Serial(port=sys.argv[1], baudrate=115200, timeout=0.1)

clientTest = InfluxDBClient(
    host='127.0.0.1', port=8086, database='openmoteTest')
clientFail = InfluxDBClient(host='127.0.0.1', port=8086, database='fail')

c = 0

while True:
    message = serialPort.read(1024)

    if len(message) >= 37:
        eui48, counter, txMode, txCounter, csma_retries, csma_rssi = unpack(
            ">6sQbbBb", message[1:19])
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
                    "csma_retries": csma_retries,
                    "csma_rssi": csma_rssi
                }
            }
        ]

        print(data)

        done = clientTest.write_points(data, protocol="json")

    elif len(message) > 0:
        data = [
            {
                "measurement": "failData",
                "tags": {
                    "location": "A"
                },
                "fields": {
                    "data": str(message),
                    "counter": c,
                    "packetSize": len(message)
                }
            }
        ]
        c += 1
        print(data)

        done = clientFail.write_points(data, protocol="json")
