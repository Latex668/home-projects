#include <U8g2lib.h>
#include <Wire.h>

#define WIRE Wire
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

struct Vector{
  int x;
  int y;
}

void setup() {
  // put your setup code here, to run once:
  WIRE.begin(14,12);
  Serial.begin(115200);
  u8g2.begin();
  delay(2000);
}

void connectPoints(Vector)

void loop() {
  // put your main code here, to run repeatedly:
  u8g2.clear();
  u8g2.drawLine(0,0,0,64);
  u8g2.sendBuffer();
  delay(1000);
}
