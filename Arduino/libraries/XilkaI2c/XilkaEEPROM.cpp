/*
 *  EEPROM 24LC64 class library
 *      should work with a huge swath of the EEPROMs
 *      that are available
 *
 *  Author: hkhijhe
 *  Date: 01/10/2010
 *
 *  2015.01.23 Made into a class library
 *  2015.02.06 Tested and refined
 *  2015.07.17 change XilkaEEPROM to Xilka::EEPROM
 *  2015.08.13 use size_t and uint16_t
 *  Kelly Anderson
 *
 */

/*
 * AT24C256 (ATMLH348) 32K EEPROM
 *
 * MicroChip 16K 24AA16/24LC16B 304176C
 */
#include <Arduino.h>
#include <Wire.h>
#include "XilkaEEPROM.h"

#if ARDUINO >= 100
#define WIRE_WRITE(x) Wire.write((uint8_t)(x))
#define WIRE_READ() Wire.read()
#else
#define WIRE_WRITE(x) Wire.send((uint8_t)(x))
#define WIRE_READ() Wire.receive()
#endif

#define MIN(a,b) ((a)<(b)?(a):(b))

/*
 * For proper operation we'll read/write in chunks
 * we need to keep out stride to (BUFFER_LENGTH - 4)
 * 4 is reserved for the read/write commands.
 *
 * Since we want to keep our STRIDE to an even divisor
 * of the EEPROM page size, this means that we are
 * stuck using 1/2 of the BUFFER_LENGTH max.
 */
#if (BUFFER_LENGTH >= 0x80)
#define WIRE_STRIDE 0x40
#else
#define WIRE_STRIDE (BUFFER_LENGTH/2)
#endif

namespace Xilka {

/*
 * This could be 1MHz if running at 5V
 */
unsigned long EEPROM::MaxI2CSpeed(400000UL);

/*
 * uS time to wait before starting new write
 */
#define SAFE_DELAY 5000

EEPROM::EEPROM()
	: _i2cAddr(0)
	, _lastAccess(micros() - SAFE_DELAY)
{
}

void EEPROM::accessDelay(void)
{
	const unsigned long previousWrite(_lastAccess);
	const unsigned long safeTime(previousWrite + SAFE_DELAY);

	if ( previousWrite > micros() )
	{
		// handle uS wrap which happens every 70 min
		delayMicroseconds(SAFE_DELAY);
	}
	else if ( safeTime > _lastAccess )
		delayMicroseconds(safeTime - _lastAccess);
}

bool EEPROM::begin(uint8_t i2cAddr)
{
	_i2cAddr = i2cAddr;

    Wire.beginTransmission(_i2cAddr);
	WIRE_WRITE(0); // MSB
	WIRE_WRITE(0); // LSB
	Wire.endTransmission();

	Wire.requestFrom(_i2cAddr, (uint8_t)1);

	return Wire.available();
}

void EEPROM::write(uint16_t address, uint8_t data)
{
	accessDelay();

	Wire.beginTransmission(_i2cAddr);
	WIRE_WRITE(address >> 8); // MSB
	WIRE_WRITE(address & 0xFF); // LSB
	WIRE_WRITE(data);
	Wire.endTransmission();

	_lastAccess = micros();
}

void EEPROM::write(uint16_t address
	, const uint8_t *source, size_t count)
{
	const uint16_t nextStrideBoundary((address & ~(WIRE_STRIDE - 1))
											+ WIRE_STRIDE);
	size_t bytesToNextStrideBoundary(nextStrideBoundary - address);

	/*
	 * write bytes until we hit a stride boundary
	 */
	while ( bytesToNextStrideBoundary )
	{
		const size_t nWritten(writeStride(address, source
							, MIN(count, bytesToNextStrideBoundary)));

		address += nWritten;
		source += nWritten;
		bytesToNextStrideBoundary -= nWritten;
		count -= nWritten;

		if ( ! count )
			break;
	}

	/*
	 * it's aligned, so let's go
	 */
	while ( count )
	{
		const size_t nWritten(writeStride(address, source, count));
		address += nWritten;
		source += nWritten;
		count -= nWritten;
	}
}

size_t EEPROM::writeStride(uint16_t address
	, const uint8_t *source, size_t count)
{
	accessDelay();

	Wire.beginTransmission(_i2cAddr);
	WIRE_WRITE(address >> 8); // MSB
	WIRE_WRITE(address & 0xFF); // LSB

	const size_t nWrite(MIN(count, WIRE_STRIDE));

	for (size_t i = 0; i < nWrite; ++i )
		WIRE_WRITE(source[i]);

	Wire.endTransmission();

	_lastAccess = micros();

	return nWrite;
}

uint8_t EEPROM::read(uint16_t address)
{
	accessDelay();

	Wire.beginTransmission(_i2cAddr);
	WIRE_WRITE(address >> 8); // MSB
	WIRE_WRITE(address & 0xFF); // LSB
	Wire.endTransmission();

	Wire.requestFrom(_i2cAddr, (uint8_t)1);

	uint8_t ret(Wire.available() ? WIRE_READ() : 0xFF);

	_lastAccess = micros();

	return ret;
}

void EEPROM::read(uint16_t address
	, uint8_t *target, size_t count)
{
	const uint16_t nextStrideBoundary((address & ~(WIRE_STRIDE - 1))
											+ WIRE_STRIDE);
	size_t bytesToNextStrideBoundary(nextStrideBoundary - address);

	/*
	 * read bytes until we hit a stride boundary
	 */
	while ( bytesToNextStrideBoundary )
	{
		const size_t nRead(readStride(address, target
							, MIN(count, bytesToNextStrideBoundary)));

		address += nRead;
		target += nRead;
		bytesToNextStrideBoundary -= nRead;
		count -= nRead;

		if ( ! count )
			break;
	}

	/*
	 * it's aligned, so let's go
	 */
	while ( count )
	{
		const size_t nRead(readStride(address, target, count));
		address += nRead;
		target += nRead;
		count -= nRead;
	}
}

size_t EEPROM::readStride(uint16_t address
	, uint8_t *target, size_t count)
{
	accessDelay();

	Wire.beginTransmission(_i2cAddr);
	WIRE_WRITE(address >> 8); // MSB
	WIRE_WRITE(address & 0xFF); // LSB
	Wire.endTransmission();

	Wire.requestFrom(_i2cAddr, (uint8_t)count);

	const size_t nRead(MIN(count, WIRE_STRIDE));

	for ( size_t i = 0; i < nRead; ++i )
		if ( Wire.available() )
			target[i] = WIRE_READ();

	_lastAccess = micros();

	return nRead;
}

} // namespace Xilka

