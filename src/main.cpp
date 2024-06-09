#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_TSL2561_U.h>
#include <ArduinoJson.h>
#include "wifimqtt.h"

// Define your LED pin
byte FAN_PIN = 18;
Adafruit_BMP085 bmp;
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

unsigned long previousMillis = 0; // Initialize to 0 for proper timing
const long interval = 3000;
float temperature, pressure, altitude, lux;

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

void configureSensor() {
  tsl.enableAutoRange(true);            // Auto-gain
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);  // Fastest
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  // Medium
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  // Longest

  Serial.println("Sensor configured");
}

void sensorReading() {
  Serial.print("Temperature = ");
  temperature = bmp.readTemperature();
  Serial.print(temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  pressure = bmp.readPressure();
  Serial.print(pressure);
  Serial.println(" Pa");

  Serial.print("Altitude = ");
  altitude = bmp.readAltitude();
  Serial.print(altitude);
  Serial.println(" meters");

  sensors_event_t event;
  tsl.getEvent(&event);

  if (event.light) {
    Serial.print("Light = ");
    lux = event.light;
    Serial.print(lux);
    Serial.println("lux");
  } else {
    Serial.println("Sensor overload");
  }
}

void sendSensorValuesMQTT(){
  DynamicJsonDocument doc(1024);
  doc["temperature"] = temperature;
  doc["pressure"] = pressure;
  doc["altitude"] = altitude;
  doc["lux"] = lux;
  char buffer[256];
  serializeJson(doc, buffer);
  client.publish("weatherstation", buffer);

  //  Print the JSON string to Serial for debugging purposes
  Serial.print("Sent JSON: ");
  Serial.println(buffer);
}

void setup() {
  Serial.begin(115200); // Initialize serial communication
  pinMode(FAN_PIN, OUTPUT);

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  if (!tsl.begin()) {
    Serial.print("No TSL2561 sensor found. Check wiring!");
    while (1);
  }

  configureSensor();
  
  // Call the connectAP function to connect to WiFi
  connectAP();

  // Initialize MQTT client
  client.setServer(mqtt_server, 1884); // Use default MQTT port 1883
  client.setCallback(callback);
}

void loop() {
  // Check MQTT connection and reconnect if necessary
  if (!client.connected()) {
    reconnect();
  }

  // Maintain MQTT connection
  client.loop();

  // Blink LED every second
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    sensorReading();
    sendSensorValuesMQTT();
  }
  // digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  // client.publish("tonodered", "hellofromesp32");
}



