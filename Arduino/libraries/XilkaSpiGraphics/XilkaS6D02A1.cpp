/*
 * Supports the Samsung S6D02A1 chip used on
 * the QDTech 1.8" 128x160 board available on
 * ebay and banggood.
 */

#include <Arduino.h>
#include <SPI.h>
#include "XilkaS6D02A1.h"

#if defined(ARDUINO_ARCH_AVR)
#include <avr/pgmspace.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <pgmspace.h>
#else
#define PROGMEM
#endif

namespace {

#if 0
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
#endif

const uint8_t DELAY(0x80);

const uint8_t PROGMEM	// Multiple LCD init commands removed
cmdList[] = {
	29,
	0xf0,	2,	0x5a, 0x5a,				// Excommand2
	0xfc,	2,	0x5a, 0x5a,				// Excommand3
	0x26,	1,	0x01,					// Gamma set
	0xfa,	15,	0x02, 0x1f,	0x00, 0x10,	0x22, 0x30, 0x38, 0x3A, 0x3A, 0x3A,	0x3A, 0x3A,	0x3d, 0x02, 0x01,	// Positive gamma control
	0xfb,	15,	0x21, 0x00,	0x02, 0x04,	0x07, 0x0a, 0x0b, 0x0c, 0x0c, 0x16,	0x1e, 0x30,	0x3f, 0x01, 0x02,	// Negative gamma control
	0xfd,	11,	0x00, 0x00, 0x00, 0x17, 0x10, 0x00, 0x01, 0x01, 0x00, 0x1f, 0x1f,							// Analog parameter control
	0xf4,	15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x3f, 0x07, 0x00, 0x3C, 0x36, 0x00, 0x3C, 0x36, 0x00,	// Power control
	0xf5,	13, 0x00, 0x70, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6d, 0x66, 0x06,				// VCOM control
	0xf6,	11, 0x02, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x02, 0x00, 0x06, 0x01, 0x00,							// Source control
	0xf2,	17, 0x00, 0x01, 0x03, 0x08, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x08, 0x08,	//Display control
	0xf8,	1,	0x11,					// Gate control
	0xf7,	4, 0xc8, 0x20, 0x00, 0x00,	// Interface control
	0xf3,	2, 0x00, 0x00,				// Power sequence control
	0x11,	DELAY, 50,					// Wake
	0xf3,	2+DELAY, 0x00, 0x01, 50,	// Power sequence control
	0xf3,	2+DELAY, 0x00, 0x03, 50,	// Power sequence control
	0xf3,	2+DELAY, 0x00, 0x07, 50,	// Power sequence control
	0xf3,	2+DELAY, 0x00, 0x0f, 50,	// Power sequence control
	0xf4,	15+DELAY, 0x00, 0x04, 0x00, 0x00, 0x00, 0x3f, 0x3f, 0x07, 0x00, 0x3C, 0x36, 0x00, 0x3C, 0x36, 0x00, 50,	// Power control
	0xf3,	2+DELAY, 0x00, 0x1f, 50,	// Power sequence control
	0xf3,	2+DELAY, 0x00, 0x7f, 50,	// Power sequence control
	0xf3,	2+DELAY, 0x00, 0xff, 50,	// Power sequence control
	0xfd,	11, 0x00, 0x00, 0x00, 0x17, 0x10, 0x00, 0x00, 0x01, 0x00, 0x16, 0x16,							// Analog parameter control
	0xf4,	15, 0x00, 0x09, 0x00, 0x00, 0x00, 0x3f, 0x3f, 0x07, 0x00, 0x3C, 0x36, 0x00, 0x3C, 0x36, 0x00,	// Power control
	0x36,	1, 0x08,					// Memory access data control
	0x35,	1, 0x00,					// Tearing effect line on
	0x3a,	1+DELAY, 0x05, 150,			// Interface pixel control
	0x29,	0,							// Display on
	0x2c,	0							// Memory write
};

#if ! defined(SPI_HAS_TRANSACTION) && defined(ARDUINO_ARCH_AVR)
uint8_t SPCRbackup;
uint8_t mySPCR;
#endif

} // namespace

namespace Xilka {

S6D02A1::S6D02A1(int8_t resetPin
		, int8_t dcPin
		, int8_t csPin
		, SpiSoftWriter *spiSoft)
		/*
		 * 33.33 MHz
		 * determined from spec sheet page 35
		 * SCL pulse H/L width 30nS
		 */
	: SpiGraphics(Dim(128, 160)
					, SPISettings(33333333, MSBFIRST, SPI_MODE0)
					, resetPin, dcPin, csPin, spiSoft)
{
}

void S6D02A1::init(void)
{
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

	issueRomCommands(cmdList);
}

void S6D02A1::setRotation(uint8_t m)
{
	const uint8_t CMD_MADCTL(0x36);
	const uint8_t MY(0x80);
	const uint8_t MX(0x40);
	const uint8_t MV(0x20);
	//const uint8_t ML(0x10);
	//const uint8_t RGB(0x00);
	const uint8_t BGR(0x08);
	//const uint8_t MH(0x04);

	spiBeginTran();

	spiSetData(false);
	spiWrite(CMD_MADCTL, false);

	rotation = m % 4; // can't be higher than 3

	spiSetData(true);

	switch ( rotation )
	{
	case 0:
		spiWrite(MX | MY | BGR, true);
		_currentDim = _rawDim;
     break;
   case 1:
		spiWrite(MY | MV | BGR, true);
		_currentDim.w = _rawDim.h;
		_currentDim.h = _rawDim.w;
		break;
	case 2:
		spiWrite(BGR, true);
		_currentDim = _rawDim;
		break;
	case 3:
		spiWrite(MX | MV | BGR, true);
		_currentDim.w = _rawDim.h;
		_currentDim.h = _rawDim.w;
		break;
	}

	spiEndTran();
}

} // namespace Xilka

