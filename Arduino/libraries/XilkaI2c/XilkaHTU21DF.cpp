
#include <Arduino.h>
#include <Wire.h>

#include "XilkaI2c.h"
#include "XilkaHTU21DF.h"

using namespace Xilka;

namespace {

const uint8_t I2C_ADDR(0x40);

const uint8_t REG_TEMP_HOLD_MASTER(0xE3);
const uint8_t REG_HUM_HOLD_MASTER(0xE5);
//const uint8_t REG_TEMP_NO_HOLD_MASTER(0xE3);
//const uint8_t REG_HUM_NO_HOLD_MASTER(0xE5);
// const uint8_t REG_WRITE(0xE6);
const uint8_t REG_STATUS(0xE7);
const uint8_t REG_RESET(0xFE);

bool checkUInt16Crc(const uint16_t &data, uint8_t crc);

bool readUInt16Crc(uint8_t i2cAddr
		, uint8_t address
		, uint16_t &result)
{
	Wire.beginTransmission(i2cAddr);
	Wire.write(address);
	Wire.endTransmission();

	delay(50); // from spec sheet

	Wire.requestFrom(i2cAddr, (uint8_t)3);

	for ( int counter = 0; Wire.available() < 3; ++counter )
	{
		delay(1);

		if ( counter > 99 )
			return false;
	}

	result = (((uint16_t)Wire.read()) << 8) | Wire.read();

	return checkUInt16Crc(result, Wire.read());
}

bool checkUInt16Crc(const uint16_t &data, uint8_t crc)
{
/*
 * Test cases from datasheet:
 * message = 0xDC, checkvalue is 0x79
 * message = 0x683A, checkvalue is 0x7C
 * message = 0x4E85, checkvalue is 0x6B
 */

// This is the 0x0131 polynomial shifted
// to farthest left of three bytes
#define SHIFTED_DIVISOR 0x988000

	/* combine data and crc into xxddddcc */
	uint32_t remainder((((uint32_t)data << 8)) | crc);

	uint32_t divisor((uint32_t)SHIFTED_DIVISOR);

	/*
	 * Operate on only 16 positions of max 24.
	 * The remaining 8 are our remainder
	 * and should be zero when we're done.
	 */
	for ( int i = 0; i < 16; ++i )
	{
		// Check if there is a one in the left position
		if ( remainder & (uint32_t)1<<(23 - i) )
			remainder ^= divisor;

		// Rotate the divisor max 16 times
		// so that we have 8 bits left of a remainder
		divisor >>= 1;
	}

	/*
	 * only the last 8 bits are significant
	 * to determine crc success
	 */

	return ! (remainder & 0xff);
}

} // namespace

namespace Xilka {

unsigned long HTU21DF::MaxI2CSpeed(400000UL);

bool HTU21DF::begin(void)
{
	I2C::sendCommand(I2C_ADDR, REG_RESET);
	delay(15); // from spec sheet

	uint8_t id(0);

	_isAvailable = I2C::readUInt8(I2C_ADDR, REG_STATUS, id)
					&& (id == 0x02);

	return _isAvailable;
}

float HTU21DF::getTempC(void) const
{
	uint16_t raw(0);

	if ( readUInt16Crc(I2C_ADDR, REG_TEMP_HOLD_MASTER, raw) )
	{
		raw &= 0xfffc; // clear the status bits

		return (((float)raw * 175.72) / 65536.0) - 46.85;
	}

	return -999.0;
}

float HTU21DF::getHumidity(void) const
{
	uint16_t raw(0);

	if ( readUInt16Crc(I2C_ADDR, REG_HUM_HOLD_MASTER, raw) )
	{
		raw &= 0xfffc; // clear the status bits

		return (((float)raw * 125.0) / 65536.0) - 6.0;
	}

	return -999.0;
}

} // namespace Xilka

