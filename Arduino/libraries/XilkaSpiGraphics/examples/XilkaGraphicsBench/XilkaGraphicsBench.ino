/*
 * select display type to test
 */
#include <Arduino.h>
#include <XilkaSpi.h>
#include <SPI.h>
#include <XilkaGFX.h>
#include <XilkaHX8357B.h>
#include <XilkaHX8357D.h>
#include <XilkaILI9340.h>
#include <XilkaILI9341.h>
#include <XilkaST7735.h>

#define USE_SPI_HARD 1
//#define USE_ST7735 1
//typedef Xilka::ST7735 DisplayType;
//typedef Xilka::HX8357B DisplayType;
//typedef Xilka::HX8357D DisplayType;
//typedef Xilka::ILI9340 DisplayType;
typedef Xilka::ILI9341 DisplayType;

using namespace Xilka;

namespace {

#if defined(ARDUINO_ARCH_AVR) && defined(__arm__)
// Teensy 3.1
const int8_t PIN_RESET(8);
const int8_t PIN_DC(9);
const int8_t PIN_CS(10);
const int8_t PIN_MOSI(11);
const int8_t PIN_MISO(12);
const int8_t PIN_SCLK(13);
#elif defined(ARDUINO_ARCH_ESP8266)
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
const int8_t PIN_DC(9);
#if defined(USE_SYSTEM_RESET)
const int8_t PIN_RESET(-1);
#else
const int8_t PIN_RESET(8);
#endif
const int8_t PIN_SCLK(13); // may be set to any digital pin when using software SPI
const int8_t PIN_MOSI(11); // may be set to any digital pin when using software SPI
#endif // defined(ARDUINO_ARCH_ESP8266)

#if defined(USE_SPI_HARD)
// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and mosi = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
DisplayType tft(PIN_RESET, PIN_DC, PIN_CS);

#else // defined(USE_SPI_HARD)

SpiSoftWriter spiSoftWriter(PIN_SCLK, PIN_MOSI);
DisplayType tft(PIN_RESET, PIN_DC, PIN_CS, &spiSoftWriter);

#endif // defined(USE_SPI_HARD)

} // namespace

void setup()
{
  Serial.begin(115200);
#if defined(USE_SPI_HARD)
#if defined(USE_TEENSY_FIFO)
	spiBegin(PIN_CS, PIN_DC);
#else
	spiBegin();
#endif
#endif

	delay(1000);

#if defined(USE_ST7735)
	tft.initR(INITR_BLACKTAB);
#else
	tft.begin();
#endif

	unsigned long t(testFillScreen());
	delay(500);

	t += testText();
	delay(3000);

	t += testLines(tft.CYAN);
	delay(500);

	t += testFastLines(tft.RED, tft.BLUE);
	delay(500);

	t += testRects(tft.GREEN);
	delay(500);

	t += testFilledRects(tft.YELLOW, tft.MAGENTA);
	delay(500);

	t += testFilledCircles(10, tft.MAGENTA);

	t += testCircles(10, tft.WHITE);
	delay(500);

	t += testTriangles();
	delay(500);

	t += testFilledTriangles();
	delay(500);

	t += testRoundRects();
	delay(500);

	t += testFilledRoundRects();
	delay(500);

	tft.fillScreen(tft.BLACK);
	tft.setTextColor(tft.WHITE);
	tft.setCursor(0, 0);
	tft.setTextSize(2);
	tft.print((float)t/1000000.0);
	tft.println(" sec");
}


void loop(void)
{
#if 0
	for( uint8_t rotation = 0; rotation < 4; ++rotation )
	{
		tft.setRotation(rotation);
		testText();
		delay(1000);
	}
#endif
}

unsigned long testFillScreen(void)
{
	const unsigned long start(micros());
	tft.fillScreen(tft.BLACK);
	tft.fillScreen(tft.RED);
	tft.fillScreen(tft.GREEN);
	tft.fillScreen(tft.BLUE);
	tft.fillScreen(tft.BLACK);
	return micros() - start;
}

unsigned long testText(void)
{
	tft.fillScreen(tft.BLACK);
	const unsigned long start(micros());
	tft.setCursor(0, 0);

	tft.setTextColor(tft.WHITE);
	tft.setTextSize(1);
	tft.println("Hello World!");

	tft.setTextColor(tft.YELLOW);
	tft.setTextSize(2);
	tft.println(1234.56);

	tft.setTextColor(tft.RED);
	tft.setTextSize(3);
	tft.println(0xDEADBEEF, HEX);
	tft.println();

	tft.setTextColor(tft.GREEN);
	tft.setTextSize(5);
	tft.println("Group");

	tft.setTextSize(2);
	tft.println("I implore thee,");

	tft.setTextSize(1);
	tft.println("my foonting turlingdromes.");
	tft.println("And hooptiously drangle me");
	tft.println("with crinkly bindlewurdles,");
	tft.println("Or I will rend thee");
	tft.println("in the gobberwarts");
	tft.println("with my blurglecruncheon,");
	tft.println("see if I don't!");
	return micros() - start;
}

unsigned long testLines(uint16_t color)
{
	const int w(tft.getCurrentDim().w);
	const int h(tft.getCurrentDim().h);

	tft.fillScreen(tft.BLACK);

	GFX::Pos p1(0, 0);
	GFX::Pos p2(0, h - 1);

	unsigned long start(micros());

	for ( p2.x = 0; p2.x < w; p2.x += 6 )
		tft.drawLine(p1, p2, color);

	p2.x = w - 1;

	for ( p2.y = 0; p2.y < h; p2.y += 6 )
		tft.drawLine(p1, p2, color);

	unsigned long t = micros() - start; // fillScreen doesn't count against timing

	tft.fillScreen(tft.BLACK);

	p1.x = w - 1;
	p1.y = 0;
	p2.y = h - 1;

	start = micros();

	for ( p2.x = 0; p2.x < w; p2.x += 6 )
		tft.drawLine(p1, p2, color);

	p2.x = 0;

	for ( p2.y = 0; p2.y < h; p2.y += 6)
		tft.drawLine(p1, p2, color);

	t += micros() - start;

	tft.fillScreen(tft.BLACK);

	p1.x = 0;
	p1.y = h - 1;
	p2.y = 0;

	start = micros();

	for ( p2.x = 0; p2.x < w; p2.x += 6 )
		tft.drawLine(p1, p2, color);

	p2.x = w - 1;

	for ( p2.y = 0; p2.y < h; p2.y += 6 )
		tft.drawLine(p1, p2, color);

	t += micros() - start;

	tft.fillScreen(tft.BLACK);

	p1.x = w - 1;
	p1.y = h - 1;
	p2.y = 0;

	start = micros();

	for ( p2.x = 0; p2.x < w; p2.x += 6 )
		tft.drawLine(p1, p2, color);

	p2.x = 0;

	for ( p2.y = 0; p2.y < h; p2.y += 6 )
		tft.drawLine(p1, p2, color);

	return t + (micros() - start);
}

unsigned long testFastLines(uint16_t color1, uint16_t color2)
{
	tft.fillScreen(tft.BLACK);

	const int w(tft.getCurrentDim().w);
	const int h(tft.getCurrentDim().h);
	const unsigned long start(micros());

	GFX::Pos pos(0, 0);

	for ( pos.y = 0; pos.y < h; pos.y += 5 )
		tft.drawFastHLine(pos, w, color1);

	pos.set(0, 0);

	for ( pos.x = 0; pos.x < w; pos.x += 5 )
		tft.drawFastVLine(pos, h, color2);

	return micros() - start;
}

unsigned long testRects(uint16_t color)
{
	tft.fillScreen(tft.BLACK);

	const int minWH(min(tft.getCurrentDim().w, tft.getCurrentDim().h));
	const int cx(tft.getCurrentDim().w / 2);
	const int cy(tft.getCurrentDim().h / 2);
	const unsigned long start(micros());

	for ( int i = 2; i < minWH; i += 6 )
	{
		int i2 = i / 2;

		tft.drawRect(GFX::Pos(cx - i2, cy - i2), GFX::Dim(i, i), color);
	}

	return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2)
{
	tft.fillScreen(tft.BLACK);

	const int minWH(min(tft.getCurrentDim().w, tft.getCurrentDim().h));
	const int cx(tft.getCurrentDim().w / 2 - 1);
	const int cy(tft.getCurrentDim().h / 2 - 1);
	unsigned long t(0);

	for ( int i = minWH; i > 0; i -= 6 )
	{
		const int i2(i / 2);
		unsigned long start(micros());

		tft.fillRect(GFX::Pos(cx - i2, cy - i2), GFX::Dim(i, i), color1);

		t  += micros() - start;

		// Outlines are not included in timing results
		tft.drawRect(GFX::Pos(cx - i2, cy - i2), GFX::Dim(i, i), color2);
	}

	return t;
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color)
{
	tft.fillScreen(tft.BLACK);

	const int w(tft.getCurrentDim().w);
	const int h(tft.getCurrentDim().h);
	const int r2(radius * 2);
	const unsigned long start(micros());

	for ( int x = radius; x < w; x += r2 )
		for ( int y = radius; y < h; y += r2 )
			tft.fillCircle(GFX::Pos(x, y), radius, color);

	return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color)
{
	// Screen is not cleared for this one -- this is
	// intentional and does not affect the reported time.

	const int w(tft.getCurrentDim().w);
	const int h(tft.getCurrentDim().h);
	const int r2(radius * 2);
	const unsigned long start(micros());

	for ( int x = radius; x < w; x += r2 )
		for ( int y = radius; y < h; y += r2 )
			tft.fillCircle(GFX::Pos(x, y), radius, color);

	return micros() - start;
}

unsigned long testTriangles(void)
{
	tft.fillScreen(tft.BLACK);

	const int cx(tft.getCurrentDim().w  / 2 - 1);
	const int cy(tft.getCurrentDim().h / 2 - 1);
	const int minCXY(min(cx, cy));
	const unsigned long start(micros());

	for ( int i = 0; i < minCXY; i += 5 )
	{
		tft.drawTriangle(
			GFX::Pos(cx, cy - i) // peak
			, GFX::Pos(cx - i, cy + i) // bottom left
			, GFX::Pos(cx + i, cy + i) // bottom right
			, tft.getColor565(0, 0, i) );
	}

	return micros() - start;
}

unsigned long testFilledTriangles(void)
{
	tft.fillScreen(tft.BLACK);

	unsigned long t(0);
	const int cx(tft.getCurrentDim().w  / 2 - 1);
	const int cy(tft.getCurrentDim().h / 2 - 1);
	const int minCXY(min(cx, cy));

	for ( int i = minCXY; i > 10; i -= 5 )
	{
		unsigned long start = micros();

		tft.fillTriangle(
			GFX::Pos(cx, cy - i)
			, GFX::Pos(cx - i, cy + i)
			, GFX::Pos(cx + i, cy + i)
			, tft.getColor565(0, i, i) );

		t += micros() - start;

		tft.drawTriangle(
			GFX::Pos(cx, cy - i)
			, GFX::Pos(cx - i, cy + i)
			, GFX::Pos(cx + i, cy + i)
			, tft.getColor565(i, i, 0));
	}

	return t;
}

unsigned long testRoundRects(void)
{
	tft.fillScreen(tft.BLACK);

	const int w(min(tft.getCurrentDim().w, tft.getCurrentDim().h));
	const int cx(tft.getCurrentDim().w  / 2 - 1);
	const int cy(tft.getCurrentDim().h / 2 - 1);
	const unsigned long start(micros());

	for ( int i = 0; i < w; i += 6 )
	{
		const int i2 = i / 2;

		tft.drawRoundRect(
			GFX::Pos(cx - i2, cy - i2)
			, GFX::Dim(i, i)
			, (i/8)
			, tft.getColor565(i, 0, 0));
	}

	return micros() - start;
}

unsigned long testFilledRoundRects(void)
{
	tft.fillScreen(tft.BLACK);

	const int w(min(tft.getCurrentDim().w, tft.getCurrentDim().h));
	const int cx(tft.getCurrentDim().w  / 2 - 1);
	const int cy(tft.getCurrentDim().h / 2 - 1);
	const unsigned long start(micros());

	for ( int i = w; i > 20; i -= 6 )
	{
		const int i2 = i / 2;

		tft.fillRoundRect(
			GFX::Pos(cx-i2, cy-i2)
			, GFX::Dim(i, i)
			, i/8
			, tft.getColor565(0, i, 0));
	}

	return micros() - start;
}

