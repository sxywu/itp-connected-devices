/*
 * Adapts TFT ST7789 example from:
 * https://github.com/tigoe/display-examples/blob/main/TFT_Examples/ST7789/ST7789_Text_Example/ST7789_Text_Example.ino
 * As well as OLED SSD1306 example:
 * https://github.com/tigoe/display-examples/blob/main/OLED_Examples/SSD1306_OLED_Example/SSD1306_OLED_Example.ino
 */

#include <Wire.h>
// load Adafruit libraries
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
 
const int SCREEN_WIDTH = 128; // OLED display width, in pixels
const int SCREEN_HEIGHT = 64; // OLED display height, in pixels

// TFT control pins:
// setting CS to -1 because of MakerFocus 1.3" not having CS pin
const int TFT_CS =  -1;
const int TFT_RST = 9;
const int TFT_DC = 8;
// TFT screen size
const int TFT_WIDTH = 240;
const int TFT_HEIGHT = 240;
const int TFT_TEXT_WIDTH = 140;
 
// initialize the oled:
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT);
// initialize tft
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// offscreen buffer for TFT which has a really slow refresh rate
GFXcanvas1 canvas(TFT_WIDTH - TFT_TEXT_WIDTH, TFT_HEIGHT);

// font colors
int fontColor = 0x6677FF;  // light blue
int bgColor = 0x0;         // black

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

  // initialize tft with SPI_MODE3 bc device doesn't have CS pin
  tft.init(TFT_WIDTH, TFT_HEIGHT, SPI_MODE3);
  tft.setRotation(2);
  
  oled.setTextSize(2);
  oled.setTextColor(SSD1306_WHITE);

  // clear the tft:
  tft.fillScreen(bgColor);
  // set the text color to teal:
  tft.setTextColor(fontColor);
  tft.setTextSize(3);
  canvas.setTextSize(3);
  
  // print unchanging text
  tft.setCursor(0, 30);
  tft.println("secs:");
  tft.println("sensor:");
  
  Serial.println("oled is good to go");
  Serial.println("tft is good to go");
}

void loop() {
  int sensorReading = analogRead(A0);

  // first display OLED
  oled.clearDisplay();

  oled.setCursor(0, 0);
  oled.print("secs:");
  oled.println(millis() / 1000);
  
  oled.println("sensor:");
  oled.println(sensorReading);

  oled.display();

  delay(10);

  // then TFT
  // clear the canvas with a rect in the background color:
  canvas.fillRect(0, 0, canvas.width(), canvas.height(), bgColor);
  // move the cursor to 0,30 (because fonts measure from their baseline):
  canvas.setCursor(0, 30);
  // print the seconds:
  canvas.println(millis() / 1000);
  canvas.println(sensorReading);
  // update the tft with the canvas:
  tft.drawBitmap(TFT_TEXT_WIDTH, 0, canvas.getBuffer(),
                     canvas.width(), canvas.height(),
                     fontColor, bgColor);

  delay(10);
}
