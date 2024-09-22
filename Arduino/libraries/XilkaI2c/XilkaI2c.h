#pragma once

#include <Arduino.h>

#if defined(__SAM3X8E__) || defined(ARDUINO_AVR_MEGA2560)
/* Use the UNO R3 SDL/SCL compatible pins */
#define Wire Wire1
#endif

namespace Xilka {
namespace I2C {

int getLastError(void);
const char *getLastErrorMessage(void);

bool sendCommand(uint8_t i2cAddr, uint8_t address, bool stop=true);

bool read(uint8_t i2cAddr, uint8_t address, void *result, size_t count, bool stop = true, int dly = 0);
bool readUInt8(uint8_t i2cAddr, uint8_t address, uint8_t &result, bool stop = true);

inline bool readUInt16(uint8_t i2cAddr, uint8_t address, uint16_t &result, bool stop = true, int dly = 0)
{
	uint16_t tmp(0);

	bool status(read(i2cAddr, address, &tmp, sizeof(tmp), stop, dly));

	if ( status )
		result = tmp;

	return status;
}

// little endian
inline bool readUInt16LE(uint8_t i2cAddr, uint8_t address, uint16_t &result, bool stop = true, int dly = 0)
{
	uint16_t tmp(0);

	bool status(readUInt16(i2cAddr, address, tmp, stop, dly));

	if ( status )
		result = (tmp >> 8 | tmp << 8);

	return status;
}

bool write(uint8_t i2cAddr, uint8_t address, const void *source, size_t count, bool stop = true);
bool writeUInt8(uint8_t i2cAddr, uint8_t address, uint8_t value, bool stop = true);

inline bool writeUInt16(uint8_t i2cAddr, uint8_t address, uint16_t value, bool stop = true)
{
	return write(i2cAddr, address, &value, sizeof(value), stop);
}

// little endian
inline bool writeUInt16LE(uint8_t i2cAddr, uint8_t address, uint16_t value, bool stop = true)
{
	uint16_t tmp(value << 8 | value >> 8);

	return writeUInt16(i2cAddr, address, tmp, stop);
}

} // namespace I2C
} // namespace Xilka

