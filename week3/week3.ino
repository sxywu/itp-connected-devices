#include <Keyboard.h>
#include <Arduino_APDS9960.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // initialize gesture sensor
  if (!APDS.begin()) {
    Serial.println("Error initializing APDS-9960 sensor!");
  }
  Serial.println("Detecting gestures ...");

  // initialize control over the keyboard:
  Keyboard.begin();
}

void loop() {
  if (APDS.gestureAvailable()) {
    // a gesture was detected, read and print to Serial Monitor
    int gesture = APDS.readGesture();

    if (gesture == GESTURE_UP || gesture == GESTURE_RIGHT) {
      Serial.println("Detected RIGHT gesture");
      Keyboard.press(KEY_LEFT_ARROW); // for book page flipping go the other way

    } else if (gesture == GESTURE_DOWN || gesture == GESTURE_LEFT) {
      Serial.println("Detected LEFT gesture");
      Keyboard.press(KEY_RIGHT_ARROW); // for book page flipping go the other way
    }
    delay(10);
    Keyboard.releaseAll();

    delay(100);
  }
}
