#pragma once
/*
 * MicroChip 24LC32 4K EEPROM
 *
 * 2015.08.29 Kelly Anderson
 *
 */

#include "XilkaI2c.h"
#include "XilkaEEPROM.h"

namespace Xilka {

class EEPROM24LC32 : public EEPROM
{
public:
	EEPROM24LC32() : EEPROM() {}

	static unsigned long MaxI2CSpeed;
};

} // namespace Xilka

