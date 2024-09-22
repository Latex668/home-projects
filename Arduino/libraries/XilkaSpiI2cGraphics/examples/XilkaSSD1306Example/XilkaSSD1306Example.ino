/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x64 size display using SPI to communicate
4 or 5 pins are required to interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <XilkaGFX.h>
#include <XilkaI2c.h>
#include <XilkaSSD1306_128x64.h>
using namespace Xilka;
//#define USE_ADA_SOFT 1
//#define USE_ADA_HARD 1
//#define USE_XILKA_SOFT 1
#define USE_XILKA_HARD 1
#if defined(ARDUINO_ARCH_ESP8266)
#define USE_ESP8266_HARDWARE_CS 1
#endif
#define USE_SYSTEM_RESET 1

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
#error you must define one of the TEST_ macros
#endif

#if !defined(IS_ADA)
const uint16_t white(SSD1306::WHITE);
const uint16_t black(SSD1306::BLACK);
#else
const uint16_t white(WHITE);
const uint16_t black(BLACK);
#endif

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

namespace {

const GFX::Dim logoDim(16, 16);

static const unsigned char PROGMEM logo16_glcd_bmp[] =
{
	B00000000, B11000000,
	B00000001, B11000000,
	B00000001, B11000000,
	B00000011, B11100000,
	B11110011, B11100000,
	B11111110, B11111000,
	B01111110, B11111111,
	B00110011, B10011111,
	B00011111, B11111100,
	B00001101, B01110000,
	B00011011, B10100000,
	B00111111, B11100000,
	B00111111, B11110000,
	B01111100, B11110000,
	B01110000, B01110000,
	B00000000, B00110000
};

void testDrawBitmap(const GFX::Dim &d, const uint8_t *bitmap);
void testDrawChar(void);
void testDrawCircle(void);
void testFillRect(void);
void testDrawTriangle(void);
void testFillTriangle(void);
void testDrawRoundRect(void);
void testFillRoundRect(void);
void testDrawRect(void);
void testDrawLine(void);
void testScrollText(void);

} // namespace


void setup()
{
	Serial.begin(115200);

	// by default, we'll generate the high voltage from the 3.3v line internally! (neat!)

#if defined(USE_XILKA_HARD)
	SPI.begin();
#endif

    delay(250);

	ssd.begin();

	ssd.clearDisplay();
	ssd.display();
	delay(1000);

#if 0
    for ( int16_t x = 0; x < ssd.getCurrentDim().w; ++x )
    {
        for ( int16_t y = 0; y < ssd.getCurrentDim().h; ++y )
        {
            ssd.clearDisplay();
            ssd.drawPixel(GFX::Pos(x, y), white);
            ssd.display();
            delay(5);
        }
    }
#endif

#if 0
#if 1
    for ( int16_t i = 0; i < ssd.getCurrentDim().w; ++i )
    {
        ssd.clearDisplay();
        ssd.drawLine(GFX::Pos(i, 0), GFX::Dim(i, ssd.getCurrentDim().h), white);
        ssd.display();
        delay(500);
    }
#else
    for ( int16_t i = 0; i < ssd.getCurrentDim().h; ++i )
    {
        ssd.clearDisplay();
        ssd.drawLine(GFX::Pos(0, i), GFX::Dim(ssd.getCurrentDim().w, i), white);
        ssd.display();
        delay(500);
    }
#endif
#endif

#if 1
	// draw a single pixel
	ssd.drawPixel(GFX::Pos(10, 10), white);
	// Show the display buffer on the hardware.
	// NOTE: You _must_ call display after making any drawing commands
	// to make them visible on the display hardware!
	ssd.display();
	delay(2000);
	ssd.clearDisplay();

	// draw many lines
	testDrawLine();
	ssd.display();
	delay(2000);

	// draw rectangles
	ssd.clearDisplay();
	testDrawRect();
	ssd.display();
	delay(2000);

	// draw multiple rectangles
	ssd.clearDisplay();
	testFillRect();
	ssd.display();
	delay(2000);

	// draw mulitple circles
	ssd.clearDisplay();
	testDrawCircle();
	ssd.display();
	delay(2000);

	// draw a white circle, 10 pixel radius
	ssd.clearDisplay();
	ssd.fillCircle(GFX::Pos(ssd.getCurrentDim().w / 2, ssd.getCurrentDim().h / 2), 10, white);
	ssd.display();
	delay(2000);

	ssd.clearDisplay();
	testDrawRoundRect();
	delay(2000);

	ssd.clearDisplay();
	testFillRoundRect();
	delay(2000);

	ssd.clearDisplay();
	testDrawTriangle();
	delay(2000);

	ssd.clearDisplay();
	testFillTriangle();
	delay(2000);

	// draw the first ~12 characters in the font
	ssd.clearDisplay();
	testDrawChar();
	ssd.display();
	delay(2000);

	ssd.clearDisplay();
	testScrollText();
	delay(2000);

	// text display tests
	ssd.clearDisplay();
	ssd.setTextSize(1);
	ssd.setTextColor(white);
	ssd.setCursor(0, 0);
	ssd.println("Hello, world!");
	ssd.setTextColor(black, white); // 'inverted' text
	ssd.println(3.141592);
	ssd.setTextSize(2);
	ssd.setTextColor(white);
	ssd.print("0x");
	ssd.println(0xDEADBEEF, HEX);
	ssd.display();
	delay(2000);

	// miniature bitmap display
	ssd.clearDisplay();
	ssd.drawBitmap(GFX::Pos(30, 16), logoDim, logo16_glcd_bmp, 1);
	ssd.display();

	ssd.invertDisplay(true);
	delay(1000);
	ssd.invertDisplay(false);
	delay(1000);

	// draw a bitmap icon and 'animate' movement
	testDrawBitmap(logoDim, logo16_glcd_bmp);
#endif
}

void loop()
{
}

namespace {

void testDrawBitmap(const GFX::Dim &d, const uint8_t *bitmap)
{
	uint8_t icons[NUMFLAKES][3];

	// initialize
	for ( uint8_t f = 0; f < NUMFLAKES; ++f )
	{
		icons[f][XPOS] = random(ssd.getCurrentDim().w);
		icons[f][YPOS] = 0;
		icons[f][DELTAY] = random(5) + 1;

		Serial.print("x: ");
		Serial.print(icons[f][XPOS], DEC);
		Serial.print(" y: ");
		Serial.print(icons[f][YPOS], DEC);
		Serial.print(" dy: ");
		Serial.println(icons[f][DELTAY], DEC);
	}

	while ( true )
	{
		// draw each icon
		for ( uint8_t f = 0; f < NUMFLAKES; ++f )
		{
			ssd.drawBitmap(GFX::Pos(icons[f][XPOS], icons[f][YPOS])
				, d
				, logo16_glcd_bmp
				, white);
		}

		ssd.display();
		delay(200);

		// then erase it + move it
		for ( uint8_t f = 0; f < NUMFLAKES; ++f )
		{
			ssd.drawBitmap(GFX::Pos(icons[f][XPOS], icons[f][YPOS])
				, d, logo16_glcd_bmp, black);

			// move it
			icons[f][YPOS] += icons[f][DELTAY];

			// if its gone, reinit
			if ( icons[f][YPOS] > ssd.getCurrentDim().h )
			{
				icons[f][XPOS] = random(ssd.getCurrentDim().w);
				icons[f][YPOS] = 0;
				icons[f][DELTAY] = random(5) + 1;
			}
		}
	}
}

void testDrawChar(void)
{
	ssd.setTextSize(1);
	ssd.setTextColor(white);
	ssd.setCursor(0, 0);

	for ( uint8_t i = 0; i < 168; ++i )
	{
		if ( i == '\n' )
			continue;

		ssd.write(i);

		if ( (i > 0) && (i % 21 == 0) )
			ssd.println();
	}

	ssd.display();
}

void testDrawCircle(void)
{
	const GFX::Pos p(ssd.getCurrentDim().w / 2, ssd.getCurrentDim().h / 2);

	for ( int16_t i = 0; i < ssd.getCurrentDim().h; i += 2 )
	{
		ssd.drawCircle(p, i, white);
		ssd.display();
	}
}

void testFillRect(void)
{
	uint16_t color(white);

	for ( int16_t i = 0; i < ssd.getCurrentDim().h / 2; i += 3 )
	{
		const int16_t i2(i * 2);

		// alternate colors
		ssd.fillRect(GFX::Pos(i, i)
				, GFX::Dim(ssd.getCurrentDim().w - i2, ssd.getCurrentDim().h - i2)
				, color);

		ssd.display();

		color = ( color == white ) ? black : white;
	}
}

void testDrawTriangle(void)
{
	const int16_t iMax(min(ssd.getCurrentDim().w, ssd.getCurrentDim().h) / 2);
	const int16_t w2(ssd.getCurrentDim().w / 2);
	const int16_t h2(ssd.getCurrentDim().h / 2);

	for ( int16_t i = 0; i < iMax; i += 5 )
	{
		ssd.drawTriangle(GFX::Pos(w2, h2 - i)
				, GFX::Pos(w2 - i, h2 + i)
				, GFX::Pos(w2 + i, h2 + i)
				, white);

		ssd.display();
	}
}

void testFillTriangle(void)
{
	const int16_t iMax(min(ssd.getCurrentDim().w, ssd.getCurrentDim().h) / 2);
	const int16_t w2(ssd.getCurrentDim().w / 2);
	const int16_t h2(ssd.getCurrentDim().h / 2);
	uint16_t color(white);

	for ( int16_t i = iMax; i > 0; i -= 5 )
	{
		ssd.fillTriangle(GFX::Pos(w2 , h2 - i)
				, GFX::Pos(w2 - i, h2 + i)
				, GFX::Pos(w2 + i, h2 + i)
				, color);

		color = ( color == white ) ? black : white;

		ssd.display();
	}
}

void testDrawRoundRect(void)
{
	for ( int16_t i = 0; i < ssd.getCurrentDim().h / 2 - 2; i += 2 )
	{
		ssd.drawRoundRect(GFX::Pos(i, i)
				, GFX::Dim(ssd.getCurrentDim().w - 2 * i, ssd.getCurrentDim().h - 2 * i)
				, ssd.getCurrentDim().h / 4
				, white);

		ssd.display();
	}
}

void testFillRoundRect(void)
{
	uint16_t color(white);

	for ( int16_t i = 0; i < ssd.getCurrentDim().h / 2 - 2; i += 2 )
	{
		ssd.fillRoundRect(GFX::Pos(i, i)
				, GFX::Dim(ssd.getCurrentDim().w - 2 * i, ssd.getCurrentDim().h - 2 * i)
				, ssd.getCurrentDim().h / 4
				, color);

		color = ( color == white ) ? black : white;

		ssd.display();
	}
}

void testDrawRect(void)
{
	for ( int16_t i = 0; i < ssd.getCurrentDim().h / 2; i += 2 )
	{
		ssd.drawRect(GFX::Pos(i, i)
			, GFX::Dim(ssd.getCurrentDim().w - 2 * i, ssd.getCurrentDim().h - 2 * i)
			, white);

		ssd.display();
	}
}

void testDrawLine(void)
{
	for ( int16_t i = 0; i < ssd.getCurrentDim().w; i += 4 )
	{
		ssd.drawLine(GFX::Pos(0, 0)
			, GFX::Pos(i, ssd.getCurrentDim().h - 1)
			, white);

		ssd.display();
	}

	for ( int16_t i = 0; i < ssd.getCurrentDim().h; i += 4 )
	{
		ssd.drawLine(GFX::Pos(0, 0)
			, GFX::Pos(ssd.getCurrentDim().w - 1, i)
			, white);

		ssd.display();
	}

	delay(250);

	ssd.clearDisplay();

	for ( int16_t i = 0; i < ssd.getCurrentDim().w; i += 4 )
	{
		ssd.drawLine(GFX::Pos(0, ssd.getCurrentDim().h - 1)
				, GFX::Pos(i, 0)
				, white);

		ssd.display();
	}

	for ( int16_t i = ssd.getCurrentDim().h - 1; i >= 0; i -= 4 )
	{
		ssd.drawLine(GFX::Pos(0, ssd.getCurrentDim().h - 1)
				, GFX::Pos(ssd.getCurrentDim().w - 1, i)
				, white);

		ssd.display();
	}

	delay(250);

	ssd.clearDisplay();

	for ( int16_t i = ssd.getCurrentDim().w - 1; i >= 0; i -= 4 )
	{
		ssd.drawLine(GFX::Pos(ssd.getCurrentDim().w - 1, ssd.getCurrentDim().h - 1)
				, GFX::Pos(i, 0)
				, white);
		ssd.display();
	}

	for ( int16_t i = ssd.getCurrentDim().h - 1; i >= 0; i -= 4 )
	{
		ssd.drawLine(GFX::Pos(ssd.getCurrentDim().w - 1, ssd.getCurrentDim().h - 1)
				, GFX::Pos(0, i)
				, white);
		ssd.display();
	}

	delay(250);

	ssd.clearDisplay();

	for ( int16_t i = 0; i < ssd.getCurrentDim().h; i += 4 )
	{
		ssd.drawLine(GFX::Pos(ssd.getCurrentDim().w - 1, 0)
				, GFX::Pos(0, i)
				, white);

		ssd.display();
	}

	for ( int16_t i = 0; i < ssd.getCurrentDim().w; i += 4 )
	{
		ssd.drawLine(GFX::Pos(ssd.getCurrentDim().w - 1, 0)
				, GFX::Pos(i, ssd.getCurrentDim().h - 1)
				, white);

		ssd.display();
	}

	delay(250);
}

void testScrollText(void)
{
	ssd.setTextSize(2);
	ssd.setTextColor(white);
	ssd.setCursor(10, 0);
	ssd.clearDisplay();
	ssd.println("scroll");
	ssd.display();

#if ! defined(IS_ADA)
    ssd.startScrollRight(0x00, 0x0F);
    delay(2000);
    ssd.stopScroll();
    delay(1000);
    ssd.startScrollLeft(0x00, 0x0F);
    delay(2000);
    ssd.stopScroll();
    delay(1000);
    ssd.startScrollRightDiag(0x00, 0x07);
    delay(2000);
    ssd.startScrollLeftDiag(0x00, 0x07);
    delay(2000);
    ssd.stopScroll();
#else
	  ssd.startscrollright(0x00, 0x0F);
	  delay(2000);
	  ssd.stopscroll();
	  delay(1000);
	  ssd.startscrollleft(0x00, 0x0F);
	  delay(2000);
	  ssd.stopscroll();
	  delay(1000);
	  ssd.startscrolldiagright(0x00, 0x07);
	  delay(2000);
	  ssd.startscrolldiagleft(0x00, 0x07);
	  delay(2000);
	  ssd.stopscroll();
#endif
}

} // namespace


