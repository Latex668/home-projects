#pragma once

#include "XilkaI2c.h"

namespace Xilka {

class INA219
{
public:
	INA219()
		: _i2cAddr(0)
		, _calValue(0)
		, _config(0)
		, _currentDividerMilliAmp(0.0)
		// , _powerDividerMilliWatt(0.0)
	{
	}

	/*
	 * default address = 0x40: A0->GND, A1->GND
	 * may range from 0x40-0x43 by tieing A0/A1 to +V
	 */
	void begin(uint8_t i2cAddr = 0x40);

	/*
	 * sampleCount may be 2,4,8,16,32,64 or 128
	 * higher values are more accurate but take
	 * corresponding longer times to read
	 *
	 * returns true for success
	 */
	bool setRange32V2A(int sampleCount = 4);
	bool setRange32V1A(int sampleCount = 4);
	bool setRange16V400mA(int sampleCount = 4);

	/*
	 * All float functions return -999.0 on error
	 */

	float getMilliAmps(void) const
	{
		int16_t i(0);

		return getRawCurrent(i)
				? ((float)i / _currentDividerMilliAmp)
				: -999.0;
	}

	float getAmps(void) const
	{
		const float ma(getMilliAmps());

		return ( ma != -999.0 )
				? (ma / 1000.0)
				: -999.0;
	}

	float getMilliVolts(void) const
	{
		const float bv(getBusVolts());

		if ( bv != -999.0 )
		{
			const float smv(getShuntMilliVolts());

			if ( smv != -999.0 )
				return (bv * 1000.0) + smv;
		}

		return -999.0;
	}

	float getVolts(void) const
	{
		const float mv(getMilliVolts());

		return ( mv != -999.0 )
				? (mv / 1000.0)
				: -999.0;
	}

	static unsigned long MaxI2CSpeed;

private:
	float getBusVolts(void) const
	{
		int16_t v(0);

		return getRawBusVoltage(v)
				?  ((float)v * 0.001)
				: -999.0;
	}

	float getShuntMilliVolts(void) const
	{
		int16_t v(0);

		return getRawShuntVoltage(v)
				? ((float)v * 0.01)
				: -999.0;
	}

	bool getRawBusVoltage(int16_t &result) const;
	bool getRawShuntVoltage(int16_t &result) const;
	bool getRawCurrent(int16_t &result) const;
	void delayRead(void) const;

	uint8_t _i2cAddr;
	uint16_t _calValue;
	uint16_t _config;

	/*
	 * The following multipliers are used to convert
	 * raw current and power values to mA and mW,
	 * taking into account the current config settings
	 */
	float _currentDividerMilliAmp;
	// float _powerDividerMilliWatt;
};

} // namespace Xilka

