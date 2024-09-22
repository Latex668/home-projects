/*
 * RA8875 480x272 support
 */

#include <Arduino.h>
#include <SPI.h>

#include "XilkaRA8875_480x272.h"
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

#if 0
  /* Timing values */
  uint8_t pixclk          = RA8875_PCSR_PDATL | RA8875_PCSR_4CLK;
  uint8_t hsync_start;
  uint8_t hsync_pw;
  uint8_t hsync_finetune;
  uint8_t hsync_nondisp;
  uint8_t vsync_pw;
  uint16_t vsync_nondisp;
  uint16_t vsync_start;

  hsync_nondisp   = 10;
  hsync_start     = 8;
  hsync_pw        = 48;
  hsync_finetune  = 0;
  vsync_nondisp   = 3;
  vsync_start     = 8;
  vsync_pw        = 10;

  // 1
  writeReg(RA8875_PCSR, pixclk);
  delay(1);

/* Horizontal settings registers */
  // 2
  writeReg(RA8875_HDWR, (_width / 8) - 1);                          // H width: (HDWR + 1) * 8 = 480

  // 3
  writeReg(RA8875_HNDFTR, RA8875_HNDFTR_DE_HIGH + hsync_finetune);

  // 4
  writeReg(RA8875_HNDR, (hsync_nondisp - hsync_finetune - 2)/8);    // H non-display: HNDR * 8 + HNDFTR + 2 = 10

  // 5
  writeReg(RA8875_HSTR, hsync_start/8 - 1);                         // Hsync start: (HSTR + 1)*8

  // 6
  writeReg(RA8875_HPWR, RA8875_HPWR_LOW + (hsync_pw/8 - 1));        // HSync pulse width = (HPWR+1) * 8

/* Vertical settings registers */

  // 7
  writeReg(RA8875_VDHR0, (uint16_t)(_height - 1) & 0xFF);

  // 8
  writeReg(RA8875_VDHR1, (uint16_t)(_height - 1) >> 8);

  // 9
  writeReg(RA8875_VNDR0, vsync_nondisp-1);                          // V non-display period = VNDR + 1

  // 10
  writeReg(RA8875_VNDR1, vsync_nondisp >> 8);

  // 11
  writeReg(RA8875_VSTR0, vsync_start-1);                            // Vsync start position = VSTR + 1

  // 12
  writeReg(RA8875_VSTR1, vsync_start >> 8);

  // 13
  writeReg(RA8875_VPWR, RA8875_VPWR_LOW + vsync_pw - 1);            // Vsync pulse width = VPWR + 1

/* Set active window X */

  // 14
  writeReg(RA8875_HSAW0, 0);                                        // horizontal start point

  // 15
  writeReg(RA8875_HSAW1, 0);

  // 16
  writeReg(RA8875_HEAW0, (uint16_t)(_width - 1) & 0xFF);            // horizontal end point

  // 17
  writeReg(RA8875_HEAW1, (uint16_t)(_width - 1) >> 8);

/* Set active window Y */

  // 18
  writeReg(RA8875_VSAW0, 0);                                        // vertical start point

  // 19
  writeReg(RA8875_VSAW1, 0);

  // 20
  writeReg(RA8875_VEAW0, (uint16_t)(_height - 1) & 0xFF);           // horizontal end point

  // 21
  writeReg(RA8875_VEAW1, (uint16_t)(_height - 1) >> 8);

  /* ToDo: Setup touch panel? */

  /* Clear the entire window */

  // 22
  writeReg(RA8875_MCLR, RA8875_MCLR_START | RA8875_MCLR_FULL);
  delay(500);
#endif
}

namespace Xilka {

RA8875_480x272::RA8875_480x272(int8_t resetPin
		, int8_t dcPin
		, int8_t csPin
		, SpiSoftWriter *spiSoft)
		/*
		 * determined from spec sheet page 62
		 * system clock/3
		 *
		 * system clock is specified on spec sheet page 89
		 * 480x272 clock is 9MHZ so use 3MHz
		 */
	: RA8875X(Dim(480, 272)
					, SPISettings(3000000, MSBFIRST, SPI_MODE0)
					, resetPin, dcPin, csPin, spiSoft)
{
}

void RA8875_480x272::begin(void)
{
	issueRomCommands(cmdInitPll);
	issueRomCommands(cmdSysr);
	// finish the init
}

} // namespace Xilka

