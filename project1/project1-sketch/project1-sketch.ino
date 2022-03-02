// accelerometer
#include <Arduino_LSM6DS3.h>
#include <ArduinoBLE.h>

BLEService orbService("E92AD644-3432-4089-A166-718422C49E41"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("E92AD644-3432-4089-A166-718422C49E41", BLERead | BLENotify | BLEWrite);

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

  // BLUETOOTH
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("orb-light");
  BLE.setAdvertisedService(orbService);

  // add the characteristic to the service
  orbService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(orbService);

  // set the initial value for the characeristic:
  switchCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");
  
}

void loop() {
  float x, y, z;

  // poll for BLE events
  BLE.poll();
  
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);

    int valueChanged = 0;
    // if z-axis is above threshold, then turn on light
    // but only if it wasn't already on
    if (z > accThreshold && !ledState) {
      ledState = 1;
      valueChanged = 1;
    } else if (z < accThreshold && ledState) {
      ledState = 0;
      valueChanged = 1;
    }

    if (valueChanged) {
      switchCharacteristic.writeValue(ledState);
      digitalWrite(ledPin, ledState);
    }
  }
}
