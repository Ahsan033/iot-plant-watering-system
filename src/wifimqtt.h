#ifndef WIFIMQTT_H
#define WIFIMQTT_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "credentials.h"
extern byte FAN_PIN;
// MQTT
String clientID = "ESP32-";
const char *mqtt_server = "192.168.1.6";
const char *mqtt_user = "ahsan";
const char *mqtt_password = "34501";

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    String newClientID = clientID + String(random(0xffff), HEX);
    if (client.connect(newClientID.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("Connected to MQTT");
      client.subscribe("fan"); // Subscribe to the topic
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char *topic, byte *message, unsigned int length) {
  String messageTemp;
  for (unsigned int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(messageTemp);
  if (messageTemp == "true") {
    digitalWrite(FAN_PIN, HIGH);
  } else if (messageTemp == "false") {
    digitalWrite(FAN_PIN, LOW);
  }
}

#endif
