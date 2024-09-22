#include <Arduino.h>
#include <Wire.h>

#include "XilkaI2c.h"
#include "XilkaHDC1008.h"

namespace {

const uint8_t REG_TEMP(0x00);
const uint8_t REG_HUMID(0x01);
const uint8_t REG_CONFIG(0x02);
const uint8_t REG_SERIAL1(0xFB);
const uint8_t REG_SERIAL2(0xFC);
const uint8_t REG_SERIAL3(0xFD);
const uint8_t REG_MAN_ID(0xFE);
const uint8_t REG_DEV_ID(0xFF);

const uint16_t CONFIG_RST(1 << 15);
const uint16_t CONFIG_HEAT(1 << 13);
const uint16_t CONFIG_MODE(1 << 12);
const uint16_t CONFIG_BATT(1 << 11);
const uint16_t CONFIG_TRES_14(0);
const uint16_t CONFIG_TRES_11(1 << 10);
const uint16_t CONFIG_HRES_14(0);
const uint16_t CONFIG_HRES_11(1 << 8);
const uint16_t CONFIG_HRES_8(1 << 9);

} // namespace

namespace Xilka {

unsigned long HDC1008::MaxI2CSpeed(400000UL);

bool HDC1008::begin(uint8_t i2cAddr)
{
	_i2cAddr = i2cAddr;

	reset();

	uint16_t mid(0);
	uint16_t did(0);

	_isAvailable = I2C::readUInt16(_i2cAddr, REG_MAN_ID, mid)
			&& I2C::readUInt16(_i2cAddr, REG_DEV_ID, did)
			&& ( mid == 0x5449 )
			&& ( did == 0x1000 );

	return _isAvailable;
}

bool HDC1008::reset(void) const
{
	// reset, and select 14 bit temp & humidity
	const uint16_t config(CONFIG_RST
					| CONFIG_MODE
					| CONFIG_TRES_14
					| CONFIG_HRES_14);

	bool rc(I2C::writeUInt16(_i2cAddr, REG_CONFIG, config));

	delay(15);

	return rc;
}

float HDC1008::getTempC(void)
{
	update();

	return _raw.temp
			? (((float)_raw.temp) / 65536.0 * 165.0 - 40.0)
			: -999.0;
}

float HDC1008::getHumidity(void)
{
	update();

	return _raw.hum
			? (((float)_raw.hum) / 65536.0 * 100)
			: -999.0;
}

void HDC1008::update(void)
{
	const unsigned long ms(millis());

	// wait at least 2 sec between updates
	if ( _lastUpdate + 2000 < ms )
	{
		RawData raw;

		if ( I2C::read(_i2cAddr, REG_TEMP, &raw, sizeof(raw), true, 20) )
		{
			_raw = raw;
			_lastUpdate = ms;
		}
	}
}

} // namespace Xilka

