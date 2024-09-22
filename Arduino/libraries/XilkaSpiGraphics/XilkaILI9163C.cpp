/*
 * Supports the ILI9163C chip used on
 * 1.44" 128x128 boards available on ebay.
 */

#include <Arduino.h>
#include <SPI.h>

#include "XilkaILI9163C.h"
#include "XilkaILI9163CCommands.h"

#if defined(ARDUINO_ARCH_AVR)
#include <avr/pgmspace.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <pgmspace.h>
#else
#define PROGMEM
#endif

namespace {

const uint8_t DELAY(0x80);

#if ! defined(SPI_HAS_TRANSACTION) && defined(ARDUINO_ARCH_AVR)
uint8_t SPCRbackup;
uint8_t mySPCR;
#endif

const uint8_t PROGMEM
cmdList[] = {
	20 // commands in list

	// 1
	, CMD_SWRESET, 0 + DELAY // reset
	, 255 // delay 500mS

	// 2
	, CMD_SLPOUT, 0 + DELAY // exit sleep
	, 5 // delay 5Ms


	// 3
	, CMD_PIXFMT, 1 + DELAY // Set Color Format 16bit
	, 0x05, 5

	// 4
	, CMD_GAMMASET, 1 + DELAY // default gamma curve 3
	, 0x08, 1

	// 5
	, CMD_GAMRSEL, 1 + DELAY // Enable Gamma adj
	,  0x01, 1

	// 6
	, CMD_NORML, 0

	// 7
	,  CMD_DFUNCTR, 2
	, 0b11111111
	, 0b00000110

	// 8
	, CMD_PGAMMAC, 15 // Positive Gamma Correction Setting
	, 0x36, 0x29, 0x12, 0x22
	, 0x1C, 0x15, 0x42, 0xB7
	, 0x2F, 0x13, 0x12, 0x0A
	, 0x11, 0x0B, 0x06

	// 9
	, CMD_NGAMMAC, 15 // Negative Gamma Correction Setting
	, 0x09, 0x16, 0x2D, 0x0D
	, 0x13, 0x15, 0x40, 0x48
	, 0x53, 0x0C, 0x1D, 0x25
	, 0x2E, 0x34, 0x39

	// 10
	, CMD_FRMCTR1, 2 + DELAY // Frame Rate Control (In normal mode/Full colors)
	, 0x08 // 0x0C // 0x08
	, 0x02 // 0x14 // 0x08
	, 1 // mS

	// 11
	, CMD_DINVCTR, 1 + DELAY //display inversion
	, 0x07
	, 1 // mS

	// 12
	, CMD_PWCTR1, 2 + DELAY // Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
	, 0x0A // 4.30 - 0x0A
	, 0x02 // 0x05
	, 1 // mS

	// 13
	, CMD_PWCTR2, 1 + DELAY // Set BT[2:0] for AVDD & VCL & VGH & VGL
	, 0x02
	, 1 // mS

	// 14
	, CMD_VCOMCTR1, 2 + DELAY // Set VMH[6:0] & VML[6:0] for VOMH & VCOML
	, 0x50 // 0x50
	, 99 // 0x5b
	, 1 // mS

	// 15
	, CMD_VCOMOFFS, 1 + DELAY
	, 0 // 0x40
	, 1 // mS

	// 16
	, CMD_CLMADRS, 4 // Set Column Address
	, 0x00, 0x00
	, 0x00, 0x80 // GRAMWIDTH

	// 17
	, CMD_PGEADRS, 4 // Set Page Address
	, 0x00, 0x00
	, 0x00, 128 // GRAMHEIGHT

	// 18
	// set scroll area (thanks Masuda)
	, CMD_VSCLLDEF, 6
	, 0x00, 0x00 // __OFFSET
	, 0x00, 128 // _GRAMHEIGH - __OFFSET
	, 0x00, 0x00

	// 19
	, CMD_DISPON, 0 + DELAY // display ON
	, 1 // mS

	// 20
	, CMD_RAMWR, 0 + DELAY // Memory Write
	, 1 // mS
};

} // namespace

namespace Xilka {

ILI9163C::ILI9163C(int8_t resetPin
		, int8_t dcPin
		, int8_t csPin
		, SpiSoftWriter *spiSoft)
		/*
		 * 33.33 MHz
		 * determined from spec sheet page 197
		 * SCL pulse H/L width 30nS
		 */
	: SpiGraphics(Dim(128, 128)
					, SPISettings(33333333, MSBFIRST, SPI_MODE0)
					, resetPin, dcPin, csPin, spiSoft)
{
}

void ILI9163C::init(void)
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

	setRotation(0);
}

void ILI9163C::setRotation(uint8_t m)
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
		spiWrite(BGR, true);
		_currentDim = _rawDim;
     break;
   case 1:
		spiWrite(MX | MV | BGR, true);
		_currentDim.w = _rawDim.h;
		_currentDim.h = _rawDim.w;
		break;
	case 2:
		spiWrite(MY | MX | BGR, true);
		_currentDim = _rawDim;
		break;
	case 3:
		spiWrite(MY | MV | BGR, true);
		_currentDim.w = _rawDim.h;
		_currentDim.h = _rawDim.w;
		break;
	}

	spiEndTran();
}

} // namespace Xilka

