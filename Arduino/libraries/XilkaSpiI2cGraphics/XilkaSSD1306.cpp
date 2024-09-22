#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#if defined(ARDUINO_ARCH_ESP8266)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif
#if defined(ARDUINO_ARCH_AVR)
#include <util/delay.h>
#endif
#include <stdlib.h>

#define IS_BIG_ENDIAN true
#if defined(__arm__)
#undef IS_BIG_ENDIAN
#include <machine/endian.h>
#if BYTE_ORDER == LITTLE_ENDIAN
#define IS_BIG_ENDIAN false
#else
#define IS_BIG_ENDIAN true
#endif
#endif // defined(__arm__)

#include <XilkaGFX.h>

#include "../XilkaSpi/XilkaSpi.h"
#include "XilkaSSD1306.h"
#include "XilkaSSD1306Commands.h"

namespace {

const uint8_t REG_CONTROL(0x00);   // Co = 0, D/C = 0

#if ! defined(SPI_HAS_TRANSACTION) && defined (ARDUINO_ARCH_AVR)
uint8_t SPCRbackup;
uint8_t mySPCR;
#endif

} // namespace

#if defined(swap)
#undef swap
#endif

#if defined(ARDUINO_ARCH_ESP8266)
#define swap(a, b) { auto t(a); a = b; b = t; }
#else
#define swap(a, b) { typeof(a) t = a; a = b; b = t; }
#endif

namespace Xilka {

const unsigned long SSD1306::MaxI2CSpeed(400000UL);

SSD1306::SSD1306(const GFX::Dim &dim
		, int8_t resetPin
		, int8_t dcPin
		, int8_t csPin
		, SpiSoftWriter *spiSoft)
	: GFX(dim)
	, _bufferSize(dim.w * dim.h / 8)
	, _buffer(new uint8_t[_bufferSize])
		/*
		 * 10 MHz
		 * determined from spec sheet page 52
		 * min tCycle time of 100nS
		 */
	, _spiSettings(10000000, MSBFIRST, SPI_MODE0)
	, _reset(resetPin)
	, _dc(dcPin)
	, _cs(csPin)
	, _spiSoft(spiSoft)
	, _useChargePump(true)
	, _i2cAddr(0x00)
{
}

SSD1306::~SSD1306()
{
	delete[] _buffer;
}

void SSD1306::begin(bool useChargePump, uint8_t i2cAddr, bool reset)
{
	_useChargePump = useChargePump;
	_i2cAddr = i2cAddr;

	/*
	 * reset is high for normal operations
	 */
	if ( _reset.getPin() != -1 )
		_reset.setHigh();

	if ( isSpi() )
	{
		/*
		 * cs is high for normal operations
		 */
		if ( _cs.getPin() != -1 )
			_cs.setHigh();

		if ( isHwSpi() )
		{
#if defined(ARDUINO_ARCH_ESP8266)
			if ( _cs.getPin() == -1 )
				SPI.setHwCs(true);
#elif ! defined (SPI_HAS_TRANSACTION) && defined (ARDUINO_ARCH_AVR)
			SPCRbackup = SPCR;
			SPI.setClockDivider(SPI_CLOCK_DIV2); // typically 8MHz
			SPI.setDataMode(SPI_MODE0);
			mySPCR = SPCR; // save our preferred state
			//Serial.print("mySPCR = 0x"); Serial.println(SPCR, HEX);
			SPCR = SPCRbackup;  // then restore
#endif
		}
	}

	if ( reset && _reset.getPin() != -1 )
	{
		delay(10);
		_reset.setLow();
		delay(10);
		_reset.setHigh();
		// turn on VCC (9V?)
	}

	// make sure the device has had enough time after reset
	delay(100);
}

void SSD1306::invertDisplay(bool wantInverted)
{
	spiBeginTran();
	spiSetData(false);
	issueCommand(wantInverted ? CMD_INVERTDISPLAY : CMD_NORMALDISPLAY);
	spiEndTran();
}

void SSD1306::startScroll(bool isRight, uint8_t start, uint8_t stop) const
{
	uint8_t cmds[] =
	{
		0xFF // direction
		, 0X00
		, 0xFF // start
		, 0X00
		, 0xFF // stop
		, 0X00
		, 0XFF
		, CMD_ACTIVATE_SCROLL
	};

	cmds[0] = ( isRight )
				? CMD_RIGHT_HORIZONTAL_SCROLL
				: CMD_LEFT_HORIZONTAL_SCROLL;
	cmds[2] = start;
	cmds[4] = stop;

	const uint8_t *cmdEnd(&cmds[0] + sizeof(cmds));

	spiBeginTran();
	spiSetData(false);

#if defined(SPI_ACCEL)
	if ( isSpi() && isHwSpi() )
		spiAccelWrite(cmds, sizeof(cmds), true);
	else
#endif
	{
		for ( const uint8_t *cmd = &cmds[0]; cmd != cmdEnd; ++cmd )
			issueCommand(*cmd);
	}

	spiEndTran();
}

void SSD1306::startScrollDiag(bool isRight, uint8_t start, uint8_t stop) const
{
	uint8_t cmds[] =
	{
		CMD_SET_VERTICAL_SCROLL_AREA
		, 0X00
		, (uint8_t)getRawDim().h
		, 0xFF // direction
		, 0X00
		, 0xFF // start
		, 0X00
		, 0xFF // stop
		, 0X01
		, CMD_ACTIVATE_SCROLL
	};

	cmds[3] = ( isRight )
				? CMD_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL
				: CMD_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL;
	cmds[5] = start;
	cmds[7] = stop;

	const uint8_t *cmdEnd(&cmds[0] + sizeof(cmds));

	spiBeginTran();
	spiSetData(false);

#if defined(SPI_ACCEL)
	if ( isSpi() && isHwSpi() )
		spiAccelWrite(cmds, sizeof(cmds), true);
	else
#endif
	{
		for ( const uint8_t *cmd = &cmds[0]; cmd != cmdEnd; ++cmd )
			issueCommand(*cmd);
	}

	spiEndTran();
}

void SSD1306::stopScroll(void) const
{
	spiBeginTran();
	spiSetData(false);
	issueCommand(CMD_DEACTIVATE_SCROLL);
	spiEndTran();
}

void SSD1306::dimDisplay(bool wantDim) const
{
	const uint8_t contrast
			= ( wantDim )
				? 0
				: ( ( ! _useChargePump )
					? 0x9F
					: 0xCF );

	/*
	 * the range of contrast to too small to be really useful
	 * it is useful to dim the display
	 */
	spiBeginTran();
	spiSetData(false);
	issueCommand(CMD_SETCONTRAST);
	issueCommand(contrast);
	spiEndTran();
}

void SSD1306::display(void) const
{
	uint8_t pageEndAddress( (getRawDim().h == 64)
							? 0x07
							: (getRawDim().h == 32)
								? 0x03
								: 0x01);
	const uint8_t cmds[] =
	{
		CMD_COLUMNADDR
		, 0x00 // Column start address (0 = reset)
		, (uint8_t)(getRawDim().w - 1) // Column end address (127 = reset)
		, CMD_PAGEADDR
		, 0x00 // Page start address (0 = reset)
		, pageEndAddress
	};

	spiBeginTran();
	spiSetData(false);

#if defined(SPI_ACCEL)
	if ( isSpi() && isHwSpi() )
		spiAccelWrite(cmds, sizeof(cmds), true);
	else
#endif
	{
		const uint8_t *cmdEnd(&cmds[0] + sizeof(cmds));

		for ( const uint8_t *cmd = &cmds[0]; cmd != cmdEnd; ++cmd )
			issueCommand(*cmd);
	}

#if defined(ARDUINO_ARCH_ESP8266)
	yield();
#endif

	spiSetData(true);

	if ( isSpi() )
	{
		if ( isHwSpi() )
		{
#if defined(SPI_ACCEL)
#if defined(SPI_ACCELWRITEARRAY16ENDIAN)
			spiAccelWriteArray16Endian(reinterpret_cast<const uint16_t*>(_buffer)
							, _bufferSize/2, IS_BIG_ENDIAN, true);
#else
			spiAccelWrite(_buffer, _bufferSize, true);
#endif
#else
			const uint8_t *p(_buffer);
			const uint8_t * const pEnd(p + _bufferSize);

			while ( p != pEnd )
				spiWriteHard(*p++);
#endif
		}
		else
		{
			const uint8_t *p(_buffer);
			const uint8_t * const pEnd(p + _bufferSize);

			while ( p != pEnd )
				_spiSoft->write(*p++);
		}
	}
	else // I2C
	{
		const uint8_t *p(_buffer);
		const uint8_t * const pEnd(p + _bufferSize);

		while ( p != pEnd )
		{
			Wire.beginTransmission(_i2cAddr);
			Wire.write(0x40);

			for ( int x = 0; x < 16; ++x )
				Wire.write(*p++);

			--p;
			Wire.endTransmission();
		}
	}

	spiEndTran();

#if defined(ARDUINO_ARCH_ESP8266)
	yield();
#endif
}

void SSD1306::issueCommand(uint8_t c) const
{
	if ( isSpi() )
	{
		if ( isHwSpi() )
		{
			spiWriteHard(c);
		}
		else
		{
			_spiSoft->write(c);
		}

	}
	else // I2C
	{
		Wire.beginTransmission(_i2cAddr);
		Wire.write(REG_CONTROL);
		Wire.write(c);
		Wire.endTransmission();
	}

#if defined(ARDUINO_ARCH_ESP8266)
	yield();
#endif
}

#if ! defined(SPI_ACCEL) && ! defined(SPI_HAS_TRANSACTION)
void SSD1306::spiWriteHard(uint8_t d) const
{
#if defined(SPI_HAS_TRANSACTION)
	SPI.transfer(d);
#elif defined (ARDUINO_ARCH_AVR)
	SPCRbackup = SPCR;
	SPCR = mySPCR;
	SPI.transfer(d);
	SPCR = SPCRbackup;
	// SPDR = c;
	// while(!(SPSR & _BV(SPIF)));
#elif defined(__arm__)
	SPI.setClockDivider(21); // 4 MHz
	SPI.setDataMode(SPI_MODE0);
	SPI.transfer(d);
#endif
}
#endif // ! defined(SPI_HAS_TRANSACTION)

void SSD1306::clearDisplay(void)
{
	memset(_buffer, 0, _bufferSize);
}

void SSD1306::drawPixelNT(const GFX::Pos &pIn, uint16_t color)
{
	if ( (pIn.x >= 0) && (pIn.x < getCurrentDim().w)
			&& (pIn.y >= 0) && (pIn.y < getCurrentDim().h) )
	{
		GFX::Pos p(pIn);

		// check rotation, move pixel around if necessary
		switch ( getRotation() )
		{
		case 1:
			swap(p.x, p.y);
			p.x = getRawDim().w - p.x - 1;
			break;
		case 2:
			p.x = getRawDim().w - p.x - 1;
			p.y = getRawDim().h - p.y - 1;
			break;
		case 3:
			swap(p.x, p.y);
			p.y = getRawDim().h - p.y - 1;
			break;
		}

		// x is which column
		switch ( color )
		{
		case WHITE:
			_buffer[p.x + (p.y/8)*getRawDim().w] |=  (1 << (p.y&7));
			break;

		case BLACK:
			_buffer[p.x + (p.y/8)*getRawDim().w] &= ~(1 << (p.y&7));
			break;

		case INVERSE:
			_buffer[p.x + (p.y/8)*getRawDim().w] ^=  (1 << (p.y&7));
			break;
		}
	}
}

void SSD1306::fillRectNT(const GFX::Pos &p
	, const GFX::Dim &d, uint16_t color)
{
	for ( int16_t i = p.x; i < p.x + d.w; ++i )
		drawFastVLineNT(GFX::Pos(i, p.y), d.h, color);
}

void SSD1306::drawFastHLineNT(const GFX::Pos &pIn
		, int16_t length, uint16_t color)
{
	GFX::Pos p(pIn);
	bool bSwap(false);

	switch ( rotation )
	{
	case 0:
		// 0 degree rotation, do nothing
		break;
	case 1:
		// 90 degree rotation, swap x & y for rotation, then invert x
		bSwap = true;
		swap(p.x, p.y);
		p.x = getRawDim().w - p.x - 1;
		break;
	case 2:
		// 180 degree rotation, invert x and y - then shift y around for height.
		p.x = getRawDim().w - p.x - 1;
		p.y = getRawDim().h - p.y - 1;
		p.x -= (length-1);
		break;
	case 3:
		/*
		 * 270 degree rotation, swap x & y for rotation,
		 * then invert y  and adjust y for w (not to become h)
		 */
		bSwap = true;
		swap(p.x, p.y);
		p.y = getRawDim().h - p.y - 1;
		p.y -= (length-1);
		break;
	}

	( bSwap )
		? drawFastVLineLocal(p, length, color)
		: drawFastHLineLocal(p, length, color);
}

void SSD1306::drawFastVLineNT(const GFX::Pos &pIn
		, int16_t length, uint16_t color)
{
	GFX::Pos p(pIn);
	bool bSwap(false);

	switch ( rotation )
	{
	case 0:
		break;
	case 1:
		/* 90 degree rotation, swap x & y for rotation,
		 * then invert x and adjust x for h (now to become w)
		 */
		bSwap = true;
		swap(p.x, p.y);
		p.x = getRawDim().w - p.x - 1;
		p.x -= (length-1);
		break;
	case 2:
		/*
		 * 180 degree rotation, invert x and y,
		 * then shift y around for height.
		 */
		p.x = getRawDim().w - p.x - 1;
		p.y = getRawDim().h - p.y - 1;
		p.y -= (length-1);
		break;
	case 3:
		/*
		 * 270 degree rotation, swap x & y for rotation,
		 * then invert y
		 */
		bSwap = true;
		swap(p.x, p.y);
		p.y = getRawDim().h - p.y - 1;
		break;
	}

	( bSwap )
		? drawFastHLineLocal(p, length, color)
		: drawFastVLineLocal(p, length, color);
}

void SSD1306::drawFastVLineLocal(const GFX::Pos &pIn
	, int16_t length, uint16_t color)
{
	if ( pIn.x >= 0 && pIn.x < getRawDim().w )
	{
		GFX::Pos p(pIn);

		// make sure we don't try to draw below 0
		if ( p.y < 0 )
		{
			/*
			 * p.y is negative, this will subtract enough
			 * from length to account for p.y being 0
			 */
			length += p.y;
			p.y = 0;
		}

		// make sure we don't go past the height of the display
		if ( (p.y + length) > getRawDim().h )
			length = (getRawDim().h - p.y);

		// if length is now negative, punt
		if ( length <= 0 )
			return;

		/*
		 * this display doesn't need ints for coordinates,
		 */
		uint8_t y(p.y);
		uint8_t h(length);

		// set up the pointer for fast movement through the buffer
		uint8_t *pBuf(_buffer);
		// adjust the buffer pointer for the current row
		pBuf += ((y/8) * getRawDim().w);
		// and offset x columns in
		pBuf += p.x;

		/*
		 * do the first partial byte,
		 * if necessary - this requires some masking
		 */
		uint8_t mod(y&7);

		if ( mod )
		{
			// mask off the high n bits we want to set
			mod = 8 - mod;

			/*
			 * note - lookup table results in a nearly
			 * 10% performance improvement in fill* functions
			 */
			// uint8_t mask = ~(0xFF >> (mod));
			static const uint8_t premask[8] =
				{ 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
			uint8_t mask(premask[mod]);

			/*
			 * adjust the mask if we're not going to reach
			 * the end of this byte
			 */
			if ( h < mod )
				mask &= (0XFF >> (mod-h));

			switch ( color )
			{
			case WHITE:
				*pBuf |=  mask;
				break;
			case BLACK:
				*pBuf &= ~mask;
				break;
			case INVERSE:
				*pBuf ^=  mask;
				break;
			}

			// fast exit if we're done here!
			if ( h < mod )
				return;

			h -= mod;

			pBuf += getRawDim().w;
		}

		/*
		 * write solid bytes while we can
		 * effectively doing 8 rows at a time
		 */
		if ( h >= 8 )
		{
			/*
			 * separate copy of the code so we don't impact
			 * performance of the black/white write version
			 * with an extra comparison per loop
			 */
			if ( color == INVERSE )
			{
				do
				{
					*pBuf = ~(*pBuf);

					// adjust the buffer forward 8 rows worth of data
					pBuf += getRawDim().w;

					/*
					 * adjust h & y (there's got to be a faster way
					 * for me to do this, but this should still help
					 * a fair bit for now)
					 */
					h -= 8;
				} while ( h >= 8 );
			}
			else
			{
				// store a local value to work with
				uint8_t val((color == WHITE) ? 255 : 0);

				do
				{
					// write our value in
					*pBuf = val;

					// adjust the buffer forward 8 rows worth of data
					pBuf += getRawDim().w;

					/*
					 * adjust h & y (there's got to be a faster way
					 * for me to do this, but this should still help
					 * a fair bit for now)
					 */
					h -= 8;
				} while ( h >= 8 );
			}
		}

		// now do the final partial byte, if necessary
		if ( h )
		{
			mod = h & 7;
			/*
			 * this time we want to mask the low bits of the byte,
			 * vs the high bits we did above
			 */
			// uint8_t mask = (1 << mod) - 1;
			/*
			 * note - lookup table results in a nearly 10%
			 * performance improvement in fill* functions
			 */
			static const uint8_t postmask[8] =
				{0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
			const uint8_t mask(postmask[mod]);

			switch ( color )
			{
			case WHITE:
				*pBuf |= mask;
				break;
			case BLACK:
				*pBuf &= ~mask;
				break;
			case INVERSE:
				*pBuf ^= mask;
				break;
			}
		}
	}
}

void SSD1306::drawFastHLineLocal(const GFX::Pos &pIn
	, int16_t length, uint16_t color)
{
	// Do bounds/limit checks
	if ( pIn.y >= 0 && pIn.y < getRawDim().h )
	{
		GFX::Pos p(pIn);

		// make sure we don't try to draw below 0
		if ( p.x < 0 )
		{
			length += p.x;
			p.x = 0;
		}

		// make sure we don't go off the edge of the display
		if ( (p.x + length) > getRawDim().w )
			length = (getRawDim().w - p.x);

		// if our width is now negative, punt
		if ( length <= 0 )
			return;

		// set up the pointer for  movement through the buffer
		uint8_t *pBuf(_buffer);
		// adjust the buffer pointer for the current row
		pBuf += ((p.y/8) * getRawDim().w);
		// and offset x columns in
		pBuf += p.x;

		uint8_t mask(1 << (p.y&7));

		switch ( color )
		{
		case WHITE:
			while ( length-- )
				*pBuf++ |= mask;

			break;
		case BLACK:
			mask = ~mask;

			while ( length-- )
				*pBuf++ &= mask;

			break;
		case INVERSE:
			while ( length-- )
				*pBuf++ ^= mask;

			break;
		}
	}
}

} // namespace Xilka

