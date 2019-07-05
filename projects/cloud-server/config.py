mqtt_config = {
    "mqtt_id": "cloud-server",
    "mqtt_address": "34.244.230.156",
    "mqtt_port": 1883,
    "mqtt_subscribe_topics": ["test"],
    "mqtt_publish_topic": "test1"
}

packet_config = {
    "timeout_ms": 2000,
    "all_keys": {"pan_id", "node_id", "retry", "mode", "pkt_count", "temp", "rhum", "pres", "lght", "cca", "rssi", "gw_id"},
    "keep_keys": ["temp", "node_id", "rhum", "pres", "lght", "rssi"]
}
