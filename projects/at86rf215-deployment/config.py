serial_config = {
    "serial_devices": 4,
    "serial_baudrate": 1267200,
    "serial_timeout": 0.1
}

mqtt_config = {
    "mqtt_id": "edison",
    "mqtt_address": "34.244.230.156",
    "mqtt_port": 1883,
    "mqtt_topic": "test"
}

message_config = {
    "message_fields"    : ["node_id", "retry", "mode", "packet_count", "temperature", "humidity", "pressure", "light", "rssi"],
    "message_structure" : ">8sbbIhhhhb"
}
