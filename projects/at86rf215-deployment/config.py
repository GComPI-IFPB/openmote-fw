serial_config = {
    "serial_devices": 3,
    "serial_baudrate": 1267200,
    "serial_timeout": 0.1
}

mqtt_config = {
    "mqtt_address": "34.244.230.156",
    "mqtt_port": 1883,
}

network_config = {
    "pan_id": "cafe"
}

message_config = {
    "message_id"        : "edison",
    "message_topic"     : "test"
    "message_fields"    : ["pan_id", "node_id", "retry", "mode", "pkt_count", "temp", "rhum", "pres", "lght", "cca", "rssi"],
    "message_structure" : ">2s8sbbIhhhhbb"
}
