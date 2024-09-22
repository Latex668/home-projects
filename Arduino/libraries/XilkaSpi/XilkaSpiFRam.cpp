#include <Arduino.h>
#include <SPI.h>

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

#include "XilkaSpiFRam.h"

namespace {

typedef enum opcodes_e
{
	OPCODE_WREN   = 0b0110,     /* Write Enable Latch */
	OPCODE_WRDI   = 0b0100,     /* Reset Write Enable Latch */
	OPCODE_RDSR   = 0b0101,     /* Read Status Register */
	OPCODE_WRSR   = 0b0001,     /* Write Status Register */
	OPCODE_READ   = 0b0011,     /* Read Memory */
	OPCODE_WRITE  = 0b0010,     /* Write Memory */
	OPCODE_RDID   = 0b10011111  /* Read Device ID */
} opcodes_t;

} // namespace

namespace Xilka {

SpiFRam::SpiFRam(int8_t csPin, SpiSoftReadWriter *spiSoft)
	: _spiSettings(20000000, MSBFIRST, SPI_MODE0) // 20 MHz
	, _cs(csPin)
	, _spiSoft(spiSoft)
{
}

bool SpiFRam::begin(void)
{
#if defined(ARDUINO_ARCH_ESP8266)
	if ( _cs.getPin() == -1 )
		SPI.setHwCs(true);
#endif

	/* Make sure we're actually connected */
	uint8_t mId;
	uint16_t pId;

	getDeviceId(mId, pId);

	return ( mId == 0x04 && pId == 0x0302 );
}

void SpiFRam::getDeviceId(uint8_t &mId, uint16_t &pId) const
{
/*
 * Read the Manufacturer ID and the Product ID from the IC
 *
 *   @params[out]  manufacturerID
 *                 The 8-bit manufacturer ID (Fujitsu = 0x04)
 *   @params[out]  productID
 *                 The memory density (bytes 15..8) and proprietary
 *                 Product ID fields (bytes 7..0). Should be 0x0302 for
 *                 the MB85RS64VPNF-G-JNERE1.
 */

	uint8_t a[4] = { 0, 0, 0, 0 };

	spiBegin();

	spiXfer(OPCODE_RDID);
	a[0] = spiXfer(0);
	a[1] = spiXfer(0);
	a[2] = spiXfer(0);
	a[3] = spiXfer(0);

	spiEnd();

	/* Shift values to separate manuf and prod IDs */
	/* See p.10 of http://www.fujitsu.com/downloads/MICRO/fsa/pdf/products/memory/fram/MB85RS64V-DS501-00015-4v0-E.pdf */
	mId = (a[0]);
	pId = ((a[2] << 8) + a[3]);
}

void SpiFRam::enableWrite(bool enable) const
{
	spiBegin();
	spiXfer(enable ? OPCODE_WREN : OPCODE_WRDI);
	spiEnd();
}

void SpiFRam::write(uint16_t addr, uint8_t value) const
{
	spiBegin();

	if ( isHwSpi() )
	{
#if defined(SPI_ACCEL)
		spiAccelWrite(OPCODE_WRITE, false);
#if defined(SPI_ACCEL16)
		spiAccelWrite16(addr, false);
#elif defined(SPI_ACCEL16ENDIAN)
		spiAccelWrite16Endian(addr, true, false);
#else
		spiAccelWrite((uint8_t)(addr >> 8), false);
		spiAccelWrite((uint8_t)(addr & 0xFF), false);
#endif
		spiAccelWrite(value, true);
#else // defined(SPI_ACCEL)
		SPI.transfer(OPCODE_WRITE);
		SPI.transfer((uint8_t)(addr >> 8));
		SPI.transfer((uint8_t)(addr & 0xFF));
		SPI.transfer(value);
#endif // defined(SPI_ACCEL)
	}
	else
	{
		_spiSoft->write(OPCODE_WRITE);
		_spiSoft->write((uint8_t)(addr >> 8));
		_spiSoft->write((uint8_t)(addr & 0xFF));
		_spiSoft->write(value);
	}

	spiEnd();
}

void SpiFRam::write(uint16_t addr
		, const uint8_t *source
		, size_t count) const
{
	spiBegin();

	if ( isHwSpi() )
	{
#if defined(SPI_ACCEL)
		spiAccelWrite(OPCODE_WRITE, false);
#if defined(SPI_ACCEL16)
		spiAccelWrite16(addr, false);
#elif defined(SPI_ACCEL16ENDIAN)
		spiAccelWrite16Endian(addr, true, false);
#else
		spiAccelWrite(addr >> 8, false);
		spiAccelWrite(addr & 0xFF, false);
#endif
#if defined(SPI_ACCELWRITEARRAY16ENDIAN)
		if ( ! ((intptr_t)source % 2) ) // if it's aligned
		{
			const size_t n(count / 2);

			if ( n )
			{
				spiAccelWriteArray16Endian(reinterpret_cast<const uint16_t*>(source)
						, n, IS_BIG_ENDIAN, true);

				const size_t nBytes(n * 2);
				source += nBytes;
				count -= nBytes;
			}
		}
#endif
		spiAccelWrite(source, count, true);
#else // defined(SPI_ACCEL)
		SPI.transfer(OPCODE_WRITE);
		SPI.transfer((uint8_t)(addr >> 8));
		SPI.transfer((uint8_t)(addr & 0xFF));

		const uint8_t *p(source);
		const uint8_t * const pEnd(p + count);

		while ( p != pEnd )
			SPI.transfer(*p++);
#endif // defined(SPI_ACCEL)
	}
	else
	{
		_spiSoft->write(OPCODE_WRITE);
		_spiSoft->write((uint8_t)(addr >> 8));
		_spiSoft->write((uint8_t)(addr & 0xFF));

		const uint8_t *p(source);
		const uint8_t * const pEnd(p + count);

		while ( p != pEnd )
			_spiSoft->write(*p++);
	}

	spiEnd();
}

uint8_t SpiFRam::read(uint16_t addr) const
{
	spiBegin();

	uint8_t x(0);

	if ( isHwSpi() )
	{
#if defined(SPI_ACCEL)
		spiAccelWrite(OPCODE_READ, true);
#if defined(SPI_ACCEL16)
		spiAccelWrite16(addr, true);
#elif defined(SPI_ACCEL16ENDIAN)
		spiAccelWrite16Endian(addr, true, true);
#else
		spiAccelWrite(addr >> 8, false);
		spiAccelWrite(addr & 0xFF, true);
#endif
#else // defined(SPI_ACCEL)
		SPI.transfer(OPCODE_READ);
		SPI.transfer((uint8_t)(addr >> 8));
		SPI.transfer((uint8_t)(addr & 0xFF));
#endif // defined(SPI_ACCEL)

		x = SPI.transfer(0);
	}
	else
	{
		_spiSoft->write(OPCODE_READ);
		_spiSoft->write((uint8_t)(addr >> 8));
		_spiSoft->write((uint8_t)(addr & 0xFF));

		x = _spiSoft->readWrite(0);
	}

	spiEnd();

	return x;
}

void SpiFRam::read(uint16_t addr
		, uint8_t *target
		, size_t count) const
{
	spiBegin();

	if ( isHwSpi() )
	{
#if defined(SPI_ACCEL)
		spiAccelWrite(OPCODE_READ, false);
#if defined(SPI_ACCEL16)
		spiAccelWrite16(addr, false);
#elif defined(SPI_ACCEL16ENDIAN)
		spiAccelWrite16Endian(addr, true, false);
#else
		spiAccelWrite(addr >> 8, false);
		spiAccelWrite(addr & 0xFF, false);
#endif
#if defined(SPI_ACCELREADARRAY16ENDIAN)
		if ( ! ((intptr_t)target % 2) ) // if it's aligned
		{
			const size_t n(count / 2);

			if ( n )
			{
				spiAccelReadArray16Endian(reinterpret_cast<uint16_t*>(target)
						, n, IS_BIG_ENDIAN);

				const size_t nBytes(n * 2);
				target += nBytes;
				count -= nBytes;
			}
		}
#endif
		spiAccelRead(target, count);
#else // defined(SPI_ACCEL)
		SPI.transfer(OPCODE_READ);
		SPI.transfer((uint8_t)(addr >> 8));
		SPI.transfer((uint8_t)(addr & 0xFF));

		uint8_t *p(target);
		uint8_t * const pEnd(p + count);

		while ( p != pEnd )
			*p++ = SPI.transfer(0);
#endif // defined(SPI_ACCEL)
	}
	else
	{
		_spiSoft->write(OPCODE_READ);
		_spiSoft->write((uint8_t)(addr >> 8));
		_spiSoft->write((uint8_t)(addr & 0xFF));

		uint8_t *p(target);
		uint8_t * const pEnd(p + count);

		while ( p != pEnd )
			*p++ = _spiSoft->readWrite(0);
	}

	spiEnd();
}

uint8_t SpiFRam::getStatusRegister(void) const
{
	spiBegin();

	spiXfer(OPCODE_RDSR);
	uint8_t reg(spiXfer(0));

	spiEnd();

	return reg;
}

void SpiFRam::setStatusRegister(uint8_t value) const
{
	spiBegin();

	spiXfer(OPCODE_WRSR);
	spiXfer(value);

	spiEnd();
}

} // namespace Xilka

