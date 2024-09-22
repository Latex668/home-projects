#include <Wire.h>

#include "XilkaI2CFRam.h"

namespace {

const uint8_t MB85RC_SLAVE_ID(0xF8);

} // namespace

namespace Xilka {

unsigned long I2CFRam::MaxI2CSpeed(1000000UL);

I2CFRam::I2CFRam()
	: _i2cAddr(0)
{
}

bool I2CFRam::begin(uint8_t i2cAddr)
{
	_i2cAddr = i2cAddr;

	uint16_t mid;
	uint16_t pid;

	getDeviceId(&mid, &pid);

	return ( mid == 0x00A )
			&& ( pid == 0x510 );
}

void I2CFRam::getDeviceId(uint16_t *mid, uint16_t *pid)
{
	Wire.beginTransmission(MB85RC_SLAVE_ID >> 1);
	Wire.write(_i2cAddr << 1);
	Wire.endTransmission(false);

	Wire.requestFrom(MB85RC_SLAVE_ID >> 1, 3);

	uint8_t a[3] = { 0, 0, 0 };
	a[0] = Wire.read();
	a[1] = Wire.read();
	a[2] = Wire.read();

	/* Shift values to separate manuf and prod IDs */
	/* See p.10 of http://www.fujitsu.com/downloads/MICRO/fsa/pdf/products/memory/fram/MB85RC256V-DS501-00017-3v0-E.pdf */
	*mid = (a[0] << 4) + (a[1]  >> 4);
	*pid = ((a[1] & 0x0F) << 8) + a[2];
}

void I2CFRam::write(uint16_t framAddr, uint8_t value)
{
	Wire.beginTransmission(_i2cAddr);
	Wire.write(framAddr >> 8);
	Wire.write(framAddr & 0xFF);
	Wire.write(value);
	Wire.endTransmission();
}

void I2CFRam::write(uint16_t framAddr, uint8_t *source, size_t count)
{
	while ( count )
	{
		const size_t n(count > 0xff ? 0xff : count);

		writeStride(framAddr, source, (uint8_t)n);

		source += n;
		count -= n;
	}
}

void I2CFRam::writeStride(uint16_t framAddr, uint8_t *source, uint8_t count)
{
	Wire.beginTransmission(_i2cAddr);
	Wire.write(framAddr >> 8);
	Wire.write(framAddr & 0xFF);

	const uint8_t *sEnd(source + count);

	while ( source != sEnd )
		Wire.write(*source++);

	Wire.endTransmission();
}

uint8_t I2CFRam::read(uint16_t framAddr)
{
	Wire.beginTransmission(_i2cAddr);
	Wire.write(framAddr >> 8);
	Wire.write(framAddr & 0xFF);
	Wire.endTransmission();

	Wire.requestFrom(_i2cAddr, (uint8_t)1);

	return Wire.read();
}

void I2CFRam::read(uint16_t framAddr, uint8_t *target, size_t count)
{
	while ( count )
	{
		const size_t n(count > 0xff ? 0xff : count);

		readStride(framAddr, target, (uint8_t)n);

		target += n;
		count -= n;
	}
}

void I2CFRam::readStride(uint16_t framAddr, uint8_t *target, uint8_t count)
{
	Wire.beginTransmission(_i2cAddr);
	Wire.write(framAddr >> 8);
	Wire.write(framAddr & 0xFF);
	Wire.endTransmission();

	Wire.requestFrom(_i2cAddr, count);

	const uint8_t *tEnd(target + count);

	while ( target != tEnd )
		*target++ = Wire.read();
}

} // namespace Xilka

