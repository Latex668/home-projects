#include <Arduino.h>

#include "XilkaI2c.h"
#include "XilkaMCP9808.h"

namespace {

const uint8_t REG_CFG(0x01);
const uint8_t REG_TEMP_UPPER(0x02);
const uint8_t REG_TEMP_LOWER(0x03);
const uint8_t REG_TEMP_CRIT(0x04);
const uint8_t REG_TEMP_AMBIENT(0x05);
const uint8_t REG_ID_MAN(0x06);
const uint8_t REG_ID_DEV(0x07);
const uint8_t REG_RESOLUTION(0x08);

const uint16_t CFG_ALERTMODE(0x0001);
const uint16_t CFG_ALERTPOL(0x0002);
const uint16_t CFG_ALERTSEL(0x0004);
const uint16_t CFG_ALERTCTRL(0x0008);
const uint16_t CFG_ALERTSTAT(0x0010);
const uint16_t CFG_INTCLR(0x0020);
const uint16_t CFG_WINLOCKED(0x0040);
const uint16_t CFG_CRITLOCKED(0x0080);
const uint16_t CFG_SHUTDOWN(0x0100);

} // namespace

namespace Xilka {

unsigned long MCP9808::MaxI2CSpeed(400000UL);

bool MCP9808::begin(uint8_t i2cAddr)
{
	_i2cAddr = i2cAddr;

	uint16_t mId(0);
	uint16_t dId(0);

	_isAvailable = I2C::readUInt16(_i2cAddr, REG_ID_MAN, mId)
			&& I2C::readUInt16(_i2cAddr, REG_ID_DEV, dId)
			&& ( mId == 0x0054 )
			&& ( (dId >> 8) == 0x04 );

	if ( _isAvailable )
	{
		setResolutionSixteenth();
		powerDown();
	}

	return _isAvailable;
}

bool MCP9808::setResolution(uint8_t resolution)
{
	if ( I2C::writeUInt8(_i2cAddr, REG_RESOLUTION, resolution) )
	{
		_resolution = resolution;
		return true;
	}

	return false;
}

bool MCP9808::powerUp(void) const
{
	uint16_t reg(0);

	if ( I2C::readUInt16(_i2cAddr, REG_CFG, reg) )
	{
		reg &= ~CFG_SHUTDOWN;

		return I2C::writeUInt16(_i2cAddr, REG_CFG, reg);
	}

	return false;
}

bool MCP9808::powerDown(void) const
{
	uint16_t reg(0);

	if ( I2C::readUInt16(_i2cAddr, REG_CFG, reg) )
	{
		reg |= CFG_SHUTDOWN;

		return I2C::writeUInt16(_i2cAddr, REG_CFG, reg);
	}

	return false;
}

float MCP9808::getTempC(bool autoPower) const
{
	if ( autoPower )
	{
		powerUp();
		delay(getPowerDelay());
	}

	uint16_t t(0);

	if ( I2C::readUInt16(_i2cAddr, REG_TEMP_AMBIENT, t) )
	{
		float temp((float)(t & 0x0FFF) / 16.0);

		if ( t & 0x1000 )
			temp -= 256.0;

		return temp;
	}

	if ( autoPower )
		powerDown();

	return -999.0;
}

} // namespace Xilka

