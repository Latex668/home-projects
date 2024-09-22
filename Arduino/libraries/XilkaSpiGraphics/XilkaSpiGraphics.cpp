/*
 * Base library for Spi Graphics displays
 */

#include <Arduino.h>
#include <SPI.h>
#include <Print.h>
#include <limits.h>
#include <pins_arduino.h>
#include <wiring_private.h>
#if defined(__arm__)
#include <machine/endian.h>
#endif

#include "../XilkaSpi/XilkaSpi.h"
#include "XilkaSpiGraphics.h"

#if defined(ARDUINO_ARCH_AVR)
#include <avr/pgmspace.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <pgmspace.h>
#else // assuming an arm processor
#define PROGMEM
#define pgm_read_byte(addr) (*(const uint8_t *)(addr))
#define pgm_read_word(addr) (*(const uint16_t *)(addr))
#endif

namespace {

const uint8_t CMD_CASET(0x2A);
const uint8_t CMD_PASET(0x2B);
const uint8_t CMD_RAMWR(0x2C);

/*
 * Uno has 2K ram
 * Mega has 8K ram
 * Zero has 32K ram
 * ESP8266 has aproximately 36K of user available RAM
 * Teensy 3.1/3.2 have 64K ram
 * Due has 96K
 *
 */
#if ! defined(SPI_ACCELFILL16) && ! defined(SPI_ACCELFILL16ENDIAN)
#if defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_ARCH_SAMD)
/*
 * the Arduino Zero samd spi implementation is currently
 * weak (no optimised buffer transfers)
 * so don't waste a bunch of buffer space here
 */
// Mega and Zero
#define SCANLINE_SIZE 40
#elif defined(__arm__)
// Due and Teensy 3.x
#define SCANLINE_SIZE 240
#elif defined(ARDUINO_ARCH_AVR)
// Uno like boards - ATmega328P, ATmega32U4
#define SCANLINE_SIZE 20
#endif
#endif // ! defined(SPI_ACCELFILL16)

#if defined(SCANLINE_SIZE)

class ScanLine
{
public:
	ScanLine() : _currentColor(0), _currentLen(0) {}

	void write(uint16_t color, size_t len)
	{
		fill(color, len);

		const size_t numLoops(len / SCANLINE_SIZE);

		for ( size_t i = 0; i < numLoops; ++i )
			writeShort(SCANLINE_SIZE, i == numLoops - 1);

		const uint16_t remainingPixels(len % SCANLINE_SIZE);

		if ( remainingPixels )
			writeShort(remainingPixels, true);
	}

private:
	void fill(uint16_t color, size_t len)
	{
#if BYTE_ORDER == LITTLE_ENDIAN
		color = (color >> 8) | (color << 8);
#endif
		/* force refill if new color */
		if ( color != _currentColor )
			_currentLen = 0;

		if ( len > _currentLen && _currentLen < SCANLINE_SIZE )
		{
			size_t count(len < SCANLINE_SIZE ? len : SCANLINE_SIZE);
			count -= _currentLen;
			uint16_t *p(_data + _currentLen);

			while ( count-- )
				*p++ = color;

			_currentColor = color;
			_currentLen = len;
		}
	}

	void writeShort(size_t n, bool isLast)
	{
#if defined(ARDUINO_ARCH_ESP8266)
		SPI.writeBytes(reinterpret_cast<uint8_t*>(_data), n * 2);
#elif defined(SPI_MODE_DMA)
		dmaSend(_data, n); // DMA16 - each pixel is 2 bytes
#elif defined(SPI_ACCEL)
		Xilka::spiAccelWrite(reinterpret_cast<uint8_t*>(_data), n * 2, isLast);
#else
		SPI.transfer(reinterpret_cast<uint8_t*>(_data), n * 2);
#endif
	}

	uint16_t _data[SCANLINE_SIZE];
	uint16_t _currentColor;
	uint16_t _currentLen;
} scanLine;

#endif // defined(SCANLINE_SIZE)

const uint8_t DELAY(0x80);

} // namespace

namespace Xilka {

SpiGraphics::SpiGraphics(const GFX::Dim &dspDim
		, SPISettings spiSettings
		, int8_t resetPin, int8_t dcPin
		, int8_t csPin, SpiSoftWriter *spiSoft)
	: GFX(dspDim)
	, _spiSettings(spiSettings)
	, _reset(resetPin)
	, _dc(dcPin)
	, _cs(csPin)
	, _spiSoft(spiSoft)
{
}

void SpiGraphics::issueRomCommands(const uint8_t *addr)
{
	// Number of commands to follow
	uint8_t numCommands(pgm_read_byte(addr++));

	spiBeginTran();

	while ( numCommands-- )
	{
		spiSetData(false);
		spiWrite(pgm_read_byte(addr++), ! numCommands); // Read, issue command

		uint8_t numArgs(pgm_read_byte(addr++)); // Number of args to follow
		bool hasDelay(numArgs & DELAY); // If high bit set, delay follows args
		numArgs &= ~DELAY; // Mask out delay bit

		spiSetData(true);

		while ( numArgs-- )
			spiWrite(pgm_read_byte(addr++), ! numArgs); // Read, issue argument

		if ( hasDelay )
		{
			uint16_t ms(pgm_read_byte(addr++)); // Read post-command delay time (ms)

			if ( ms == 255 )
				ms = 500;

			delay(ms);
		}
	}

	spiEndTran();
}

void SpiGraphics::issueRamCommands(const uint8_t *cmds)
{
	// Number of commands to follow
	uint8_t numCommands(*cmds++);

	spiBeginTran();

	while ( numCommands-- )
	{
		spiSetData(false);
		spiWrite(*cmds++, ! numCommands); // Read, issue command

		uint8_t numArgs(*cmds++); // Number of args to follow
		bool hasDelay(numArgs & DELAY); // If high bit set, delay follows args
		numArgs &= ~DELAY; // Mask out delay bit

		spiSetData(true);

#if defined(SPI_ACCEL)
		spiAccelWrite(cmds, static_cast<size_t>(numArgs), ! numCommands);
		cmds += numArgs;
#else
		while ( numArgs-- )
			spiWrite(*cmds++, ! numArgs); // Read, issue argument
#endif

		if ( hasDelay )
		{
			uint16_t ms(*cmds++); // Read post-command delay time (ms)

			if ( ms == 255 )
				ms = 500;

			delay(ms);
		}
	}

	spiEndTran();
}

void SpiGraphics::pushColor(uint16_t color)
{
	spiBeginTran();
	spiSetData(true);

	if ( isHwSpi() )
	{
#if defined(SPI_ACCEL)
#if defined(SPI_ACCEL16)
		spiAccelWrite16(color, true);
#elif defined(SPI_ACCEL16ENDIAN)
		spiAccelWrite16Endian(color, true, true);
#else
		spiAccelWrite(color >> 8, false);
		spiAccelWrite(color & 0xFF, true);
#endif
#else
		spiWriteHard(color >> 8);
		spiWriteHard(color & 0xFF);
#endif
	}
	else
	{
		_spiSoft->write(color >> 8);
		_spiSoft->write(color & 0xFF);
	}

	spiEndTran();
}

void SpiGraphics::drawPixelNT(const GFX::Pos &p, uint16_t color)
{
	if ( (p.x >= 0) && ( p.x < getCurrentDim().w )
			&& (p.y >= 0) && (p.y < getCurrentDim().h) )
	{
		setWindow(p, GFX::Pos(p.x + 1, p.y + 1));

		spiSetData(true);

		if ( isHwSpi() )
		{
#if defined(SPI_ACCEL)
#if defined(SPI_ACCEL16)
#if defined(USE_TEENSY_FIFO)
			spiAccelWrite16(color, false);
#else
			spiAccelWrite16(color, true);
#endif
#elif defined(SPI_ACCEL16ENDIAN)
			spiAccelWrite16Endian(color, true, true);
#else
			spiAccelWrite(color >> 8, false);
			spiAccelWrite(color & 0xFF, true);
#endif
#else
			spiWriteHard(color >> 8);
			spiWriteHard(color & 0xFF);
#endif
		}
		else
		{
			_spiSoft->write(color >> 8);
			_spiSoft->write(color & 0xFF);
		}
	}
}

void SpiGraphics::drawFastVLineNT(const GFX::Pos &p
		, int16_t length, uint16_t color)
{
	if ( length && (p.x < getCurrentDim().w) && (p.y < getCurrentDim().h) )
	{
		// Simple clipping
		if ( (p.y + length - 1) >= getCurrentDim().h )
			length = getCurrentDim().h - p.y;

		setWindow(p, GFX::Pos(p.x, p.y + length - 1));

		spiSetData(true);

		if ( isHwSpi() )
		{
#if defined(SPI_ACCELFILL16)
			spiAccelFill16(color, length, true);
#elif defined(SPI_ACCELFILL16ENDIAN)
#if defined(USE_TEENSY_FIFO)
			spiAccelFill16Endian(color, length, true, false);
#else
			spiAccelFill16Endian(color, length, true, true);
#endif
#elif defined(SCANLINE_SIZE)
			scanLine.write(color, length);
#else
			const uint8_t hi(color >> 8);
			const uint8_t lo(color & 0xFF);

			while ( length-- )
			{
				spiWriteHard(hi);
				spiWriteHard(lo);
			}
#endif
		}
		else
		{
			const uint8_t hi(color >> 8);
			const uint8_t lo(color & 0xFF);

			while ( length-- )
			{
				_spiSoft->write(hi);
				_spiSoft->write(lo);
			}
		}

#if defined(ARDUINO_ARCH_ESP8266)
		yield();
#endif
	}
}

void SpiGraphics::drawFastHLineNT(const GFX::Pos &p
		, int16_t length, uint16_t color)
{
	if ( length && (p.x < getCurrentDim().w) && (p.y < getCurrentDim().h) )
	{
		// Simple clipping
		if ( (p.x + length - 1) >= getCurrentDim().w )
			length = getCurrentDim().w - p.x;

		setWindow(p, GFX::Pos(p.x + length - 1, p.y));

		spiSetData(true);

		if ( isHwSpi() )
		{
#if defined(SPI_ACCELFILL16)
			spiAccelFill16(color, length, true);
#elif defined(SPI_ACCELFILL16ENDIAN)
#if defined(USE_TEENSY_FIFO)
			spiAccelFill16Endian(color, length, true, false);
#else
			spiAccelFill16Endian(color, length, true, true);
#endif
#elif defined(SCANLINE_SIZE)
			scanLine.write(color, length);
#else
			const uint8_t hi(color >> 8);
			const uint8_t lo(color & 0xFF);

			while ( length-- )
			{
				spiWriteHard(hi);
				spiWriteHard(lo);
			}
#endif
		}
		else
		{
			const uint8_t hi(color >> 8);
			const uint8_t lo(color & 0xFF);

			while ( length-- )
			{
				_spiSoft->write(hi);
				_spiSoft->write(lo);
			}

		}

#if defined(ARDUINO_ARCH_ESP8266)
		yield();
#endif
	}
}

void SpiGraphics::fillRectNT(const GFX::Pos &p
		, const GFX::Dim &dIn, uint16_t color)
{
	if ( (p.x < getCurrentDim().w) && (p.y < getCurrentDim().h) )
	{
		GFX::Dim d(dIn);

		// rudimentary clipping (drawChar w/big text requires this)
		if ( (p.x + d.w - 1) >= getCurrentDim().w )
			d.w = getCurrentDim().w  - p.x;

		if ( (p.y + d.h - 1) >= getCurrentDim().h )
			d.h = getCurrentDim().h - p.y;

		setWindow(p, GFX::Pos(p.x + d.w - 1, p.y + d.h - 1));

		spiSetData(true);

		if ( isHwSpi() )
		{
#if defined(SPI_ACCELFILL16)
#if defined(__arm__)
			// size_t is 4 bytes
			spiAccelFill16(color, d.h*d.w, true);
#else // defined(__arm__)
			// size_t is 2 bytes
			{
				uint32_t count((uint32_t)d.h * (uint32_t)d.w);

				while ( count )
				{
					uint32_t n(count < 0xFFFF ? count : 0xFFFF);
					spiAccelFill16(color, n, true);
					count -= n;
				}
			}
#endif // defined(__arm__)
#elif defined(SPI_ACCELFILL16ENDIAN)
#if defined(USE_TEENSY_FIFO)
			spiAccelFill16Endian(color, d.h*d.w, true, false);
#else
			spiAccelFill16Endian(color, d.h*d.w, true, true);
#endif
#elif defined(SCANLINE_SIZE)
			scanLine.write(color, d.h*d.w);
#else
			const uint8_t hi(color >> 8);
			const uint8_t lo(color & 0xFF);
			size_t count(d.h*d.w);

			while ( count-- )
			{
				spiWriteHard(hi);
				spiWriteHard(lo);
			}
#endif
		}
		else
		{
			const uint8_t hi(color >> 8);
			const uint8_t lo(color & 0xFF);
			size_t count(d.h*d.w);

			while ( count-- )
			{
				_spiSoft->write(hi);
				_spiSoft->write(lo);
			}
		}

#if defined(ARDUINO_ARCH_ESP8266)
		yield();
#endif
	}
}

/*
 * Default to "normal" - most common
 */
void SpiGraphics::setWindow(const GFX::Pos &p1, const GFX::Pos &p2)
{
	const uint8_t CMD_CASET(0x2A);
	const uint8_t CMD_RASET(0x2B);
	const uint8_t CMD_RAMWR(0x2C);
	Data32 cas;
	Data32 ras;
	cas.w[0] = p1.x;
	cas.w[1] = p2.x;
	ras.w[0] = p1.y;
	ras.w[1] = p2.y;

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

/*
 * Default to "normal" - most common
 */
void SpiGraphics::invertDisplay(bool wantInverse)
{
	const uint8_t CMD_INVOFF(0x20);
	const uint8_t CMD_INVON(0x21);

	spiBeginTran();

	spiSetData(false);
	spiWrite(wantInverse ? CMD_INVON : CMD_INVOFF, true);

	spiEndTran();
}

void SpiGraphics::spiWrite(uint8_t d, bool isLast) const
{
	if ( isHwSpi() )
	{
#if defined(SPI_ACCEL)
		spiAccelWrite(d, isLast);
#else
		spiWriteHard(d);
#endif
	}
	else
	{
		_spiSoft->write(d);
	}
}

#if ! defined(SPI_HAS_TRANSACTION)
void SpiGraphics::spiWriteHard(uint8_t d) const
{
#if defined (SPI_HAS_TRANSACTION)
	SPI.transfer(d);
#elif defined (ARDUINO_ARCH_AVR)
	SPCRbackup = SPCR;
	SPCR = mySPCR;
	SPI.transfer(d);
	SPCR = SPCRbackup;
	// SPDR = c;
	// while(!(SPSR & _BV(SPIF)));
#elif defined (__arm__)
	SPI.setClockDivider(21); // 4MHz
	SPI.setDataMode(SPI_MODE0);
	SPI.transfer(d);
#endif
}
#endif // ! defined(SPI_HAS_TRANSACTION)

} // namespace Xilka

