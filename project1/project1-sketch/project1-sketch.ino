// accelerometer
#include <Arduino_LSM6DS3.h>
#define ledPin 12

int ledState = 0;
float accThreshold = 0.8;

void setup() {
  Serial.begin(9600);
  while(!Serial);

  // led
  pinMode(ledPin, OUTPUT);
  
  // initialize accelerometer
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }
}

void loop() {
  float x, y, z;
  
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);

    // if z-axis is above threshold, then turn on light
    // but only if it wasn't already on
    if (z > accThreshold && !ledState) {
    Serial.println(z);
      digitalWrite(ledPin, HIGH);
      ledState = 1;
    } else if (z < accThreshold && ledState) {
      digitalWrite(ledPin, LOW);
      ledState = 0;
    }
  }
}
