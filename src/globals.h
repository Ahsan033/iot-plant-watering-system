#ifndef globals_h
#define globals_h

#include <Arduino.h>

// Change the relay pin to a commonly used GPIO pin (e.g., GPIO 5)
const byte relaisglucosfeder = 5; 

// HC-SR04
const int trigger1 = 12;
const int echo1 = 14;

long distance1 = 0;

enum states {
  GETDATA,
  PUMPING,
  TIMEFORSLEEP,
  WAIT
} state;

unsigned long previousMillis = 0; 

#endif
