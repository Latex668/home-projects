#include <Adafruit_GFX.h>               // Adafruit LCD Graphics library
#include <Adafruit_ST7735.h>            // ST7735 LCD Display library
#include <SPI.h>                        // Arduino SPI and I2C library

// pin definition for the 1.8", 128x160 ST7735 LCD Display on Raspberry Pico
// These can be connected to any GP Pin not in use
#define TFT_CS      9       // using SPI1 CSn
#define TFT_RST     8
#define TFT_DC      7 // A0

// Used for SPI connectivity on SPI1
#define TFT_SCK     10
#define TFT_MOSI    11 // SDA
// #define TFT_MISO         // not used with this display

// Setup the ST7735 LCD Display and variables
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);
// Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int textxoffset = 5;     // move 5 pixels across to avoid the box
int textyoffset = 7;     // move 7 pixels down to avoid the box

int tft_line1 = 0;


void setup() {
  // put your setup code here, to run once:
  tft.initR(INITR_BLACKTAB);                          // initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);                       // Fill the screen with Black
  tft.setRotation(1);                                 // Set rotation to Portrait mode (set to '1' for Landscape)
  tft.setTextWrap(false);                             // By default long lines of text are NOT wrapped at the end of the line, set to 'TRUE' for wrapping the text

  // Draw the 1.8" LED display layout using 24 bit colour (limitation of the ST7735S library)
  // tft.drawRect(0, 0, 159, 128, ST7735_CYAN);       // Draw a Rectangle  x,y, across, down for the Colour Values
  tft.drawRoundRect(0, 0, 83, 31, 10, ST7735_CYAN);   // Draw a Rectangle  x,y, across, down with corner & Colour Values
  tft.drawRoundRect(0, 35, 160, 93, 10, ST7735_CYAN); // Draw a Rectangle  x,y, across, down with corner & Colour Values
  tft.drawLine(80, 65, 80, 110, ST7735_CYAN);         // Draw a mid-line  x1,y1, x2, y2 for the Colour Values


  tft.setTextColor(ST7735_GREEN, ST7735_BLACK);
  tft.setTextSize(1);                                 // Set the text size to 1
  tft.setCursor(textxoffset + 54,textyoffset + 1);
//   tft.print("Test");


}

void loop() {
  // put your main code here, to run repeatedly:

}