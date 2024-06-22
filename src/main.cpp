#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include "globals.h"
#include "wifimqtt.h"
#include "ultrasonic.h"

const long interval = 3000;

void getData() {
  client.publish("pumpinfo", "give me information");
  Serial.println("MQTT publish pumpinfo - now wait");
  state = WAIT;
}

void sendNewPumpInfos()
{
  // Create a JsonDocument object with enough capacity
  StaticJsonDocument<200> doc;

  // Populate the JsonDocument
  doc["distance1"] = distance1;
  doc["distance2"] = 22; // Assuming distance2 is an integer
  
  // Serialize the JsonDocument to a char buffer
  char buffer[256];
  serializeJson(doc, buffer);

  // Assuming client is your MQTT client instance
  client.publish("waterDistance", buffer); // Publish the JSON data

  // For debugging, print to serial monitor
  Serial.println("Distance transmitted");
}

void startPumping() {
  unsigned long currentMillis = millis();
  digitalWrite(relaisglucosfeder, LOW);  // Assuming active-low relay
  if (currentMillis - previousMillis >= interval) {
    digitalWrite(relaisglucosfeder, HIGH);  // Turn off the relay
    Serial.println("Pump is off");
    
    getDistance();
    
    Serial.print(distance1);
    Serial.println( "cm");
    sendNewPumpInfos();
    state = TIMEFORSLEEP;
  }
}

void connectAP() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    attempts++;
    if (attempts > 30) {
      Serial.println("WiFi connection failed. Restarting...");
      ESP.restart();
    }
  }
  Serial.println("WiFi connected!");
}

void setup() {
  Serial.begin(115200);
  pinMode(relaisglucosfeder, OUTPUT);
  pinMode(trigger1, OUTPUT);
  pinMode(echo1, INPUT);

  state = GETDATA;
  
  connectAP();
  
  client.setServer(mqtt_server, 1884);  
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  switch (state) {
    case GETDATA:
      getData();
      break;
    case PUMPING:
      startPumping();
      break;
    case WAIT:
  
      break;
    case TIMEFORSLEEP:
    Serial.println("time for going to sleep ZzzzzZZZZ");
    client.publish("recorddeepsleep", "ZzzzzZZ");
    delay(1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH,ESP_PD_OPTION_OFF);
    state = GETDATA;
    delay(1000*10);
      break;
  }
}
