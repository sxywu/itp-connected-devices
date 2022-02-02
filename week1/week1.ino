#include <Arduino_JSON.h>
#include <Encoder.h>

Encoder myEncoder(2, 3);

// set up pin numbers for echo pin and trigger pins:
const int trigPin = 11;
const int echoPin = 12;
const int encoderPin = 4;
const int fps = 16;
int prevDistance = 0;
int prevTurn = 0;

// JSON object
JSONVar outgoing;

void setup() {
  // set the modes for the trigger pin and echo pin:
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  // encoder's pushbutton
  pinMode(encoderPin, INPUT_PULLUP);
  // initialize serial communication:
  Serial.begin(9600);

  outgoing["time"] = 0;
  outgoing["sensor"] = 0;
  outgoing["button"] = 0;
  outgoing["turn"] = 0;
}

void loop() {
  // take the trigger pin low to start a pulse:
  digitalWrite(trigPin, LOW);
  // delay 2 microseconds:
  delayMicroseconds(2);
  // take the trigger pin high:
  digitalWrite(trigPin, HIGH);
  // delay 10 microseconds:
  delayMicroseconds(10);
  // take the trigger pin low again to complete the pulse:
  digitalWrite(trigPin, LOW);

  // listen for a pulse on the echo pin:
  long duration = pulseIn(echoPin, HIGH);
  // calculate the distance in cm.
  //Sound travels approx.0.0343 microseconds per cm.,
  // and it's going to the target and back (hence the /2):
  float distance = (duration * 0.0343) / 2;

  if (distance >= 1000) {
    // sometimes sensor will give a faulty reading when
    // object is too far or too close
    // so set it to previous reading
    distance = prevDistance;
  }
  // then constrain to 0 to 20cm
  distance = constrain(distance, 0, 20);

  outgoing["time"] = millis();
  outgoing["sensor"] = distance;
  outgoing["button"] = !digitalRead(encoderPin);
  long turn = myEncoder.read();
  outgoing["turn"] = turn - prevTurn;
  Serial.println(outgoing);

  prevDistance = distance;
  prevTurn = turn;  
  
  // a short delay between readings (16ms for 64fps):
  delay(fps);
}
