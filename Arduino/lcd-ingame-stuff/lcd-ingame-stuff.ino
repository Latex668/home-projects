#include <ADAFRUIT

#define SDA 14
#define SCL 12

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Init Serial Comm");
  u8g2.begin();
  u8g2.setFont(u8g2_font_helvR12_tr);
  u8g2.setFontMode(0);
  u8g2.clearBuffer();
  u8g2.setCursor(0,16);
  u8g2.setDrawColor(2);
  u8g2.print("Hello!!!!");
  u8g2.sendBuffer();
}

void loop() {
  // put your main code here, to run repeatedly:

}
