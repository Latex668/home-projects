/*
 * RA8875 800x480 support
 */

#include <Arduino.h>
#include <SPI.h>

#include "XilkaRA8875_800x480.h"
#include "XilkaRA8875Commands.h"

#if defined(ARDUINO_ARCH_AVR)
#include <avr/pgmspace.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <pgmspace.h>
#else
#define PROGMEM
#endif

namespace {

const uint8_t DELAY(0x80);

const uint8_t PROGMEM
cmdInitPll[] = {
	2 // num of commands
	// 1
	, RA8875_PLLC1, 1 + DELAY
	, RA8875_PLLC1_PLLDIV1 + 10
	, 1 // mS
	// 2
	, RA8875_PLLC2, 1 + DELAY
	, RA8875_PLLC2_DIV4
	, 1 // mS
};

const uint8_t PROGMEM
cmdSysr[] = {
	1 // num of commands
	, RA8875_SYSR, 1
	, RA8875_SYSR_16BPP | RA8875_SYSR_MCU8
};

}

namespace Xilka {

RA8875_800x480::RA8875_800x480(int8_t resetPin
		, int8_t dcPin
		, int8_t csPin
		, SpiSoftWriter *spiSoft)
		/*
		 * determined from spec sheet page 62
		 * system clock/3
		 *
		 * system clock is specified on spec sheet page 89
		 * 800x480 clock is 30-33MHZ so use 10MHz
		 */
	: RA8875X(Dim(800, 480)
					, SPISettings(10000000, MSBFIRST, SPI_MODE0)
					, resetPin, dcPin, csPin, spiSoft)
{
}

void RA8875_800x480::begin(void)
{
	issueRomCommands(cmdInitPll);
	issueRomCommands(cmdSysr);
}

} // namespace Xilka

