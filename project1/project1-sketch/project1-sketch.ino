// accelerometer
#include <Arduino_LSM6DS3.h>
#include <ArduinoBLE.h>
#include "name.h"

BLEService orbService("E92AD644-1000-4089-A166-718422C49E41"); // BLE LED Service

// central to turn orb light on/off
BLEByteCharacteristic remoteCharacteristic("E92AD644-0001-4089-A166-718422C49E41", BLERead | BLEWrite);
// this orb telling central whether it's turned on/off
BLEByteCharacteristic localCharacteristic("E92AD644-0002-4089-A166-718422C49E41", BLERead | BLENotify);

#define ledPin 12

int localState = 0; // current led state controlled locally
int remoteState = 0; // current led state controlled remotely
float accThreshold = 0.6;

// smoothing code from
// https://github.com/pyeseul/intangible-interaction/blob/master/Spring2022/class3/proximity_average/proximity_average.ino
const int numReadings = 20;  // adjust the number depending on how many values you want to average
float readings[numReadings];  // the raw proximity readings from the sensor
int readIndex = 0;  // the index of the current reading
float total = 0;  // the total to be divided by numReadings

void setup() {
  Serial.begin(9600);

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
  BLE.setLocalName(orbID);
  Serial.println(orbID);
  BLE.setAdvertisedService(orbService);

  // add the characteristic to the service
  orbService.addCharacteristic(remoteCharacteristic);
  orbService.addCharacteristic(localCharacteristic);

  // add service
  BLE.addService(orbService);

  // set the initial value for the characeristic:
  localCharacteristic.writeValue(0);
  remoteCharacteristic.writeValue(0);

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

    z = averageReadings(z);                                            
    int localOn = z > accThreshold;
    int localChanged = localOn && !localState || !localOn && localState;
    int remoteOn = remoteCharacteristic.value();
    int remoteChanged = remoteOn && !remoteState || !remoteOn && remoteState;

//    Serial.print(localOn);
//    Serial.print("\t"); 
//    Serial.print(localChanged);
//    Serial.print("\t");
//    Serial.println(remoteOn);

    // if local has changed, notify remote
    if (localChanged) {
      localCharacteristic.writeValue(localOn);
    }

    // figure out LED state
    if (localOn) {
      // if locally interacted with, light it up
      analogWrite(ledPin, 255);
    } else if (remoteOn) {
      // if not locally on but remote says to turn on
      // then do sine wave
      float time = 0.25 * millis() / 1000.0;
      int val = 128.0 + 128 * sin( time * 2.0 * PI  );
      analogWrite(ledPin, val);
    } else {
      // else local AND remote off
      analogWrite(ledPin, 0);
    }
    
    localState = localOn;
    remoteState = remoteOn;
  }
}

float averageReadings(float val) {
  
  total = total - readings[readIndex];  // subtract the last reading in the array
 
  readings[readIndex] = val;  // save the latest value from the sensor to the array's [readIndex]th slot
  total = total + readings[readIndex];  // add the latest proximity value to the total  
  readIndex = readIndex + 1;  // advance to the next position in the array

  if (readIndex >= numReadings) {
    readIndex = 0;  // if we're at the end of the array, reset readIndex to 0
  }

  return total / numReadings;  // calculate the average
}
