#include <Arduino.h>
#include <Wire.h>

#include "XilkaI2c.h"
#include "XilkaFT6206.h"

namespace {

//#define G_FT5201ID 0xA8

//#define NUM_X 0x33
//#define NUM_Y 0x34

const uint8_t I2C_ADDR(0x38);
const uint8_t REG_MODE(0x00);
const uint8_t REG_CALIBRATE(0x02);
const uint8_t REG_NUMTOUCHES(0x02);
const uint8_t REG_WORKMODE(0x00);
const uint8_t REG_FACTORYMODE(0x40);
const uint8_t REG_THRESHHOLD(0x80);
const uint8_t REG_POINTRATE(0x88);
const uint8_t REG_FIRMVERS(0xA6);
const uint8_t REG_CHIPID(0xA3);
const uint8_t REG_VENDID(0xA8);

} // namespace

// calibrated for Adafruit 2.8" ctp screen
#define DEFAULT_THRESSHOLD 128

namespace Xilka {

/*
 * 400KHz - from datasheet page 6
 */
unsigned long FT6206::MaxI2CSpeed(400000UL);

bool FT6206::begin(uint8_t threshhold)
{
	I2C::writeUInt8(I2C_ADDR, REG_THRESHHOLD, threshhold);

	uint8_t vId(0);
	uint8_t cId(0);

	return I2C::readUInt8(I2C_ADDR, REG_VENDID, vId)
			&& I2C::readUInt8(I2C_ADDR, REG_CHIPID, cId)
			&& (vId == 0x11)
			&& (cId == 0x06);
}

bool FT6206::touched(void)
{
	uint8_t n(0);

	return I2C::readUInt8(I2C_ADDR, REG_NUMTOUCHES, n)
			&& (( n == 1 ) || ( n == 2 ));
}

void FT6206::readData(uint16_t *x, uint16_t *y)
{
	uint8_t i2cdat[32];

	/*
	 * Apparently we have to read 32 bytes
	 */
	if ( I2C::read(I2C_ADDR, 0, i2cdat, sizeof(i2cdat)) )
	{
		touches = i2cdat[0x02];

		if ( touches > 2 )
		{
			touches = 0;
			*x = *y = 0;
		}

		if ( touches == 0 )
		{
			*x = *y = 0;
			return;
		}

		for ( uint8_t i = 0; i < 2; ++i )
		{
			const uint8_t i6(i*6);

			touchX[i] = i2cdat[0x03 + i6] & 0x0F;
			touchX[i] <<= 8;
			touchX[i] |= i2cdat[0x04 + i6];
			touchY[i] = i2cdat[0x05 + i6] & 0x0F;
			touchY[i] <<= 8;
			touchY[i] |= i2cdat[0x06 + i6];
			touchID[i] = i2cdat[0x05 + i6] >> 4;
		}

		*x = touchX[0];
		*y = touchY[0];
	}
}

TsPoint FT6206::getPoint(void)
{
	uint16_t x(0);
	uint16_t y(0);

	readData(&x, &y);
	return TsPoint(x, y, 1);
}

} // namespace Xilka

