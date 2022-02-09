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
// canvas size
const int padding = 30;
const int CANVAS_SIZE = TFT_WIDTH - 2 * padding;
 
// initialize the oled:
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT);
// initialize tft
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// offscreen buffer for TFT which has a really slow refresh rate
GFXcanvas1 canvas(CANVAS_SIZE, CANVAS_SIZE);

// colors
int fontColor = 0x6677FF;  // light blue
int bgColor = 0x0;         // black

// array for remembering past values
const int bandWidth = 6;
const int numPoints = CANVAS_SIZE / bandWidth;
int values[numPoints];
int valueIndex = 0;
int prevValue = 0;

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
  tft.setCursor(0, 0);
  tft.print("sensor");
  tft.setCursor(165, 215);
  tft.print("secs");

  // print tft chart lines
  tft.drawFastVLine(padding - 1, padding, CANVAS_SIZE, fontColor);
  tft.drawFastVLine(padding - 2, padding, CANVAS_SIZE, fontColor);
  tft.drawFastHLine(padding, padding + CANVAS_SIZE, CANVAS_SIZE, fontColor);
  tft.drawFastHLine(padding, padding + CANVAS_SIZE + 1, CANVAS_SIZE, fontColor);
  
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

  // if we need to wrap around in the array, remember value we're about to replace
  prevValue = values[valueIndex];
  // map sensor reading to screen position, store in values array
  values[valueIndex % numPoints] = map(sensorReading, 0, 1023, CANVAS_SIZE, 0);

  // starting point for looping through the array
  int start = max((valueIndex - numPoints) % numPoints, 0);

  // loop through and draw past values
  for (int i = 0; i < numPoints; i += 1) {
    if (i > valueIndex) continue;
    
    int x0 = (i - 1) * bandWidth;
    int x = i * bandWidth;

    int startIndex = (start + i) % numPoints;
    int y0 = values[startIndex - 1];
    int y = values[startIndex];
    
    canvas.drawLine(x0, y0, x, y, fontColor);
  }
  
  // update the tft with the canvas:
  tft.drawBitmap(padding, padding, canvas.getBuffer(),
                     canvas.width(), canvas.height(),
                     fontColor, bgColor);

  // increment index
  valueIndex += 1;
  
  delay(10);
}
