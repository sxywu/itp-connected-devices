#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
 
const int SCREEN_WIDTH = 128; // OLED display width, in pixels
const int SCREEN_HEIGHT = 64; // OLED display height, in pixels
 
// initialize the display:
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);

void setup() {
  // initialize serial and wait for serial monitor to open:
  Serial.begin(9600);
  if (!Serial) delay(3000);
  // first parameter of begin() sets voltage source.
  // SSD1306_SWITCHCAPVCC is for 3.3V
  // second parameter is I2C address, which is
  // 0x3C, or 3D for some 128x64 modules:
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Display setup failed");
    while (true);
  }
  Serial.println("Display is good to go");
}

void loop() {
  display.clearDisplay();
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print("secs:");
  display.print(millis() / 1000);

  display.display();
}
