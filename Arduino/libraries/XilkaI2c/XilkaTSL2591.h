#pragma once

#include "XilkaI2c.h"

/*
 * Typical 2.3uA in powerDown mode 275uA in powerUp/conversion
 *
 */

namespace Xilka {

class TSL2591
{
public:
	TSL2591(void)
		: _isAvailable(false)
		, _gain(0x10)
		, _time(0x00)
		, _integrationMs(0)
		, _normalizeFactor(0.0)
	{
	}

	bool begin(void); // address is fixed at 0x29

	bool isAvailable(void) const { return _isAvailable; }
	bool getId(uint8_t &id) const; // part# should return 0x50

	bool setGainLow(void) { _gain = 0x00; return setTiming(); } // 1x
	bool setGainMedium(void) { _gain = 0x10; return setTiming(); } // 25x
	bool setGainHigh(void) { _gain = 0x20; return setTiming(); } // 428x
	bool setGainMax(void) { _gain = 0x30; return setTiming(); } // 9876x
	bool setIntegrationMs100(void) { _time = 0x00; return setTiming(); }
	bool setIntegrationMs200(void) { _time = 0x01; return setTiming(); }
	bool setIntegrationMs300(void) { _time = 0x02; return setTiming(); }
	bool setIntegrationMs400(void) { _time = 0x03; return setTiming(); }
	bool setIntegrationMs500(void) { _time = 0x04; return setTiming(); }
	bool setIntegrationMs600(void) { _time = 0x05; return setTiming(); }

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
		return (unsigned long)((_time + 1) * 120);
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

	bool _isAvailable;
	uint8_t _gain;
	uint8_t _time;

	unsigned int _integrationMs;
	double _normalizeFactor;
};

} // namespace Xilka

