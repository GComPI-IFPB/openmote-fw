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
    "keep_keys": ["node_id", "pkt_count", "temp", "rhum", "pres", "lght", "rssi"]
}

calibration_values = {
    "5565": {"temp": -0.775, "rhum":  4.601, "pres": -0.261, "lght": -0.013},
    "55ad": {"temp":  0.000, "rhum":  0.000, "pres":  0.000, "lght":  0.000},
    "62e5": {"temp":  0.000, "rhum":  0.000, "pres":  0.000, "lght":  0.000},
    "630a": {"temp": -0.312, "rhum":  1.918, "pres":  0.199, "lght":  0.117},
    "5626": {"temp": -0.026, "rhum":  1.441, "pres": -0.387, "lght": -0.613},
    "5653": {"temp":  0.672, "rhum": -2.708, "pres":  0.085, "lght":  0.447},
    "55b3": {"temp":  0.059, "rhum": -2.358, "pres": -0.251, "lght": -0.322},
    "55e4": {"temp": -0.138, "rhum":  0.491, "pres":  0.178, "lght":  0.147},
    "5578": {"temp": -0.176, "rhum":  0.451, "pres":  0.437, "lght":  0.237}
}