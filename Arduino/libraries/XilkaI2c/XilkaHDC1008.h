#pragma once

/*
 * HDC1008 Temp/Humidity Sensor
 *
 * Typical Humidity accuracy:
 *     4% from 10-80%
 * Typical Humidity repeatability:
 *     0.1%
 *
 * Typical temp accuracy:
 *     0.2°C from -20°C to 85°C
 * Typical temp repeatability:
 *     0.1°C
 * Typical temp precision:
 *     0.0125°C when in 14-bit mode (default)
 *
 * Typical current draw:
 *     0.2uA in sleep mode (automatic)
 *     1.2uA in 11 bit measurement mode (automatic)
 *
 * Automatically goes into sleep mode
 * when not processing a read request,
 * which minimizes self-heating.
 *
 * Conversion time:
 *     6.5mS @ 14 bit resolution
 *     3.85mS @ 11 bit resolution
 *     2.5mS @ 8 bit resolution
 *
 * https://www.adafruit.com/products/2635
 */

#include "XilkaI2c.h"

namespace Xilka {

class HDC1008
{
public:

	HDC1008()
		: _i2cAddr(0)
		, _isAvailable(false)
		, _lastUpdate(0)
	{
		_raw.temp = _raw.hum = 0;
	}

	/*
	 * range 0x40-0x43
	 * set by pulling up A0/A1 to +V
	 */
	bool begin(uint8_t a = 0x40);

	/*
	 * These funcs return -999.0
	 * if the data is invalid (I2C error).
	 */
	float getHumidity(void);
	float getTempC(void);

	float getTempF(void)
	{
		const float f(getTempC());

		return ( f != -999.0 ) ? (f * 9.0 / 5.0 + 32.0) : -999.0;
	}

	bool isAvailable(void) const { return _isAvailable; }
	bool reset(void) const;

	static unsigned long MaxI2CSpeed;

private:
	void update(void);

	uint8_t _i2cAddr;
	bool _isAvailable;
	unsigned long _lastUpdate;

	struct RawData
	{
		uint16_t temp;
		uint16_t hum;
	} _raw;
};

} // namespace Xilka

