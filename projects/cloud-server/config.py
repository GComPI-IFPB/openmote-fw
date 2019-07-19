mqtt_config = {
    "mqtt_id": "cloud-server",
    "mqtt_address": "34.240.186.196",
    "mqtt_port": 1883,
    "mqtt_subscribe_topics": ["test"],
    "mqtt_publish_topic": "test1"
}

packet_config = {
    "timeout_ms": 2000,
    "all_keys": {"pan_id", "node_id", "retry", "mode", "pkt_count", "temp", "rhum", "pres", "lght", "cca", "rssi", "gw_id"},
    "keep_keys": ["node_id", "pkt_count", "temp", "rhum", "pres", "lght", "rssi"]
}

calibration_config = {
    "use_calibration": False
}

calibration_values = {
}
