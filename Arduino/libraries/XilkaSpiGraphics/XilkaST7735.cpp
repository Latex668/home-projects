/***************************************************
  This is a library for the Adafruit 1.8" SPI display.

This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
The 1.8" TFT shield
  ----> https://www.adafruit.com/product/802
The 1.44" TFT breakout
  ----> https://www.adafruit.com/product/2088
as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Arduino.h>
#include <SPI.h>
#include "XilkaST7735.h"

#if defined(ARDUINO_ARCH_AVR)
#include <avr/pgmspace.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <pgmspace.h>
#else
#define PROGMEM
#endif

namespace {

#define TFTWIDTH  128
// for 1.44" display
#define TFTHEIGHT_144 128
// for 1.8" display
#define TFTHEIGHT_18  160

const uint8_t CMD_NOP(0x00);
const uint8_t CMD_SWRESET(0x01);
const uint8_t CMD_RDDID(0x04);
const uint8_t CMD_RDDST(0x09);

const uint8_t CMD_SLPIN(0x10);
const uint8_t CMD_SLPOUT(0x11);
const uint8_t CMD_PTLON(0x12);
const uint8_t CMD_NORON(0x13);

const uint8_t CMD_INVOFF(0x20);
const uint8_t CMD_INVON(0x21);
const uint8_t CMD_DISPOFF(0x28);
const uint8_t CMD_DISPON(0x29);
const uint8_t CMD_CASET(0x2A);
const uint8_t CMD_RASET(0x2B);
const uint8_t CMD_RAMWR(0x2C);
const uint8_t CMD_RAMRD(0x2E);

const uint8_t CMD_PTLAR(0x30);
const uint8_t CMD_COLMOD(0x3A);
const uint8_t CMD_MADCTL(0x36);

const uint8_t CMD_FRMCTR1(0xB1);
const uint8_t CMD_FRMCTR2(0xB2);
const uint8_t CMD_FRMCTR3(0xB3);
const uint8_t CMD_INVCTR(0xB4);
const uint8_t CMD_DISSET5(0xB6);

const uint8_t CMD_PWCTR1(0xC0);
const uint8_t CMD_PWCTR2(0xC1);
const uint8_t CMD_PWCTR3(0xC2);
const uint8_t CMD_PWCTR4(0xC3);
const uint8_t CMD_PWCTR5(0xC4);
const uint8_t CMD_VMCTR1(0xC5);

const uint8_t CMD_RDID1(0xDA);
const uint8_t CMD_RDID2(0xDB);
const uint8_t CMD_RDID3(0xDC);
const uint8_t CMD_RDID4(0xDD);

const uint8_t CMD_PWCTR6(0xFC);

const uint8_t CMD_GMCTRP1(0xE0);
const uint8_t CMD_GMCTRN1(0xE1);

const uint8_t DELAY(0x80);

const uint8_t PROGMEM
  Bcmd[] = {                  // Initialization commands for 7735B screens
    18,                       // 18 commands in list:
    CMD_SWRESET,   DELAY,  //  1: Software reset, no args, w/delay
      50,                     //     50 ms delay
    CMD_SLPOUT ,   DELAY,  //  2: Out of sleep mode, no args, w/delay
      255,                    //     255 = 500 ms delay
    CMD_COLMOD , 1+DELAY,  //  3: Set color mode, 1 arg + delay:
      0x05,                   //     16-bit color
      10,                     //     10 ms delay
    CMD_FRMCTR1, 3+DELAY,  //  4: Frame rate control, 3 args + delay:
      0x00,                   //     fastest refresh
      0x06,                   //     6 lines front porch
      0x03,                   //     3 lines back porch
      10,                     //     10 ms delay
    CMD_MADCTL , 1      ,  //  5: Memory access ctrl (directions), 1 arg:
      0x08,                   //     Row addr/col addr, bottom to top refresh
    CMD_DISSET5, 2      ,  //  6: Display settings #5, 2 args, no delay:
      0x15,                   //     1 clk cycle nonoverlap, 2 cycle gate
                              //     rise, 3 cycle osc equalize
      0x02,                   //     Fix on VTL
    CMD_INVCTR , 1      ,  //  7: Display inversion control, 1 arg:
      0x0,                    //     Line inversion
    CMD_PWCTR1 , 2+DELAY,  //  8: Power control, 2 args + delay:
      0x02,                   //     GVDD = 4.7V
      0x70,                   //     1.0uA
      10,                     //     10 ms delay
    CMD_PWCTR2 , 1      ,  //  9: Power control, 1 arg, no delay:
      0x05,                   //     VGH = 14.7V, VGL = -7.35V
    CMD_PWCTR3 , 2      ,  // 10: Power control, 2 args, no delay:
      0x01,                   //     Opamp current small
      0x02,                   //     Boost frequency
    CMD_VMCTR1 , 2+DELAY,  // 11: Power control, 2 args + delay:
      0x3C,                   //     VCOMH = 4V
      0x38,                   //     VCOML = -1.1V
      10,                     //     10 ms delay
    CMD_PWCTR6 , 2      ,  // 12: Power control, 2 args, no delay:
      0x11, 0x15,
    CMD_GMCTRP1,16      ,  // 13: Magical unicorn dust, 16 args, no delay:
      0x09, 0x16, 0x09, 0x20, //     (seriously though, not sure what
      0x21, 0x1B, 0x13, 0x19, //      these config values represent)
      0x17, 0x15, 0x1E, 0x2B,
      0x04, 0x05, 0x02, 0x0E,
    CMD_GMCTRN1,16+DELAY,  // 14: Sparkles and rainbows, 16 args + delay:
      0x0B, 0x14, 0x08, 0x1E, //     (ditto)
      0x22, 0x1D, 0x18, 0x1E,
      0x1B, 0x1A, 0x24, 0x2B,
      0x06, 0x06, 0x02, 0x0F,
      10,                     //     10 ms delay
    CMD_CASET  , 4      ,  // 15: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 2
      0x00, 0x81,             //     XEND = 129
    CMD_RASET  , 4      ,  // 16: Row addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 1
      0x00, 0x81,             //     XEND = 160
    CMD_NORON  ,   DELAY,  // 17: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    CMD_DISPON ,   DELAY,  // 18: Main screen turn on, no args, w/delay
      255 },                  //     255 = 500 ms delay

  Rcmd1[] = {                 // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    CMD_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    CMD_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    CMD_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    CMD_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    CMD_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    CMD_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    CMD_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    CMD_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    CMD_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    CMD_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,
    CMD_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    CMD_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    CMD_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    CMD_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      0xC8,                   //     row addr/col addr, bottom to top refresh
    CMD_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 },                 //     16-bit color

  Rcmd2green[] = {            // Init for 7735R, part 2 (green tab only)
    2,                        //  2 commands in list:
    CMD_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 0
      0x00, 0x7F+0x02,        //     XEND = 127
    CMD_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x01,             //     XSTART = 0
      0x00, 0x9F+0x01 },      //     XEND = 159
  Rcmd2red[] = {              // Init for 7735R, part 2 (red tab only)
    2,                        //  2 commands in list:
    CMD_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    CMD_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F },           //     XEND = 159

  Rcmd2green144[] = {              // Init for 7735R, part 2 (green 1.44 tab)
    2,                        //  2 commands in list:
    CMD_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    CMD_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F },           //     XEND = 127

  Rcmd3[] = {                 // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    CMD_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    CMD_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    CMD_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    CMD_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay

#if 0
inline uint16_t swapcolor(uint16_t x)
{
	return (x << 11) | (x & 0x07E0) | (x >> 11);
}
#endif

#if ! defined(SPI_HAS_TRANSACTION) && defined(ARDUINO_ARCH_AVR)
uint8_t SPCRbackup;
uint8_t mySPCR;
#endif

} // namespace

namespace Xilka {

ST7735::ST7735(int8_t resetPin
		, int8_t dcPin
		, int8_t csPin
		, SpiSoftWriter *spiSoft)
		/*
		 * 15.1 MHz
		 * determined from spec sheet page 24
		 * min tCycle time of 66nS
		 */
	: SpiGraphics(Dim(TFTWIDTH, TFTHEIGHT_18)
					, SPISettings(15100000, MSBFIRST, SPI_MODE0)
					, resetPin, dcPin, csPin, spiSoft)
	, _tabColor(0)
	, _colStart(0)
	, _rowStart(0)
{
}

// Initialization code common to both 'B' and 'R' type displays
void ST7735::initCommon(const uint8_t *cmdList)
{
	_colStart = _rowStart = 0; // May be overridden in init func

	/*
	 * reset is high for normal operations
	 */
	_reset.setHigh();

	/*
	 * cs is high for normal operations
	 */
	_cs.setHigh();

	if ( isHwSpi() )
	{
#if defined(ARDUINO_ARCH_ESP8266)
		if ( _cs.getPin() == -1 )
			SPI.setHwCs(true);
#elif ! defined(SPI_HAS_TRANSACTION) && defined(ARDUINO_ARCH_AVR)
		SPCRbackup = SPCR;
		SPI.setClockDivider(SPI_CLOCK_DIV2); // typically 8MHz
		SPI.setDataMode(SPI_MODE0);
		mySPCR = SPCR; // save our preferred state
		//Serial.print("mySPCR = 0x"); Serial.println(SPCR, HEX);
		SPCR = SPCRbackup;  // then restore
#endif
	}

	if ( _reset.getPin() != -1 )
	{
		_cs.setLow();

		_reset.setHigh();
		delay(500);

		_reset.setLow();
		delay(500);

		_reset.setHigh();
		delay(500);

		_cs.setHigh();
	}

	if ( cmdList )
		issueRomCommands(cmdList);
}

// Initialization for ST7735B screens
void ST7735::initB(void)
{
	initCommon(Bcmd);
}

// Initialization for ST7735R screens (green or red tabs)
void ST7735::initR(uint8_t options)
{
	initCommon(Rcmd1);

	if ( options == INITR_GREENTAB )
	{
		issueRomCommands(Rcmd2green);
		_colStart = 2;
		_rowStart = 1;
	}
	else if ( options == INITR_144GREENTAB )
	{
		_currentDim.h = TFTHEIGHT_144;
		issueRomCommands(Rcmd2green144);
		_colStart = 2;
		_rowStart = 3;
	}
	else
	{
		issueRomCommands(Rcmd2red);
	}

	issueRomCommands(Rcmd3);

	// if black, change MADCTL color filter
	if ( options == INITR_BLACKTAB )
	{
		spiBeginTran();

		spiSetData(false);
		spiWrite(CMD_MADCTL, false);

		spiSetData(true);
		spiWrite(0xC0, true);

		spiEndTran();
	}

	_tabColor = options;
}

void ST7735::setWindow(const GFX::Pos &p1, const GFX::Pos &p2)
{
	Data32 cas;
	Data32 ras;
	cas.w[0] = p1.x + _colStart; // XSTART
	cas.w[1] = p2.x + _colStart; // XEND
	ras.w[0] = p1.y + _rowStart; // YSTART
	ras.w[1] = p2.y + _rowStart; // YEND

	if ( isHwSpi() )
	{
#if defined(SPI_ACCEL)
		spiSetData(false);
		spiAccelWrite(CMD_CASET, false); // Column addr set

		spiSetData(true);
		spiWriteHard(cas);

		spiSetData(false);
		spiAccelWrite(CMD_RASET, false); // Row addr set

		spiSetData(true);
		spiWriteHard(ras);

		spiSetData(false);
		spiAccelWrite(CMD_RAMWR, false); // write to RAM
#else // defined(SPI_ACCEL)
		spiSetData(false);
		spiWriteHard(CMD_CASET); // Column addr set

		spiSetData(true);
		spiWriteHard(cas);

		spiSetData(false);
		spiWriteHard(CMD_RASET); // Row addr set

		spiSetData(true);
		spiWriteHard(ras);

		spiSetData(false);
		spiWriteHard(CMD_RAMWR); // write to RAM
#endif // defined(SPI_ACCEL)
	}
	else
	{
		spiSetData(false);
		spiWrite(CMD_CASET, false); // Column addr set

		spiSetData(true);
		spiWrite(cas);

		spiSetData(false);
		spiWrite(CMD_RASET, false); // Row addr set

		spiSetData(true);
		spiWrite(ras);

		spiSetData(false);
		spiWrite(CMD_RAMWR, false); // write to RAM
	}
}

void ST7735::setRotation(uint8_t m)
{
	static const uint8_t MY(0x80);
	static const uint8_t MX(0x40);
	static const uint8_t MV(0x20);
	// static const uint8_t ML(0x10);
	static const uint8_t RGB(0x00);
	static const uint8_t BGR(0x08);
	// static const uint8_t MH(0x04);

	spiBeginTran();

	spiSetData(false);
	spiWrite(CMD_MADCTL, false);

	rotation = m % 4; // can't be higher than 3

	spiSetData(true);

	switch ( rotation )
	{
	case 0:
		spiWrite( ( _tabColor == INITR_BLACKTAB )
					? (MX | MY | RGB)
					: (MX | MY | BGR), true);

		_currentDim.w = TFTWIDTH;
		_currentDim.h = ( _tabColor == INITR_144GREENTAB )
					? TFTHEIGHT_144
					: TFTHEIGHT_18;

		break;
	case 1:
		spiWrite( ( _tabColor == INITR_BLACKTAB )
					? (MY | MV | RGB)
					: (MY | MV | BGR), true);

		_currentDim.w = ( _tabColor == INITR_144GREENTAB )
					? TFTHEIGHT_144
					: TFTHEIGHT_18;

		_currentDim.h = TFTWIDTH;
		break;
	case 2:
		spiWrite( ( _tabColor == INITR_BLACKTAB )
					? RGB
					: BGR, true);

		_currentDim.w = TFTWIDTH;

		_currentDim.h = ( _tabColor == INITR_144GREENTAB )
					? TFTHEIGHT_144
					: TFTHEIGHT_18;
		break;
	case 3:
		spiWrite( ( _tabColor == INITR_BLACKTAB )
					? (MX | MV | RGB)
					: (MX | MV | BGR), true);

		_currentDim.w = ( _tabColor == INITR_144GREENTAB )
					? TFTHEIGHT_144
					: TFTHEIGHT_18;

		_currentDim.h = TFTWIDTH;
		break;
	}

	spiEndTran();
}

} // namespace Xilka

