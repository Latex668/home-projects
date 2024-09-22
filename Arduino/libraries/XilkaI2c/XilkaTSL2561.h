#pragma once

#include "XilkaI2c.h"

/*
 * Typical 3.2uA in powerDown mode 0.24mA in powerUp/conversion
 *
 * Typical 100mS (up to 400mS) conversion time
 * (in effect if you're toggling between powerUp/powerDown
 * it takes this long to get a measurement)
 *
 */

namespace Xilka {

class TSL2561
{
public:
	TSL2561(void)
		: _i2cAddr(0)
		, _isAvailable(false)
		, _highGain(false)
		, _time(2)
		, _integrationMs(0)
		, _normalizeFactor(0.0)
	{
	}

	/*
	 * Sparkfun:
	 *     0x29 with '0' shorted on board
	 *     0x39 default address
	 *     0x49 with '1' shorted on board
	 *
	 * Adafruit:
	 *     0x29 with address pulled to 3.3v
	 *     0x39 default address
	 *     0x49 with address pulled to GND
	 */
	bool begin(uint8_t i2cAddr = 0x39);
	bool isAvailable(void) const { return _isAvailable; }
	bool getId(uint8_t &id) const; // part and revision code

	bool setGainLow(void) { _highGain = false; return setTiming(); } // 1x
	bool setGainHigh(void) { _highGain = true; return setTiming(); } // 16x
	bool setIntegrationMs14(void) { _time = 0; return setTiming(); }
	bool setIntegrationMs101(void) { _time = 1; return setTiming(); }
	bool setIntegrationMs402(void) { _time = 2; return setTiming(); }

	bool powerUp(void) const;
	bool powerDown(void) const;

	/*
	 * true if we have valid data
	 * false and lux = 0.0
	 *     if unable to retrieve data
	 *         or either sensor saturated
	 *
	 * autoPower indicates we want getLux
	 * to to powerUp/delay(getPowerDelay()/powerDown
	 * automatically.
	 */
	bool getLux(double &lux, bool autoPower = true) const;

	/*
	 * used when calling getLux(lux, autoPower=false)
	 */
	unsigned long getPowerDelay(void) const
	{
		switch( _time )
		{
			case 0: return 15;
			case 1: return 120;
			case 2: return 450;
			default: return 1000;
		}
	}

	/*
	 * Sets up interrupt operations
	 * If control = 0, interrupt output disabled
	 * If control = 1, use level interrupt, see setInterruptThreshold()
	 * If persist = 0, every integration cycle generates an interrupt
	 * If persist = 1, any value outside of threshold generates an interrupt
	 * If persist = 2 to 15, value must be outside of threshold
	 * for 2 to 15 integration cycles
	 */
	bool setInterruptControl(uint8_t control, uint8_t persist) const;

	/*
	 * Set interrupt thresholds (channel 0 only)
	 * low, high: 16-bit threshold values
	 */
	bool setInterruptThreshold(uint16_t low, uint16_t high) const;

	/*
	 * Clears an active interrupt
	 */
	bool clearInterrupt(void) const;

	static unsigned long MaxI2CSpeed;

private:
	bool setTiming(void);
	bool getData(uint16_t &ch0, uint16_t &ch1, bool autoPower) const;
	bool startManual(void) const;
	bool stopManual(void) const;

	uint8_t _i2cAddr;
	bool _isAvailable;
	bool _highGain;
	int _time;
	unsigned int _integrationMs;
	double _normalizeFactor;
};

} // namespace Xilka

