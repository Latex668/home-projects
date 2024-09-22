#pragma once

#include "XilkaI2c.h"

/*
 * Typical 2uA in powerdown mode 2mA during active conversion
 *
 */

namespace Xilka {

class MPL3115A2
{
public:
	MPL3115A2() : _i2cAddr(0), _isAvailable(false), _oversampleRate(1) {}
	bool begin(uint8_t i2cAddr = 0x60);

	bool isAvailable(void) const { return _isAvailable; }

	/*
	 * These funcs return -999.0
	 * if the data is invalid (I2C error).
	 */

	float getAltitudeMeters(void) const;

	float getAltitudeFeet(void) const
	{
		const float f(getAltitudeMeters());

		return ( f != -999.0 ) ? f * 3.28084 : -999.0;
	}

	float getPressurePa(void) const;

    float getPressureHg(void) const
	{
		const float f(getPressurePa());

		return ( f != -999.0 ) ? f / 3377.0 : -999.0;
	}

	float getTempC(void) const;

	float getTempF(void) const
	{
		const float f(getTempC());

		return ( f != -999.0 ) ? (f * 9.0 / 5.0 + 32.0) : -999.0;
	}

	bool powerDown(void) const; // Required when changing CTRL1 register.
	bool powerUp(void) const; // Start taking measurements!

	/*
	 * returns delay time after powerUp()
	 * before getTemp*, getPressure* or getAltitude*
	 * should be called
	 */
	unsigned long getPowerDelay(void) const
	{
		switch ( _oversampleRate )
		{
		case 1: return 60UL; // from spec sheet pg 6
		case 2: return 80UL; // guessed from spec sheet pg 31
		case 4: return 110UL; // guessed from spec sheet pg 31
		case 16: return 140UL; // guessed from spec sheet pg 31
		case 32: return 280UL; // guessed from spec sheet pg 31
		case 64: return 500UL; // guessed from spec sheet pg 31
		case 128: return 1000UL; // from spec sheet pg 6
		default: return 1000UL; // safety net
		}
	}

	/*
	 * Oversample=1 is power up default
	 */
	bool setOversample1X(void) { _oversampleRate = 1; return setOversample(0); }
	bool setOversample2X(void) { _oversampleRate = 2; return setOversample(1); }
	bool setOversample4X(void) { _oversampleRate = 4; return setOversample(2); }
	bool setOversample8X(void) { _oversampleRate = 8; return setOversample(3); }
	bool setOversample16X(void) { _oversampleRate = 16; return setOversample(4); }
	bool setOversample32X(void) { _oversampleRate = 32; return setOversample(5); }
	bool setOversample64X(void) { _oversampleRate = 64; return setOversample(6); }
	bool setOversample128X(void) { _oversampleRate = 128; return setOversample(7); }

	bool setModeBarometer(void) const;
	bool setModeAltimeter(void) const;
	bool enableEventFlags(void) const; // Sets the fundamental event flags. Required during setup.

	static unsigned long MaxI2CSpeed;

private:
	bool setOversample(uint8_t) const;
	bool waitForPressureData(void) const;
	bool waitForTemperatureData(void) const;

	bool isTemperatureDataAvailable(void) const
	{
		uint8_t result(0);

		I2C::readUInt8(_i2cAddr, 0x00, result);

		return result & (1<<1); // STATUS/TDR
	}

	bool isPressureDataAvailable(void) const
	{
		uint8_t result(0);

		I2C::readUInt8(_i2cAddr, 0x00, result);

		return result & (1<<2); // STATUS/PDR
	}

	bool requestImmediateRead(void) const;

	uint8_t _i2cAddr;
	uint8_t _isAvailable;
	uint8_t _oversampleRate;
};

} // namespace Xilka

