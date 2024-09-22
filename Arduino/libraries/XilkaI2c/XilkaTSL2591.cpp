#include <Arduino.h>
#include <Wire.h>

#include "XilkaI2c.h"
#include "XilkaTSL2591.h"

namespace {

// TSL2591 registers

const uint8_t I2C_ADDR(0x29);

const uint8_t REG_CMD(0x80);
const uint8_t REG_CMD_NORMAL(REG_CMD|0x20);

#define TSL_VISIBLE           (2)       // channel 0 - channel 1
#define TSL_INFRARED          (1)       // channel 1
#define TSL_FULLSPECTRUM      (0)       // channel 0

#define TSL_READBIT           (0x01)

const uint8_t CMD_NORMAL(0xA0);    // bits 7 and 5 for 'command normal'
#define TSL_CLEAR_BIT         (0x40)    // Clears any pending interrupt (write 1 to clear)
#define TSL_WORD_BIT          (0x20)    // 1 = read/write word (rather than byte)
#define TSL_BLOCK_BIT         (0x10)    // 1 = using block read/write

#define TSL_CONTROL_RESET     (0x80)

const uint8_t REG_ENABLE(0x00);
const uint8_t ENABLE_POWERON(0x01);
const uint8_t ENABLE_POWEROFF(0x00);
const uint8_t ENABLE_AEN(0x02);
const uint8_t ENABLE_AIEN(0x10);

const uint8_t REG_CONTROL(0x01);
const uint8_t REG_THRESHHOLDL_LOW(0x02);
const uint8_t REG_THRESHHOLDL_HIGH(0x03);
const uint8_t REG_THRESHHOLDH_LOW(0x04);
const uint8_t REG_THRESHHOLDH_HIGH(0x05);
const uint8_t REGI_INTERRUPT(0x06);
const uint8_t REG_CRC(0x08);
const uint8_t REG_ID(0x0A);
const uint8_t REG_CHAN0_LOW(0x14);
const uint8_t REG_CHAN0_HIGH(0x15);
const uint8_t REG_CHAN1_LOW(0x16);
const uint8_t REG_CHAN1_HIGH(0x17);

double getGain(uint8_t gain)
{
	switch(gain)
	{
	default: case 0x00: return 1.0;
	case 0x10: return 25.0;
	case 0x20: return 428.0;
	case 0x30: return 9876.0;
	}
}

} // namespace

namespace Xilka {

unsigned long TSL2591::MaxI2CSpeed(400000UL);

bool TSL2591::begin(void)
{
	uint8_t id(0);

	// TSL2591 should return 0x50
	_isAvailable = ( getId(id) && id == 0x50 );

	return _isAvailable && setTiming();
}

bool TSL2591::getId(uint8_t &id) const
{
	return I2C::readUInt8(I2C_ADDR, CMD_NORMAL | 0x12, id);
}

bool TSL2591::powerUp(void) const
{
	return I2C::writeUInt8(I2C_ADDR
					, CMD_NORMAL | REG_ENABLE
					, ENABLE_POWERON
						| ENABLE_AEN
						| ENABLE_AIEN);
}

bool TSL2591::powerDown(void) const
{
	return I2C::writeUInt8(I2C_ADDR
					, CMD_NORMAL | REG_ENABLE
					, ENABLE_POWEROFF);
}

bool TSL2591::setTiming(void)
{
	return I2C::writeUInt8(I2C_ADDR
					, CMD_NORMAL | REG_CONTROL
					, _time|_gain);
}

bool TSL2591::getLux(double &lux, bool autoPower) const
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

		/*
		 * This algorithm is based on preliminary coefficients
		 * provided by AMS and may need to be updated in the future
		 *
		 */
		static const double LUX_DF(408.0F);
		static const double LUX_COEFB(1.64F);  // CH0 coefficient
		static const double LUX_COEFC(0.59F);  // CH1 coefficient A
		static const double LUX_COEFD(0.86F);  // CH2 coefficient B

		double atime((double)((_time + 1) * 100));
		double again(getGain(_gain));
		double cpl((atime * again) / LUX_DF);

		double lux1(( ch0 - (LUX_COEFB * ch1) ) / cpl);
		double lux2(( ( LUX_COEFC * ch0 ) - ( LUX_COEFD * ch1 ) ) / cpl);

		// The highest value is the approximate lux equivalent
		lux = (lux1 > lux2) ? lux1 : lux2;

		return true;
	}

	lux = 0.0;
	return false;
}

bool TSL2591::getData(uint16_t &ch0, uint16_t &ch1, bool autoPower) const
{
	if ( autoPower )
	{
		powerUp();
		delay(getPowerDelay());
	}

#if 1
	uint8_t raw[4] = { 0 };
	bool status(I2C::read(I2C_ADDR, REG_CMD_NORMAL | REG_CHAN0_LOW, raw, sizeof(raw)));

	if ( status )
	{
		ch0 = ((uint16_t)raw[1] << 8) | (uint16_t)raw[0];
		ch1 = ((uint16_t)raw[3] << 8) | (uint16_t)raw[2];
	}
#else
	bool status(I2C::readUInt16LE(I2C_ADDR, REG_CMD_NORMAL | REG_CHAN0_LOW, ch0)
					&& I2C::readUInt16LE(I2C_ADDR, REG_CMD_NORMAL | REG_CHAN1_LOW, ch1));
#endif

	if ( autoPower )
		powerDown();

	return status;
}

#if 0
// TSL2591 is probably different than the TSL2561, so this needs to be reworked

bool TSL2591::setInterruptControl(uint8_t control, uint8_t persist) const
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

bool TSL2591::setInterruptThreshold(uint16_t low, uint16_t high) const
{
/*
 * Set interrupt thresholds (channel 0 only)
 */

	return ( I2C::writeUInt16LE(_i2cAddr, REG_CMD | REG_THRESH_L, low)
			&& I2C::writeUInt16LE(_i2cAddr, REG_CMD | REG_THRESH_H, high) );
}

bool TSL2591::clearInterrupt(void) const
{
/*
 * Clears an active interrupt
 */

	// Set up command byte for interrupt clear
	Wire.beginTransmission(_i2cAddr);
	Wire.write(REG_CMD_CLEAR);

	return ! Wire.endTransmission();
}
#endif

} // namespace Xilka

