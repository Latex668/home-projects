/*
 * Base class for the RA8875 family of chips
 */

#include <Arduino.h>
#include <SPI.h>

#include "XilkaRA8875X.h"
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
cmdSoftReset[] = {
	1 // num of cmds
	, RA8875_PWRR, 2 + DELAY
	, RA8875_PWRR_SOFTRESET
	, RA8875_PWRR_NORMAL
	, 1 // mS
};

}

namespace Xilka {

void RA8875X::softReset(void)
{
	issueRomCommands(cmdSoftReset);
}

void RA8875X::drawPixelNT(const GFX::Pos &p, uint16_t color)
{
#warning finish drawPixelNT
#if 0
	writeReg(RA8875_CURH0, p.x);
	writeReg(RA8875_CURH1, p.x >> 8);
	writeReg(RA8875_CURV0, p.y);
	writeReg(RA8875_CURV1, p.y >> 8);
	writeCommand(RA8875_MRWC);

	digitalWrite(_cs, LOW);
	SPI.transfer(RA8875_DATAWRITE);
	SPI.transfer(color >> 8);
	SPI.transfer(color);
	digitalWrite(_cs, HIGH);
#endif
}

void RA8875X::drawLineNT(const GFX::Pos &p1
		, const GFX::Pos &p2, uint16_t color)
{
	const uint8_t cmds[] = {
		12 // number of commands

		/* Set X */
		// 1
		, 0x91, 1
		, (uint8_t)(p1.x & 0xFF)

		// 2
		, 0x92, 1
		, (uint8_t)(p1.x >> 8)

		/* Set Y */
		// 3
		, 0x93, 1
		, (uint8_t)(p1.y & 0xFF)

		// 4
		, 0x94, 1
		, (uint8_t)(p1.y >> 8)

		/* Set X1 */
		// 5
		, 0x95, 1
		, (uint8_t)(p2.x & 0xFF)

		// 6
		, 0x96, 1
		, (uint8_t)(p2.x >> 8)

		/* Set Y1 */
		// 7
		, 0x97, 1
		, (uint8_t)(p2.y & 0xFF)

		// 8
		, 0x98, 1
		, (uint8_t)(p2.y >> 8)

		/* Set Color */
		// 9
		, 0x63, 1
		, (uint8_t)((color & 0xf800) >> 11)

		// 10
		, 0x64, 1
		, (uint8_t)((color & 0x07e0) >> 5)

		// 11
		, 0x65, 1
		, (uint8_t)(color & 0x001f)

		/* Draw! */
		// 12
		, RA8875_DCR, 1
		, 0x80
	};

	issueRamCommands(cmds);
	waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

void RA8875X::circleHelper(const GFX::Pos &p
		, int16_t r, uint16_t color
		, bool wantFilled)
{
	const uint8_t cmds[] = {
		9 // number of commands
		/* Set X */
		// 1
		, 0x99, 1
		, (uint8_t)(p.x & 0xFF)

		// 2
		, 0x9a, 1
		, (uint8_t)(p.x >> 8)

		/* Set Y */
		// 3
		, 0x9b, 1
		, (uint8_t)(p.y & 0xFF)

		// 4
		, 0x9c, 1
		, (uint8_t)(p.y >> 8)

		/* Set Radius */
		// 5
		, 0x9d, 1
		, (uint8_t)(r & 0xFF)

		/* Set Color */
		// 6
		, 0x63, 1
		, (uint8_t)((color & 0xf800) >> 11)

		// 7
		, 0x64, 1
		, (uint8_t)((color & 0x07e0) >> 5)

		// 8
		, 0x65, 1
		, (uint8_t)(color & 0x001f)

		/* Draw! */
		// 9
		, RA8875_DCR, 1
		, (wantFilled
				? (RA8875_DCR_CIRCLE_START | RA8875_DCR_FILL)
				: (RA8875_DCR_CIRCLE_START | RA8875_DCR_NOFILL))
	};

	issueRamCommands(cmds);
	waitPoll(RA8875_DCR, RA8875_DCR_CIRCLE_STATUS);
}

void RA8875X::rectHelper(const GFX::Pos &p1
		, const GFX::Pos &p2
		, uint16_t color
		, bool wantFilled)
{
	const uint8_t cmds[] = {
		12 // number of commands
		/* Set X1 */
		// 1
		, 0x91, 1
		, (uint8_t)(p1.x & 0xFF)

		// 2
		, 0x92, 1
		, (uint8_t)(p1.x >> 8)

		/* Set Y1 */
		// 3
		, 0x93, 1
		, (uint8_t)(p1.y & 0xFF)

		// 4
		, 0x94, 1
		, (uint8_t)(p1.y >> 8)

		/* Set X2 */
		// 5
		, 0x95, 1
		, (uint8_t)(p2.x & 0xFF)

		// 6
		, 0x96, 1
		, (uint8_t)(p2.x >> 8)

		/* Set Y2 */
		// 7
		, 0x97, 1
		, (uint8_t)(p2.y & 0xFF)

		// 8
		, 0x98, 1
		, (uint8_t)(p2.y >> 8)

		/* Set Color */
		// 9
		, 0x63, 1
		, (uint8_t)((color & 0xf800) >> 11)

		// 10
		, 0x64, 1
		, (uint8_t)((color & 0x07e0) >> 5)

		// 11
		, 0x65, 1
		, (uint8_t)(color & 0x001f)

		/* Draw! */
		// 12
		, RA8875_DCR, 1
		, (wantFilled ? 0xB0 : 0x90)
	};

	issueRamCommands(cmds);
	waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

void RA8875X::triangleHelper(const GFX::Pos &p1
		, const GFX::Pos &p2
		, const GFX::Pos &p3
		, uint16_t color
		, bool wantFilled)
{
	const uint8_t cmds[] = {
		16 // number of commands
		/* Set p1 */
		// 1
		, 0x91, 1
		, (uint8_t)p1.x

		// 2
		, 0x92, 1
		, (uint8_t)(p1.x >> 8)

		// 3
		, 0x93, 1
		, (uint8_t)p1.y

		// 4
		, 0x94, 1
		, (uint8_t)(p1.y >> 8)

		/* Set p2 */
		// 5
		, 0x95, 1
		, (uint8_t)p2.x

		// 6
		, 0x96, 1
		, (uint8_t)(p2.x >> 8)

		// 7
		, 0x97, 1
		, (uint8_t)p2.y

		// 8
		, 0x98, 1
		, (uint8_t)(p2.y >> 8)

		/* Set p3 */
		// 9
		, 0xA9, 1
		, (uint8_t)p3.x

		// 10
		, 0xAA, 1
		, (uint8_t)(p3.x >> 8)

		// 11
		, 0xAB, 1
		, (uint8_t)p3.y

		// 12
		, 0xAC, 1
		, (uint8_t)(p3.y >> 8)

		/* Set Color */
		// 13
		, 0x63, 1
		, (uint8_t)((color & 0xf800) >> 11)

		// 14
		, 0x64, 1
		, (uint8_t)((color & 0x07e0) >> 5)

		// 15
		, 0x65, 1
		, (uint8_t)(color & 0x001f)

		/* Draw! */
		// 16
		, RA8875_DCR, 1
		, (wantFilled ? 0xA1 : 0x81)
	};

	issueRamCommands(cmds);
	waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
}

void RA8875X::ellipseHelper(const GFX::Pos &center
		, int16_t longAxis, int16_t shortAxis
		, uint16_t color, bool wantFilled)
{
	const uint8_t cmds[] = {
		12 // number of commands

		/* Set Center Point */
		// 1
		, 0xA5, 1
		, (uint8_t)(center.x & 0xFF)

		// 2
		, 0xA6, 1
		, (uint8_t)(center.x >> 8)

		// 3
		, 0xA7, 1
		, (uint8_t)(center.y & 0xFF)

		// 4
		, 0xA8, 1
		, (uint8_t)(center.y >> 8)

		/* Set Long Axis */
		// 5
		, 0xA1, 1
		, (uint8_t)(longAxis & 0xFF)

		// 6
		, 0xA2, 1
		, (uint8_t)(longAxis >> 8)

		/* Set Short Axis */
		// 7
		, 0xA3, 1
		, (uint8_t)(shortAxis & 0xFF)

		// 8
		, 0xA4, 1
		, (uint8_t)(shortAxis >> 8)

		/* Set Color */
		// 9
		, 0x63, 1
		, (uint8_t)((color & 0xf800) >> 11)

		// 10
		, 0x64, 1
		, (uint8_t)((color & 0x07e0) >> 5)

		// 11
		, 0x65, 1
		, (uint8_t)(color & 0x001f)

		/* Draw! */
		// 12
		, 0xA0, 1
		, (wantFilled ? 0xC0 : 0x80)
	};

	issueRamCommands(cmds);
	waitPoll(RA8875_ELLIPSE, RA8875_ELLIPSE_STATUS);
}


void RA8875X::curveHelper(const GFX::Pos &center
		, int16_t longAxis, int16_t shortAxis
		, uint8_t curvePart, uint16_t color
		, bool wantFilled)
{
	const uint8_t cmds[] = {
		12 // number of commands

		// 1
		, 0xA5, 1 /* Set Center Point */
		, (uint8_t)(center.x & 0xFF)

		// 2
		, 0xA6, 1
		, (uint8_t)(center.x >> 8)

		// 3
		, 0xA7, 1
		, (uint8_t)(center.y & 0xFF)

		// 4
		, 0xA8, 1
		, (uint8_t)(center.y >> 8)

		/* Set Long Axis */
		// 5
		, 0xA1, 1
		, (uint8_t)(longAxis & 0xFF)

		// 6
		, 0xA2, 1
		, (uint8_t)(longAxis >> 8)

		/* Set Short Axis */
		// 7
		, 0xA3, 1
		, (uint8_t)(shortAxis & 0xFF)

		// 8
		, 0xA4, 1
		, (uint8_t)(shortAxis >> 8)

		/* Set Color */
		// 9
		, 0x63, 1
		, (uint8_t)((color & 0xf800) >> 11)

		// 10
		, 0x64, 1
		, (uint8_t)((color & 0x07e0) >> 5)

		// 11
		, 0x65, 1
		, (uint16_t)(color & 0x001f)

		/* Draw! */
		// 12
		, 0xA0, 1
		, (uint8_t)((wantFilled ? 0xD0 : 0x90) | (curvePart & 0x03))
	};

	issueRamCommands(cmds);
	waitPoll(RA8875_ELLIPSE, RA8875_ELLIPSE_STATUS);
}

bool RA8875X::waitPoll(uint8_t regname, uint8_t waitflag)
{
/*
 * Wait for a command to finish
 */
	const unsigned long start(micros());

	for ( ; ; )
	{
		uint8_t temp = readReg(regname);

		if ( !(temp & waitflag) )
			return true;
		else if ( (micros() - start) > 5000 ) // wait for up to 5mS
			break;
	}

	return false;
}

void RA8875X::displayOn(bool wantOn)
{
	spiSetData(false);
	spiWrite(RA8875_PWRR, false);

	spiSetData(true);
	spiWrite(wantOn
			? RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON
			: RA8875_PWRR_NORMAL | RA8875_PWRR_DISPOFF
					, true);
}

void RA8875X::sleep(bool wantSleep)
{
	spiSetData(false);
	spiWrite(RA8875_PWRR, false);

	spiSetData(true);
	spiWrite(wantSleep
				? RA8875_PWRR_DISPOFF | RA8875_PWRR_SLEEP
				: RA8875_PWRR_DISPOFF
						, true);
}

} // namespace Xilka

