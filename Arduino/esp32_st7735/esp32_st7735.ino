#include <Adafruit_ST7735.h>

// Define pins
#define TFT_CS    5
#define TFT_RST   4
#define TFT_DC    2

// Create an instance of the ST7735 class
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize the display
  tft.initR(INITR_BLACKTAB); // For 1.8" displays, use INITR_144GREENTAB for 1.44"
  tft.setRotation(1); // Adjust orientation

  // Clear the screen with a color
  tft.fillScreen(ST77XX_BLACK); 

  // Display text
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 30);
  tft.println("Hello, ST7735!");
}

void loop() {
  // You can update the screen here if needed
}