#define ARDUINOJSON_USE_LONG_LONG 1

#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <stdlib.h>
#include "config.h"

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

// WiFiEventHandler wifiConnectHandler;
// WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;


void connectToMqtt() {
  Serial.println("[MQTT] Connecting to MQTT...");
  mqttClient.setClientId(WIFI_CLIENT_ID);
  mqttClient.setKeepAlive(5);
  mqttClient.setWill(MQTT_TOPIC_MONITOR,MQTT_TOPIC_MONITOR_QoS,true,"{\"alive\": false}");
  mqttClient.connect();
}

void connectToWifi()
{
  if (WiFi.status() == WL_CONNECTED)
    return;

  while (true)
  {
    unsigned int connectStartTime = millis();
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    WiFi.begin(WIFI_SSID);
    Serial.printf("Attempting to connect to SSID: ");
    Serial.printf(WIFI_SSID);
    while (millis() - connectStartTime < 10000)
    {
      Serial.print(".");
      delay(1000);
      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.println("connected to wifi");
        Serial.println(WiFi.localIP());
        wifiReconnectTimer.detach();
        connectToMqtt();
        return;
      }
    }
    Serial.println(" could not connect for 10 seconds. retry");
  }
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("[MQTT] Connected to MQTT!");

  mqttReconnectTimer.detach();

  Serial.print("Session present: ");
  Serial.println(sessionPresent);
//   uint16_t packetIdSub = mqttClient.subscribe(MQTT_TOPIC_LEDS, MQTT_TOPIC_LEDS_QoS);
//   Serial.print("Subscribing to ");
//   Serial.println(MQTT_TOPIC_LEDS);
  Serial.println("Sending alive message");
  mqttClient.publish(MQTT_TOPIC_MONITOR, MQTT_TOPIC_MONITOR_QoS, true, "{\"alive\": true}");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("[MQTT] Disconnected from MQTT!");

  if (WiFi.isConnected()) {
    Serial.println("[MQTT] Trying to reconnect...");
    mqttReconnectTimer.once(MQTT_RECONNECT_TIME, connectToMqtt);
  }
}

// void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
//   if (!strcmp(topic, MQTT_TOPIC_LEDS)) {
//     Serial.print("Leds message: ");
//     Serial.print(len);
//     Serial.print(" [");
//     Serial.print(topic);
//     Serial.print("] ");
//     for (int i = 0; i < len; i++) {
//       Serial.print((char)payload[i]);
//     }
//     Serial.println();

//     DynamicJsonDocument doc(50);
//     deserializeJson(doc, payload);
    // color = doc["color"];
    // master_state = doc["master_state"];
    // Serial.print("color: ");
    // Serial.println(color);
    // Serial.print("master_state: ");
    // Serial.println(master_state);
//   }
// }

// buttons

#define BTN_DEBOUNCE_TIME 50
#define MAX_IO 30 // max number of IO pins
#define BTN_IO_NUM 27 // specific IO pin for button

uint8_t lastBtnsState[MAX_IO]; 
uint8_t btnsState[MAX_IO];
unsigned long lastDebouncedTime[MAX_IO];

unsigned long debounceDelay = BTN_DEBOUNCE_TIME;


uint8_t buttons[] = { BTN_IO_NUM };

// Debounced check that button is indeed pressed
bool is_button_pressed(uint8_t btn_port) {
  uint8_t reading = digitalRead(btn_port);
  if (reading != lastBtnsState[btn_port]) {
    // reset the debouncing timer
    lastDebouncedTime[btn_port] = millis();
  }
  if ((millis() - lastDebouncedTime[btn_port]) > debounceDelay) {
    if (reading != btnsState[btn_port]) {
      btnsState[btn_port] = reading;
      if (btnsState[btn_port] == LOW) {
        Serial.println("Button pressed");
        Serial.println(btn_port);
        return true;
      }
    }
  }
  lastBtnsState[btn_port] = reading;

  return false;
}

void setup_buttons() {
  for (uint8_t i = 0; i < (sizeof(buttons)/sizeof(buttons[0])); i++) {
    pinMode(buttons[i], INPUT_PULLUP);   
  }

  // we are initializing an array of maximum number of ports
  for (uint8_t i = 0; i < MAX_IO; i++) {
    lastBtnsState[i] = LOW;
    btnsState[i] = LOW;
    lastDebouncedTime[i] = 0;
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println("Startup!");

  setup_buttons();

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
//   mqttClient.onMessage(onMqttMessage);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  if (MQTT_USER != "") {
    mqttClient.setCredentials(MQTT_USER, MQTT_PASS);
  }

  connectToWifi();
}

/**
 * Main loop.
 */
void loop() {

  bool button_pressed = is_button_pressed(BTN_IO_NUM);

  if (mqttClient.connected() && button_pressed) {
    StaticJsonDocument<128> button_msg;
    button_msg["state"] = button_pressed;
    char button_msg_buffer[100];
    serializeJson(button_msg, button_msg_buffer);
    mqttClient.publish(MQTT_TOPIC_STATE, MQTT_TOPIC_STATE_QoS, false, button_msg_buffer);
    Serial.print("Sending button message: ");
    serializeJson(button_msg, Serial);
  }

}
