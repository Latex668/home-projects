/*
 * Supports the HX8357B chip (320x480)
 */

#include <Arduino.h>
#include <SPI.h>

#include "XilkaHX8357B.h"
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

	// 1 - seqpower
	, HX8357B_SETPOWER, 3
	, 0x44, 0x41, 0x06

	// 2 - seq_vcom
	, HX8357B_SETVCOM, 2
	, 0x40, 0x10

	// 3 - seq_power_normal
	, HX8357B_SETPWRNORMAL, 2
	, 0x05, 0x12

	// 4 - seq_panel_driving
	, HX8357B_SET_PANEL_DRIVING, 5
	, 0x14, 0x3b, 0x00, 0x02
	, 0x11

	// 5 - seq_display_frame
	, HX8357B_SETDISPLAYFRAME, 1
	, 0x0c // 6.8mhz

	// 6 - seq_panel_related
	, HX8357B_SETPANELRELATED, 1
	, 0x01 // BGR

	// 7 - seq_undefined1
	, 0xEA, 3
	, 0x03, 0x00, 0x00

	// 8 - undef2
	, 0xEB, 4
	, 0x40, 0x54, 0x26, 0xdb

	// 9 - seq_gamma
	, HX8357B_SETGAMMA, 12
	, 0x00, 0x15, 0x00, 0x22
	, 0x00, 0x08, 0x77, 0x26
	, 0x66, 0x22, 0x04, 0x00

	// 10 - seq_addr mode
	, HX8357_MADCTL, 1
	, 0xC0

	// 11 - pixel format
	, HX8357_COLMOD, 1
	, 0x55

	// 12
	, HX8357_PASET, 4
	, 0x00, 0x00, 0x01, 0xDF

	// 13
	, HX8357_CASET, 4
	, 0x00, 0x00, 0x01, 0x3F

	// 14 - display mode
	, HX8357B_SETDISPMODE, 1
	, 0x00 // CPU (DBI) and internal oscillation ??

	// 15 - exit sleep
	, HX8357_SLPOUT, 0 + DELAY
	, 120 // mS

	// 16 - main screen turn on
	, HX8357_DISPON, 0 + DELAY
	, 10 // mS
};

} // namespace

namespace Xilka {

HX8357B::HX8357B(int8_t resetPin
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

void HX8357B::begin(void)
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

