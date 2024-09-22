
#include <SPI.h>
#include <Wire.h>
#include <XilkaGFX.h>
#include <XilkaI2c.h>
#include <XilkaSSD1306_128x64.h>
//#define USE_ADA_SOFT 1
//#define USE_ADA_HARD 1
//#define USE_XILKA_SOFT 1
#define USE_XILKA_HARD 1
#if defined(ARDUINO_ARCH_ESP8266)
#define USE_ESP8266_HARDWARE_CS 1
#endif
//#define USE_SYSTEM_RESET 1

#if defined(ARDUINO_ARCH_ESP8266)
/*
 * It looks as if you must have
 * a 4.7k pulldown resistor on GPIO15/PIN_CS
 * for SPI to work on the esp8266
 */
const int8_t PIN_MISO(12);
const int8_t PIN_MOSI(13);
const int8_t PIN_SCLK(14);
#if defined(USE_ESP8266_HARDWARE_CS)
const int8_t PIN_CS(-1);
#else
const int8_t PIN_CS(15);
#endif
const int8_t PIN_DC(2);
#if defined(USE_SYSTEM_RESET)
const int8_t PIN_RESET(-1);
#else
const int8_t PIN_RESET(16);
#endif
#else // defined(ARDUINO_ARCH_ESP8266)
const int8_t PIN_DC(6);
const int8_t PIN_CS(10);
const int8_t PIN_SCLK(13);
const int8_t PIN_MOSI(11);
const int8_t PIN_MISO(12);
#if defined(USE_SYSTEM_RESET)
const int8_t PIN_RESET(-1);
#else
const int8_t PIN_RESET(8);
#endif
#endif // defined(ARDUINO_ARCH_ESP8266)

#if defined(USE_ADA_SOFT)
#define IS_ADA 1
Adafruit_SSD1306 ssd(PIN_MOSI, PIN_SCLK, PIN_DC, PIN_RESET, PIN_CS);
#elif defined(USE_ADA_HARD)
#define IS_ADA 1
Adafruit_SSD1306 ssd(PIN_DC, PIN_RESET, PIN_CS);
#elif defined(USE_XILKA_SOFT)
Xilka::SpiSoftWriter spiSoftWriter(PIN_SCLK, PIN_MOSI);
Xilka::SSD1306_128x64 ssd(PIN_RESET, PIN_DC, PIN_CS, &spiSoftWriter);
#elif defined(USE_XILKA_HARD)
Xilka::SSD1306_128x64 ssd(PIN_RESET, PIN_DC, PIN_CS);
#else
#error you must define one of the USE_ macros
#endif

namespace {

#if defined(IS_ADA)
const uint16_t white(WHITE);
#else
const uint16_t white(Xilka::SSD1306::WHITE);
#endif

} // namespace

void setup()
{
	Serial.begin(115200);

#if defined(USE_I2C)
	Wire.begin();
	Wire.setClock(SSD1306::MaxI2CSpeed);
#elif defined(USE_XILKA_HARD)
	SPI.begin();
#endif

	Serial.println("ssd.begin() - begin");
	delay(250);

	ssd.begin();

	Serial.println("ssd.begin() - end");

	Serial.println("loop - begin");

	unsigned long start(millis());

	for ( int i = 0; i < 100; ++i )
		ssd.display();

	unsigned long end(millis());

	Serial.println("loop - end");
	Serial.print("Time ");
	Serial.print(end - start);
	Serial.println("mS");

	ssd.clearDisplay();
	ssd.setTextSize(1);
	ssd.setTextColor(white);
	ssd.setCursor(5, 5);
	ssd.print("Time ");
	ssd.print(end - start);
	ssd.println("mS");
	ssd.display();
}

void loop()
{
}

