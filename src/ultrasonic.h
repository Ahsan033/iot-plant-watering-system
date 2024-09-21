#include <Arduino.h>

long distanceMeasure(int trigPin, int echoPin){
    long duration, distance = 0;
// Clear the trigPin by setting it LOW for 2 microseconds
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    // Trigger the sensor by setting the trigPin HIGH for 10 microseconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Read the echoPin, which returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);

    // Calculate the distance in centimeters
    distance = duration * 0.034 / 2;
    return distance;
}


void getDistance(){
    distance1 = distanceMeasure(trigger1, echo1);
    // distance2 = distanceMeasure(trigger2, echo2);
    // distance3 = distanceMeasure(trigger3, echo3);
}