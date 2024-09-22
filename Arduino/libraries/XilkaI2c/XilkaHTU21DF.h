#pragma once

/*
 * HTU21D-F Temp/Humidity Sensor
 *
 * Typical Humidity accuracy:
 *     2% RH from 5-95%
 *
 * Typical temp accuracy:
 *     0.3°C from 4°C to 60°C
 *     0.4°C from -40°C to 125°C
 *
 * Typical current draw:
 *     20uA in sleep mode (automatic)
 *     450uA in measurement mode (automatic)
 *
 * Conversion time (temp):
 *     44mS @ 14 bit resolution
 *     14mS @ 12 bit resolution
 *
 * I2C Address is fixed at 0x40
 *
 * https://www.adafruit.com/products/1899
 *
 */

#include "XilkaI2c.h"

namespace Xilka {

class HTU21DF
{
public:
	HTU21DF() : _isAvailable(false) {}
	bool begin(void); // I2C address is fixed at 0x40
	bool isAvailable(void) const { return _isAvailable; }

	/*
	 * These funcs return -999.0
	 * if the data is invalid
	 * (I2C or crc error).
	 */
	float getHumidity(void) const;
	float getTempC(void) const;

	float getTempF(void) const
	{
		const float f(getTempC());

		return ( f != -999.0 ) ? (f * 9.0 / 5.0 + 32.0) : -999.0;
	}

	static unsigned long MaxI2CSpeed;

private:

	bool _isAvailable;
};

} // namespace Xilka

