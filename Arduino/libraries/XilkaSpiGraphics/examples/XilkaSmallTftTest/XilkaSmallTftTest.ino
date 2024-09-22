/*
 * Test programs for small (128x128, 128x160) displays
 */

#include <SPI.h>
#include <XilkaGFX.h>
#include <XilkaST7735.h>
#include <XilkaILI9163C.h>
#include <XilkaILI9163CRed.h>

using namespace Xilka;

#define USE_ST7735 1
//#define USE_ILI9163C 1
//#define USE_ILI9163CRED 1

#define USE_SPI_HARD 1
#define USE_ESP8266_HARDWARE_CS 1
// if the display reset is connected to the microcontroller reset
//#define USE_SYSTEM_RESET 1

namespace {

#if defined(ARDUINO_ARCH_ESP8266)
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
const int8_t PIN_CS(10);
const int8_t PIN_DC(8);
#if defined(USE_SYSTEM_RESET)
const int8_t PIN_RESET(-1);
#else
const int8_t PIN_RESET(9);
#endif
const int8_t PIN_SCLK(13); // may be set to any digital pin when using software SPI
const int8_t PIN_MOSI(11); // may be set to any digital pin when using software SPI
#endif // defined(ARDUINO_ARCH_ESP8266)

#if defined(USE_SPI_HARD)
// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and mosi = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
#if defined(USE_ILI9163CRED)
ILI9163CRed tft(PIN_RESET, PIN_DC, PIN_CS);
#elif defined(USE_ILI9163C)
ILI9163C tft(PIN_RESET, PIN_DC, PIN_CS);
#else
ST7735 tft(PIN_RESET, PIN_DC, PIN_CS);
#endif
#else

SpiSoftWriter spiSoftWriter(PIN_SCLK, PIN_MOSI);
#if defined(USE_ILI9163CRED)
ILI9163CRed tft(PIN_RESET, PIN_DC, PIN_CS, &spiSoftWriter);
#elif defined(USE_ILI9163C)
ILI9163C tft(PIN_RESET, PIN_DC, PIN_CS, &spiSoftWriter);
#else
ST7735 tft(PIN_RESET, PIN_DC, PIN_CS, &spiSoftWriter);
#endif
#endif

const float pi(3.1415926);

} // namespace


void setup(void)
{
	Serial.begin(115200);

#if defined(USE_SPI_HARD)
	SPI.begin();
#endif

	Serial.print("Hello! Small TFT Test");

#if defined(USE_ILI9163C) || defined(USE_ILI9163CRED)
	tft.init();
#else
	// Use this initializer if you're using a 1.8" TFT
	tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

	// Use this initializer (uncomment) if you're using a 1.44" TFT
	//tft.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab
#endif

	Serial.println("Initialized");

	uint16_t time(millis());
	tft.fillScreen(tft.BLACK);
	time = millis() - time;

	Serial.println(time, DEC);
	delay(500);

	tft.fillScreen(tft.BLACK);
	testDrawText("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", tft.WHITE);
	delay(1000);

	tftPrintTest();
	delay(4000);

	tft.drawPixel(GFX::Pos(tft.getCurrentDim().w / 2, tft.getCurrentDim().h / 2), tft.GREEN);
	delay(500);

	testLines(tft.YELLOW);
	delay(500);

	testFastLines(tft.RED, tft.BLUE);
	delay(500);

	testDrawRects(tft.GREEN);
	delay(500);

	testFillRects(tft.YELLOW, tft.MAGENTA);
	delay(500);

	tft.fillScreen(tft.BLACK);
	testFillCircles(10, tft.BLUE);
	testDrawCircles(10, tft.WHITE);
	delay(500);

	testRoundRects();
	delay(500);

	testTriangles();
	delay(500);

	testMediaButtons();
	delay(500);

	Serial.println("done");
	delay(1000);
}

void loop()
{
	tft.invertDisplay(true);
	delay(500);
	tft.invertDisplay(false);
	delay(500);
}

void testLines(uint16_t color)
{
	tft.fillScreen(tft.BLACK);

	const int16_t w(tft.getCurrentDim().w);
	const int16_t h(tft.getCurrentDim().h);
	const GFX::Pos zero(0, 0);

	for ( int16_t x = 0; x < w; x += 6 )
		tft.drawLine(zero, GFX::Pos(x, h - 1), color);

	for ( int16_t y = 0; y < h; y += 6 )
		tft.drawLine(zero, GFX::Pos(w - 1, y), color);

	tft.fillScreen(tft.BLACK);

	for ( int16_t x = 0; x < w; x += 6 )
		tft.drawLine(GFX::Pos(w - 1, 0), GFX::Pos(x, h - 1), color);

	for ( int16_t y = 0; y < h; y += 6 )
		tft.drawLine(GFX::Pos(w - 1, 0), GFX::Pos(0, y), color);

	tft.fillScreen(tft.BLACK);

	for ( int16_t x = 0; x < w; x += 6 )
		tft.drawLine(GFX::Pos(0, h - 1), GFX::Pos(x, 0), color);

	for ( int16_t y = 0; y < h; y += 6 )
		tft.drawLine(GFX::Pos(0, h - 1), GFX::Pos(w - 1, y), color);

	tft.fillScreen(tft.BLACK);

	for ( int16_t x = 0; x < w; x += 6 )
		tft.drawLine(GFX::Pos(w - 1, h - 1), GFX::Pos(x, 0), color);

	for ( int16_t y = 0; y < h; y += 6 )
		tft.drawLine(GFX::Pos(w - 1, h - 1), GFX::Pos(0, y), color);
}

void testDrawText(const char *text, uint16_t color)
{
	tft.setCursor(0, 0);
	tft.setTextColor(color);
	tft.setTextWrap(true);
	tft.print(text);
}

void testFastLines(uint16_t color1, uint16_t color2)
{
	tft.fillScreen(tft.BLACK);

	const int16_t w(tft.getCurrentDim().w);
	const int16_t h(tft.getCurrentDim().h);

	for ( int16_t y = 0; y < h; y += 5 )
		tft.drawFastHLine(GFX::Pos(0, y), w, color1);

	for ( int16_t x = 0; x < w; x += 5 )
		tft.drawFastVLine(GFX::Pos(x, 0), h, color2);
}

void testDrawRects(uint16_t color)
{
	tft.fillScreen(tft.BLACK);

	const int16_t w(tft.getCurrentDim().w);
	const int16_t h(tft.getCurrentDim().h);

	for ( int16_t x = 0; x < w; x += 6 )
		tft.drawRect(GFX::Pos(w / 2 - x / 2, h / 2 - x / 2)
				, GFX::Dim(x, x), color);
}

void testFillRects(uint16_t color1, uint16_t color2)
{
	tft.fillScreen(tft.BLACK);

	const int16_t w(tft.getCurrentDim().w);
	const int16_t h(tft.getCurrentDim().h);

	for ( int16_t x = w - 1; x > 6; x -= 6 )
	{
		const GFX::Pos p((w - x) / 2, (h - x) / 2);
		const GFX::Dim d(x, x);

		tft.fillRect(p, d, color1);
		tft.drawRect(p, d, color2);
	}
}

void testFillCircles(int16_t radius, uint16_t color)
{
	const int16_t diameter(radius*2);
	const int16_t w(tft.getCurrentDim().w);
	const int16_t h(tft.getCurrentDim().h);

	for ( int16_t x = radius; x < w; x += diameter )
		for ( int16_t y = radius; y < h; y += diameter )
			tft.fillCircle(GFX::Pos(x, y), radius, color);
}

void testDrawCircles(int16_t radius, uint16_t color)
{
	const int16_t diameter(radius*2);
	const int16_t w(tft.getCurrentDim().w);
	const int16_t h(tft.getCurrentDim().h);

	for ( int16_t x = 0; x < w + radius; x += diameter )
		for ( int16_t y = 0; y < h + radius; y += diameter )
			tft.drawCircle(GFX::Pos(x, y), radius, color);
}

void testTriangles(void)
{
	tft.fillScreen(tft.BLACK);

	int color(0xF800);
	const int w(tft.getCurrentDim().w / 2);
	int x(tft.getCurrentDim().h - 1);
	int y(0);
	int z(tft.getCurrentDim().w);

	for ( int t = 0; t <= 15; ++t )
	{
		tft.drawTriangle(GFX::Pos(w, y), GFX::Pos(y, x), GFX::Pos(z, x), color);
		x -= 4;
		y += 4;
		z -= 4;
		color += 100;
	}
}

void testRoundRects(void)
{
	tft.fillScreen(tft.BLACK);

	int color(100);

	for ( int t = 0; t <= 4; ++t )
	{
		int x(0);
		int y(0);
		int w(tft.getCurrentDim().w - 2);
		int h(tft.getCurrentDim().h - 2);

		for ( int i = 0; i <= 16; ++i )
		{
			tft.drawRoundRect(GFX::Pos(x, y), GFX::Dim(w, h), 5, color);
			x += 2;
			y += 3;
			w -= 4;
			h -= 6;
			color += 1100;
		}

		color += 100;
	}
}

void tftPrintTest(void)
{
	tft.setTextWrap(false);
	tft.fillScreen(tft.BLACK);
	tft.setCursor(0, 30);
	tft.setTextColor(tft.RED);
	tft.setTextSize(1);
	tft.println("Hello World!");
	tft.setTextColor(tft.YELLOW);
	tft.setTextSize(2);
	tft.println("Hello World!");
	tft.setTextColor(tft.GREEN);
	tft.setTextSize(3);
	tft.println("Hello World!");
	tft.setTextColor(tft.BLUE);
	tft.setTextSize(4);
	tft.print(1234.567);
	delay(1500);
	tft.setCursor(0, 0);
	tft.fillScreen(tft.BLACK);
	tft.setTextColor(tft.WHITE);
	tft.setTextSize(0);
	tft.println("Hello World!");
	tft.setTextSize(1);
	tft.setTextColor(tft.GREEN);
	tft.print(pi, 6);
	tft.println(" Want pi?");
	tft.println(" ");
	tft.print(8675309, HEX); // print 8,675,309 out in HEX!
	tft.println(" Print HEX!");
	tft.println(" ");
	tft.setTextColor(tft.WHITE);
	tft.println("Sketch has been");
	tft.println("running for: ");
	tft.setTextColor(tft.MAGENTA);
	tft.print(millis() / 1000);
	tft.setTextColor(tft.WHITE);
	tft.print(" seconds.");
}

void testMediaButtons(void)
{
	// play
	tft.fillScreen(tft.BLACK);
	tft.fillRoundRect(GFX::Pos(25, 10), GFX::Dim(78, 60), 8, tft.WHITE);
	tft.fillTriangle(GFX::Pos(42, 20), GFX::Pos(42, 60), GFX::Pos(90, 40), tft.RED);

	delay(500);

	tft.fillRoundRect(GFX::Pos(25, 90), GFX::Dim(78, 60), 8, tft.WHITE);
	tft.fillRoundRect(GFX::Pos(39, 98), GFX::Dim(20, 45), 5, tft.GREEN);
	tft.fillRoundRect(GFX::Pos(69, 98), GFX::Dim(20, 45), 5, tft.GREEN);

	delay(500);

	tft.fillTriangle(GFX::Pos(42, 20), GFX::Pos(42, 60), GFX::Pos(90, 40), tft.BLUE);

	delay(50);

	tft.fillRoundRect(GFX::Pos(39, 98), GFX::Dim(20, 45), 5, tft.RED);
	tft.fillRoundRect(GFX::Pos(69, 98), GFX::Dim(20, 45), 5, tft.RED);
	// play color
	tft.fillTriangle(GFX::Pos(42, 20), GFX::Pos(42, 60), GFX::Pos(90, 40), tft.GREEN);
}

