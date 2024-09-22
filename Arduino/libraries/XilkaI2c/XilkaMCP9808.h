#pragma once

/*
 * MCP9808 High Accuracy Temp Sensor
 *
 * Typical temp accuracy:
 *     0.25°C from -40°C to 125°C
 * Typical temp precision:
 *     +0.0625°C
 *
 * Typical current draw:
 *     0.1uA in powerDown mode
 *     200uA in measurement mode
 *
 * Typical conversion speed 250mS in high precision mode
 * (in effect if you're toggling between powerUp/powerDown
 * it take a quarter of a second after powerUp
 * to get a measurement)
 *
 * https://www.adafruit.com/products/1782
 *
 * Address may be set from 0x18 t0 0x1F
 * by connecting combinations of A0-A2 to V+
 *
 */

#include "XilkaI2c.h"

namespace Xilka {

class MCP9808
{
public:
	MCP9808() : _i2cAddr(0), _isAvailable(false), _resolution(3) {}
	bool begin(uint8_t i2cAddr = 0x18); // 0x18-0x1F

	bool isAvailable(void) const { return _isAvailable; }

	/*
	 * These funcs return -999.0
	 * if the data is invalid (I2C error).
	 */
	float getTempC(bool autoPower = true) const;

	float getTempF(bool autoPower = true) const
	{
		const float f(getTempC(autoPower));

		return ( f != -999.0 ) ? (f * 9.0 / 5.0 + 32.0) : -999.0;
	}

	/*
	 * Set resolution in parts of a degree
	 */
	bool setResolutionHalf(void) { return setResolution(0); }
	bool setResolutionQuarter(void) { return setResolution(1); }
	bool setResolutionEigth(void) { return setResolution(2); }
	bool setResolutionSixteenth(void) { return setResolution(3); } // default

	bool powerUp(void) const;	// ~200uA
	bool powerDown(void) const; // ~0.1uA

	unsigned long getPowerDelay(void) const
	{
		switch ( _resolution )
		{
			case 0: return 40; // typical 30, add 10 for safety
			case 1: return 75; // typical 65, add 10 for safety
			case 2: return 140; // typical 130, add 10 for safety
			case 3: return 260; // default - typical 250, add 10 for safety
			default: return 260;
		}
	}

	static unsigned long MaxI2CSpeed;

private:
	bool setResolution(uint8_t resolution);

	uint8_t _i2cAddr;
	bool _isAvailable;
	uint8_t _resolution;
};

} // namespace Xilka

