#include "secrets.h"

// MQTT Broker Config
#define MQTT_HOST IPAddress(10, 0, 1, 200)
#define MQTT_PORT 1883
#define MQTT_USER ""
#define MQTT_PASS ""
#define MQTT_RECONNECT_TIME 2 // in seconds

// MQTT Client Config
#define MQTT_CLIENT_ID "button"
// change box number for each box!
#define MQTT_UNIQUE_ID ""
#define MQTT_TOPIC_MONITOR MQTT_CLIENT_ID MQTT_UNIQUE_ID "/monitor"
#define MQTT_TOPIC_MONITOR_QoS 0 // Keep 0 if you don't know what it is doing
#define MQTT_TOPIC_STATE MQTT_CLIENT_ID MQTT_UNIQUE_ID "/state"
#define MQTT_TOPIC_STATE_QoS 0 // Keep 0 if you don't know what it is doing
#define MQTT_TOPIC_LEDS MQTT_CLIENT_ID MQTT_UNIQUE_ID "/leds"
#define MQTT_TOPIC_LEDS_QoS 0 // Keep 0 if you don't know what it is doing

// WiFi Config
// change box number for each box!
#define WIFI_CLIENT_ID "button"
#define WIFI_RECONNECT_TIME 2 // in seconds

// Wifi optional static ip (leave client ip empty to disable)
#define WIFI_STATIC_IP    0 // set to 1 to enable static ip
#define WIFI_CLIENT_IP    IPAddress(10, 0, 0, 221)
#define WIFI_GATEWAY_IP   IPAddress(10, 0, 0, 1)
#define WIFI_SUBNET_IP    IPAddress(255, 255, 255, 0)
#define WIFI_DNS_IP       IPAddress(10, 0, 0, 1)

#define OTA_PATCH 0 // Set to 0 if you don't want to update your code over-the-air
#define OTA_PASS "set_ota_password"
