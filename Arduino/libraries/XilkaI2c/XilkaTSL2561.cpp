#include <Arduino.h>
#include <Wire.h>

#include "XilkaI2c.h"
#include "XilkaTSL2561.h"

namespace {

// TSL2561 registers

const uint8_t REG_CMD(0x80);
const uint8_t REG_CMD_CLEAR(0xC0);

// sub items withing REG_CMD
const uint8_t REG_CONTROL(0x00);
const uint8_t REG_TIMING(0x01);
const uint8_t REG_THRESH_LOW(0x02);
const uint8_t REG_THRESH_HIGH(0x04);
const uint8_t REG_INTCTL(0x06);
const uint8_t REG_ID(0x0A);
const uint8_t REG_CHAN0_LOW(0x0C);
//const uint8_t REG_CHAN0_HIGH(0x0D);
const uint8_t REG_CHAN1_LOW(0x0E);
//const uint8_t REG_CHAN1_HIGH(0x0F);

} // namespace

namespace Xilka {

unsigned long TSL2561::MaxI2CSpeed(400000UL);

bool TSL2561::begin(uint8_t i2cAddr)
{
	_i2cAddr = i2cAddr;

	uint8_t id(0);

	// (id >> 4) : 0b0100=TSL2560, 0b0101=TSL2561
	_isAvailable = ( getId(id)
			&& ( ((id >> 4) == 0b0100)
				|| (((id >> 4) == 0b0101) ) ) );

	return _isAvailable && setTiming();
}

bool TSL2561::getId(uint8_t &id) const
{
	return I2C::readUInt8(_i2cAddr, REG_CMD | REG_ID, id);
}

bool TSL2561::powerUp(void) const
{
	return I2C::writeUInt8(_i2cAddr, REG_CMD | REG_CONTROL, 0x03);
}

bool TSL2561::powerDown(void) const
{
	return I2C::writeUInt8(_i2cAddr, REG_CMD | REG_CONTROL, 0x00);
}

bool TSL2561::setTiming(void)
{
/*
 * _highGain == false, device is set to low gain (1X)
 * _highGain == true, device is set to high gain (16X)
 * _time == 0, integration will be 13.7ms
 * _time == 1, integration will be 101ms
 * _time == 2, integration will be 402ms
 */

	switch ( _time )
	{
	case 0: _integrationMs = 14; break;
	case 1: _integrationMs = 101; break;
	case 2: _integrationMs = 402; break;
	default: _integrationMs = 0; break;
	}

	// Normalize for integration time
	_normalizeFactor = 402.0 / _integrationMs;

	// Normalize for gain
	if ( ! _highGain )
		_normalizeFactor *= 16.0;

	uint8_t timing(0);

	// Get timing byte
	if ( I2C::readUInt8(_i2cAddr, REG_CMD | REG_TIMING, timing) )
	{
		// Set gain (0 or 1)
		if ( _highGain )
			timing |= 0x10;
		else
			timing &= ~0x10;

		// Set integration time (0 to 3)
		timing &= ~0x03;
		timing |= (_time & 0x03);

		return I2C::writeUInt8(_i2cAddr, REG_CMD | REG_TIMING, timing);
	}

	return false;
}

bool TSL2561::startManual(void) const
{
	uint8_t timing(0);

	if ( I2C::readUInt8(_i2cAddr, REG_CMD | REG_TIMING, timing) )
	{
		// Set integration time to 3 (manual integration)
		timing |= 0x03;

		if ( I2C::writeUInt8(_i2cAddr, REG_CMD | REG_TIMING, timing) )
		{
			timing |= 0x08; // Begin manual integration

			return I2C::writeUInt8(_i2cAddr, REG_CMD | REG_TIMING, timing);
		}
	}

	return false;
}

bool TSL2561::stopManual(void) const
{
	uint8_t timing(0);

	if ( I2C::readUInt8(_i2cAddr, REG_CMD | REG_TIMING, timing) )
	{
		timing &= ~0x08; // Stop manual integration

		return I2C::writeUInt8(_i2cAddr, REG_CMD | REG_TIMING, timing);
	}

	return false;
}

bool TSL2561::getLux(double &lux, bool autoPower) const
{
/*
 * returns true on success
 *
 * returns false AND lux = 0.0
 * if either sensor was saturated (0XFFFF)
 */

	uint16_t raw0(0xFFFF);
	uint16_t raw1(0xFFFF);

	// ensure we've retrieved data and no saturated sensors
	if ( getData(raw0, raw1, autoPower)
		&& (raw0 != 0xFFFF)
		&& (raw1 != 0xFFFF) )
	{
		double ch0(raw0);
		double ch1(raw1);
		const double ratio(ch1 / ch0);

		ch0 *= _normalizeFactor;
		ch1 *= _normalizeFactor;

		// formula pulled from datasheet pg 24-25

/*
 * everything but the CS package first
 */
#if ! defined(USE_CS_PACKAGE)
		if ( ratio >= 0.0 && ratio <= 0.5 )
			lux = (0.0304 *ch0) - ((0.062 * pow(ratio, 1.4)) * ch0);
		else if ( ratio <= 0.61 )
			lux = (0.0224 * ch0) - (0.031 * ch1);
		else if ( ratio <= 0.80 )
			lux = (0.0128 * ch0) - (0.0153 * ch1);
		else if ( ratio <= 1.30 )
			lux = (0.00146 * ch0) - (0.00112 * ch1);
		else // (ratio > 1.30)
			lux = 0.0;
#else // USE_CS_PACKAGE
		if ( ratio >= 0.0 && ratio <= 0.52 )
			lux = (0.0315 *ch0) - ((0.0593 * pow(ratio, 1.4)) * ch0);
		else if ( ratio <= 0.65 )
			lux = (0.0229 * ch0) - (0.0291 * ch1);
		else if ( ratio <= 0.80 )
			lux = (0.00157 * ch0) - (0.00180 * ch1);
		else if ( ratio <= 1.30 )
			lux = (0.00338 * ch0) - (0.00260 * ch1);
		else // (ratio > 1.30)
			lux = 0.0;
#endif // USE_CS_PACKAGE

		return true;
	}

	lux = 0.0;
	return false;
}

bool TSL2561::getData(uint16_t &ch0, uint16_t &ch1, bool autoPower) const
{
	if ( autoPower )
	{
		powerUp();
		delay(getPowerDelay());
	}

	/*
	 * combining this into one 4byte read does not work
	 */
	bool status(I2C::readUInt16LE(_i2cAddr, REG_CMD | REG_CHAN0_LOW, ch0)
					&& I2C::readUInt16LE(_i2cAddr, REG_CMD | REG_CHAN1_LOW, ch1));

	if ( autoPower )
		powerDown();

	return status;
}

bool TSL2561::setInterruptControl(uint8_t control, uint8_t persist) const
{
/*
 * Sets up interrupt operations
 * If control = 0, interrupt output disabled
 * If control = 1, use level interrupt, see setInterruptThreshold()
 * If persist = 0, every integration cycle generates an interrupt
 * If persist = 1, any value outside of threshold generates an interrupt
 * If persist = 2 to 15, value must be outside of threshold for 2 to 15 integration cycles
 */

	// Place control and persist bits into proper location
	// in interrupt control register
	return I2C::writeUInt8(_i2cAddr
					, REG_CMD | REG_INTCTL
					, ((control | 0B00000011) << 4)
						& (persist | 0B00001111));
}

bool TSL2561::setInterruptThreshold(uint16_t low, uint16_t high) const
{
/*
 * Set interrupt thresholds (channel 0 only)
 */

	return ( I2C::writeUInt16LE(_i2cAddr, REG_CMD | REG_THRESH_LOW, low)
			&& I2C::writeUInt16LE(_i2cAddr, REG_CMD | REG_THRESH_HIGH, high) );
}

bool TSL2561::clearInterrupt(void) const
{
/*
 * Clears an active interrupt
 */

	// Set up command byte for interrupt clear
	Wire.beginTransmission(_i2cAddr);
	Wire.write(REG_CMD_CLEAR);

	return ! Wire.endTransmission();
}

} // namespace Xilka

