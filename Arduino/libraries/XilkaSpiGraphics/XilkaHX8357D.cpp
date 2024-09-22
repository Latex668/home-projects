/*
 * Supports the HX8357D chip (320x480) used on
 *  ----> http://www.adafruit.com/products/2050
 */

#include <Arduino.h>
#include <SPI.h>

#include "XilkaHX8357D.h"
#include "XilkaHX8357Commands.h"

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
	16 // commands in list

	// 1
	, HX8357_SWRESET, 0

	// 2
	, HX8357D_SETC, 3 + DELAY
	, 0xFF, 0x83, 0x57
	, 255

	// 3
	, HX8357_SETRGB, 4
	, 0x80 // enable SDO pin!
	, 0x0
	, 0x06
	, 0x06

	// 4
	, HX8357D_SETCOM, 1
	, 0x25 // -1.52V

	// 5
	, HX8357_SETOSC, 1
	, 0x68 // Normal mode 70Hz, Idle mode 55 Hz

	// 6
	, HX8357_SETPANEL, 1
	, 0x05 // BGR, Gate direction swapped

	// 7
	, HX8357_SETPWR1, 6
	, 0x00 // Not deep standby
	, 0x15 // BT
	, 0x1C // VSPR
	, 0x1C // VSNR
	, 0x83 // AP
	, 0xAA // FS

	// 8
	, HX8357D_SETSTBA, 6
	, 0x50 // OPON normal
	, 0x50 // OPON idle
	, 0x01 // STBA
	, 0x3C // STBA
	, 0x1E // STBA
	, 0x08 // GEN

	// 9
	, HX8357D_SETCYC, 7
	, 0x02 // NW 0x02
	, 0x40 // RTN
	, 0x00 // DIV
	, 0x2A // DUM
	, 0x2A // DUM
	, 0x0D // GDON
	, 0x78 // GDOFF

	// 10
	, HX8357D_SETGAMMA, 34
	, 0x02, 0x0A, 0x11, 0x1d
	, 0x23, 0x35, 0x41, 0x4b
	, 0x4b, 0x42, 0x3A, 0x27
	, 0x1B, 0x08, 0x09, 0x03
	, 0x02, 0x0A, 0x11, 0x1d
	, 0x23, 0x35, 0x41, 0x4b
	, 0x4b, 0x42, 0x3A, 0x27
	, 0x1B, 0x08, 0x09, 0x03
	, 0x00, 0x01

	// 11
	, HX8357_COLMOD, 1
	, 0x55 // 16 bit

	// 12
	, HX8357_MADCTL, 1
	, 0xC0

	// 13
	, HX8357_TEON, 1 // TE off
	, 0x00

	// 14
	, HX8357_TEARLINE, 2 // tear line
	, 0x00, 0x02

	// 15
	, HX8357_SLPOUT, 0 + DELAY // Exit Sleep
	, 150 // mS

	// 16
	, HX8357_DISPON, 0 + DELAY // display on
	, 50 // mS
};

} // namespace

namespace Xilka {

HX8357D::HX8357D(int8_t resetPin
		, int8_t dcPin
		, int8_t csPin
		, SpiSoftWriter *spiSoft)
		/*
		 * 33.33 MHz
		 * determined from spec sheet page 240
		 * SCL pulse H/L width 30nS
		 */
	: HX8357X(Dim(320, 480)
					, SPISettings(33333333, MSBFIRST, SPI_MODE0)
					, resetPin, dcPin, csPin, spiSoft)
{
}

void HX8357D::begin(void)
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

} // namespace Xilka

