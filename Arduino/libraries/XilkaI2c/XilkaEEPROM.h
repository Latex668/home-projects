#pragma once
/*
 *  EEPROM 24LC64 I2C library
 *
 *  Author: hkhijhe
 *  Date: 01/10/2010
 *
 *  Kelly Anderson
 *  2015.01.23 Made into a class library
 *  2015.07.17 changes XilkaEEPROM to Xilka::EEPROM
 *  2015.08.18 cleanup, adopt common approach
 *      to my other i2c libraries
 *
 */

#include "XilkaI2c.h"

namespace Xilka {

class EEPROM
{
public:
	EEPROM();
	bool begin(uint8_t i2cAddr = 0x50);

	void write(uint16_t address, uint8_t data);
	void write(uint16_t address, const uint8_t *source, size_t count);
	uint8_t read(uint16_t address);
	void read(uint16_t address, uint8_t *target, size_t count);

	static unsigned long MaxI2CSpeed;

private:
	size_t writeStride(uint16_t address, const uint8_t *data, size_t length);
	size_t readStride(uint16_t address, uint8_t *data, size_t length);
	void accessDelay(void);

	uint8_t _i2cAddr;
	unsigned long _lastAccess; // track uS
};

} // namespace Xilka

