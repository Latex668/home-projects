/*
 MPL3115A2 Barometric Pressure Sensor Library
 By: Nathan Seidle
 SparkFun Electronics
 Date: September 22nd, 2013
 License: This code is public domain but you buy me a beer
     if you use this and we meet someday (Beerware license).

 This library allows an Arduino to read from the MPL3115A2
 low-cost high-precision pressure sensor.

 If you have feature suggestions or need support please use
 the github support page: https://github.com/sparkfun/MPL3115A2_Breakout

 Hardware Setup: The MPL3115A2 lives on the I2C bus.
 Attach the SDA pin to A4, SCL to A5. Use inline 10k resistors
 if you have a 5V board. If you are using the SparkFun breakout board
 you *do not* need 4.7k pull-up resistors
 on the bus (they are built-in).

 */

#include <Arduino.h>

#if defined(__AVR_ATtiny85__)
#include <TinyWireM.h>
#else
#include <Wire.h>
#endif

#include "XilkaI2c.h"
#include "XilkaMPL3115A2.h"

#define STATUS     0x00
#define OUT_P_MSB  0x01
#define OUT_P_CSB  0x02
#define OUT_P_LSB  0x03
#define OUT_T_MSB  0x04
#define OUT_T_LSB  0x05
#define DR_STATUS  0x06
#define OUT_P_DELTA_MSB  0x07
#define OUT_P_DELTA_CSB  0x08
#define OUT_P_DELTA_LSB  0x09
#define OUT_T_DELTA_MSB  0x0A
#define OUT_T_DELTA_LSB  0x0B
#define WHO_AM_I   0x0C
#define F_STATUS   0x0D
#define F_DATA     0x0E
#define F_SETUP    0x0F
#define TIME_DLY   0x10
#define SYSMOD     0x11
#define INT_SOURCE 0x12
#define PT_DATA_CFG 0x13
#define BAR_IN_MSB 0x14
#define BAR_IN_LSB 0x15
#define P_TGT_MSB  0x16
#define P_TGT_LSB  0x17
#define T_TGT      0x18
#define P_WND_MSB  0x19
#define P_WND_LSB  0x1A
#define T_WND      0x1B
#define P_MIN_MSB  0x1C
#define P_MIN_CSB  0x1D
#define P_MIN_LSB  0x1E
#define T_MIN_MSB  0x1F
#define T_MIN_LSB  0x20
#define P_MAX_MSB  0x21
#define P_MAX_CSB  0x22
#define P_MAX_LSB  0x23
#define T_MAX_MSB  0x24
#define T_MAX_LSB  0x25
#define CTRL_REG1  0x26
#define CTRL_REG2  0x27
#define CTRL_REG3  0x28
#define CTRL_REG4  0x29
#define CTRL_REG5  0x2A
#define OFF_P      0x2B
#define OFF_T      0x2C
#define OFF_H      0x2D

namespace Xilka {

unsigned long MPL3115A2::MaxI2CSpeed(400000UL);

bool MPL3115A2::begin(uint8_t i2cAddr)
{
	_i2cAddr = i2cAddr;

	uint8_t id;

	_isAvailable = I2C::readUInt8(_i2cAddr, WHO_AM_I, id, false) && (id == 0xC4);

	return _isAvailable;
}

bool MPL3115A2::setModeBarometer(void) const
{
	uint8_t ctl(0);

	if ( I2C::readUInt8(_i2cAddr, CTRL_REG1, ctl, false) )
	{
		ctl &= ~(1<<7); // Clear ALT bit
		return I2C::writeUInt8(_i2cAddr, CTRL_REG1, ctl, false);
	}

	return false;
}

bool MPL3115A2::setModeAltimeter(void) const
{
	uint8_t ctl(0);

	if ( I2C::readUInt8(_i2cAddr, CTRL_REG1, ctl, false) )
	{
		ctl |= (1<<7); // Set ALT bit
		return I2C::writeUInt8(_i2cAddr, CTRL_REG1, ctl, false);
	}

	return false;
}

bool MPL3115A2::powerDown(void) const
{
/*
 * Puts the sensor in standby mode
 * This is needed so that we can modify
 * the major control registers
 */
	uint8_t ctl(0);

	if ( I2C::readUInt8(_i2cAddr, CTRL_REG1, ctl, false) )
	{
		ctl &= ~(1<<0); // Clear SBYB bit for Standby mode
		return I2C::writeUInt8(_i2cAddr, CTRL_REG1, ctl, false);
	}

	return false;
}

bool MPL3115A2::powerUp(void) const
{
/*
 * Puts the sensor in active mode
 */
	uint8_t ctl(0);

	if ( I2C::readUInt8(_i2cAddr, CTRL_REG1, ctl, false) )
	{
		ctl |= (1<<0); // Set SBYB bit for Active mode
		return I2C::writeUInt8(_i2cAddr, CTRL_REG1, ctl, false);
	}

	return false;
}

bool MPL3115A2::setOversample(uint8_t sampleRate) const
{
/*
 * Call with a rate from 0 to 7. See page 33 for table of ratios.
 * Sets the over sample rate. Datasheet calls for 128 but you can
 * set it from 1 to 128 samples. The higher the oversample rate
 * the greater the time between data samples.
 */

	// OS cannot be larger than 0b.0111
	if ( sampleRate > 7 )
		sampleRate = 7;

	// Align it for the CTRL_REG1 register
	sampleRate <<= 3;

	uint8_t ctl(0);

	if ( I2C::readUInt8(_i2cAddr, CTRL_REG1, ctl, false) )
	{
		ctl &= B11000111; // Clear out old OS bits
		ctl |= sampleRate; // Mask in new OS bits
		return I2C::writeUInt8(_i2cAddr, CTRL_REG1, ctl, false);
	}

	return false;
}

bool MPL3115A2::enableEventFlags(void) const
{
/*
 * Enables the pressure and temp measurement event flags
 * so that we can test against them. This is recommended
 * in datasheet during setup.
 */
	// Enable all three pressure and temp event flags
	return I2C::writeUInt8(_i2cAddr, PT_DATA_CFG, 0x07, false);
}

float MPL3115A2::getAltitudeMeters(void) const
{
/*
 * Returns -999.0 if no new data is available
 */

	if ( ! waitForPressureData() )
		return -999.0;

	// Read pressure registers
	Wire.beginTransmission(_i2cAddr);
	Wire.write(OUT_P_MSB);
	// Send data to I2C dev with option for a repeated start.
	Wire.endTransmission(false);

	if ( Wire.requestFrom(_i2cAddr, (uint8_t)3) != 3 )
		return -999.0;

	uint8_t msb(Wire.read());
	uint8_t csb(Wire.read());
	uint8_t lsb(Wire.read());

	// The least significant bytes l_altitude and l_temp are 4-bit,
	// fractional values, so you must cast the calulation in (float),
	// shift the value over 4 spots to the right and divide by 16 (since
	// there are 16 values in 4-bits).
	float tempcsb( (lsb >> 4) / 16.0);

	float altitude((float)( (msb << 8) | csb) + tempcsb);

	return altitude;
}

float MPL3115A2::getPressurePa(void) const
{
/*
 * Unit must be set in barometric pressure mode.
 *
 * Returns -999.0 if no new data is available.
 */

	if ( ! waitForPressureData() )
		return -999.0;

	// Read pressure registers
	Wire.beginTransmission(_i2cAddr);
	Wire.write(OUT_P_MSB);
	// Send data to I2C dev with option for a repeated start.
	Wire.endTransmission(false);

	if ( Wire.requestFrom(_i2cAddr, (uint8_t)3) != 3 )
		return -999.0;

	uint8_t msb(Wire.read());
	uint8_t csb(Wire.read());
	uint8_t lsb(Wire.read());

	requestImmediateRead();

	// Pressure comes back as a left shifted 20 bit number
	long pressure_whole((long)msb<<16 | (long)csb<<8 | (long)lsb);
	// Pressure is an 18 bit number with 2 bits of decimal. Get rid of decimal portion.
	pressure_whole >>= 6;

	lsb &= B00110000; // Bits 5/4 represent the fractional component
	lsb >>= 4; // Get it right aligned
	float pressure_decimal((float)lsb / 4.0); // Turn it into fraction

	float pressure((float)pressure_whole + pressure_decimal);

	return pressure;
}

bool MPL3115A2::waitForPressureData(void) const
{
	if ( ! isPressureDataAvailable() )
	{
		requestImmediateRead();

		for ( int counter = 0; ! isPressureDataAvailable(); ++counter )
		{
			if ( counter > 600 )
				return false;

			delay(1);
		}
	}

	return true;
}

float MPL3115A2::getTempC(void) const
{
	if ( ! waitForTemperatureData() )
		return -999.0;

	// Read temperature registers
	Wire.beginTransmission(_i2cAddr);
	Wire.write(OUT_T_MSB);  // Address of data to get
	// Send data to I2C dev with option for a repeated start.
	// THIS IS NECESSARY and not supported before Arduino V1.0.1!
	Wire.endTransmission(false);

	if ( Wire.requestFrom(_i2cAddr, (uint8_t)2) != 2 )
		return -999.0;

	uint8_t msb(Wire.read());
	uint8_t lsb(Wire.read());

	requestImmediateRead();

	uint16_t foo(0);
    bool negSign(false);

    //Check for 2s compliment
	if ( msb > 0x7F )
	{
        foo = ~((msb << 8) + lsb) + 1;  // 2’s complement
        msb = foo >> 8;
        lsb = foo & 0x00F0;
        negSign = true;
	}

	/*
	 * The least significant bytes l_altitude and l_temp are 4-bit,
	 * fractional values, so you must cast the calulation in (float),
	 * shift the value over 4 spots to the right and divide by 16 (since
	 * there are 16 values in 4-bits).
	 */
	float templsb((lsb >> 4) / 16.0); // fraction of a degree
	float temperature((float)(msb + templsb));

	return ( negSign ) ? -temperature : temperature;
}

bool MPL3115A2::waitForTemperatureData(void) const
{
	if ( ! isTemperatureDataAvailable() )
	{
		requestImmediateRead();

		for ( int counter = 0; ! isTemperatureDataAvailable(); ++counter )
		{
			if ( counter > 600 )
				return false;

			delay(1);
		}
	}

	return true;
}

bool MPL3115A2::requestImmediateRead(void) const
{
/*
 * toggles the OST bit which causes
 * the sensor to immediately take another reading
 * Needed to sample faster than 1Hz
 */
	uint8_t ctl(0);

	if ( I2C::readUInt8(_i2cAddr, CTRL_REG1, ctl, false) )
	{
		ctl &= ~(1<<1); // Clear OST bit

		if ( I2C::writeUInt8(_i2cAddr, CTRL_REG1, ctl, false)
				&& I2C::readUInt8(_i2cAddr, CTRL_REG1, ctl, false) )
		{
			ctl |= (1<<1); // Set OST bit
			return I2C::writeUInt8(_i2cAddr, CTRL_REG1, ctl, false);
		}
	}

	return false;
}

} // namespace Xilka

