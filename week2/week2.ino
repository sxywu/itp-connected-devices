#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
 
const int SCREEN_WIDTH = 128; // OLED display width, in pixels
const int SCREEN_HEIGHT = 64; // OLED display height, in pixels
 
// initialize the oled:
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT);

void setup() {
  // initialize serial and wait for serial monitor to open:
  Serial.begin(9600);
  if (!Serial) delay(3000);
  // first parameter of begin() sets voltage source.
  // SSD1306_SWITCHCAPVCC is for 3.3V
  // second parameter is I2C address, which is
  // 0x3C, or 3D for some 128x64 modules:
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("oled setup failed");
    while (true);
  }
  
  oled.setTextSize(2);
  oled.setTextColor(SSD1306_WHITE);
  Serial.println("oled is good to go");
}

void loop() {
  oled.clearDisplay();

  oled.setCursor(0, 0);
  oled.print("secs:");
  oled.print(millis() / 1000);

  oled.display();
}
