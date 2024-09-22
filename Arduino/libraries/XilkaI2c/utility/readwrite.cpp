#include <Arduino.h>
#include <Wire.h>

#include "../XilkaI2c.h"
#include "error.h"

namespace Xilka {
namespace I2C {

int lastI2CError;

bool sendCommand(uint8_t i2cAddr, uint8_t address, bool stop)
{
	Wire.beginTransmission(i2cAddr);
	Wire.write(address);
	return Wire.endTransmission(stop);
}

bool read(uint8_t i2cAddr, uint8_t address, void *result, size_t count, bool stop, int dly)
{
	Wire.beginTransmission(i2cAddr);
	Wire.write(address);

	if ( ! (lastI2CError = Wire.endTransmission(stop)) )
	{
		if ( dly )
			delay(dly);

		Wire.requestFrom(i2cAddr, count);

		size_t nRead(0);
		int timeoutCounter(0);
		uint8_t *p((uint8_t*)result);

		while ( nRead < count )
		{
			if ( Wire.available() )
			{
				timeoutCounter = 0;
				p[nRead++] = Wire.read();
			}
			else
			{
				if ( ++timeoutCounter > 99 )
				{
					lastI2CError = I2C_TIMEOUT;
					return false;
				}

				delay(1);
			}
		}

		return true;
	}

	return false;
}

bool readUInt8(uint8_t i2cAddr, uint8_t address, uint8_t &result, bool stop)
{
	Wire.beginTransmission(i2cAddr);
	Wire.write(address);

	if ( ! (lastI2CError = Wire.endTransmission(stop)) )
	{
		Wire.requestFrom(i2cAddr, sizeof(result));

		if ( static_cast<size_t>(Wire.available()) >= sizeof(result) )
		{
			result = Wire.read();
			return true;
		}
	}

	lastI2CError = I2C_TIMEOUT;
	return false;
}

bool write(uint8_t i2cAddr, uint8_t address, const void *source, size_t count, bool stop)
{
	Wire.beginTransmission(i2cAddr);
	Wire.write(address);

	const uint8_t *p((uint8_t*)source);

	for (size_t i = 0; i < count; ++i )
		Wire.write(*p++);

	return ! (lastI2CError = Wire.endTransmission(stop));
}

bool writeUInt8(uint8_t i2cAddr, uint8_t address, uint8_t value, bool stop)
{
	Wire.beginTransmission(i2cAddr);
	Wire.write(address);
	Wire.write(value);
	return ! (lastI2CError = Wire.endTransmission(stop));
}

} // namespace I2C
} // namespace Xilka

