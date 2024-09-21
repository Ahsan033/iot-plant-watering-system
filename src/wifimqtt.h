#ifndef WIFIMQTT_H
#define WIFIMQTT_H

#include <Arduino.h>
#include "globals.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "credentials.h"

// MQTT
String clientID = "ESP32-";
const char *mqtt_server = "192.168.1.7";
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
      client.subscribe("pumpStart"); // Subscribe to the topic
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
  if(String(topic) == "pumpStart")
  {
    Serial.print("message is : ");
    Serial.println(messageTemp);
    if(messageTemp == "1")
    {
      previousMillis = millis();
      state = PUMPING;
      Serial.println("Pumping...");
    }
    else if (messageTemp == "0")
    {
      state = TIMEFORSLEEP;
      Serial.println("sleeping.....");
    }
  }

}

#endif