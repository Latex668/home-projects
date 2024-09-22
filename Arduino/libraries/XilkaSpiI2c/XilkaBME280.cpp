#include <Arduino.h>
#include <SPI.h>
#ifdef __AVR_ATtiny85__
#include <TinyWireM.h>
#define Wire TinyWireM
#else
#include <Wire.h>
#endif

#define IS_BIG_ENDIAN true
#if defined(__arm__)
#undef IS_BIG_ENDIAN
#include <machine/endian.h>
#if BYTE_ORDER == LITTLE_ENDIAN
#define IS_BIG_ENDIAN false
#else
#define IS_BIG_ENDIAN true
#endif
#endif // defined(__arm__)

#include "../XilkaSpi/XilkaSpi.h"
#include "../XilkaI2c/XilkaI2c.h"
#include "XilkaBME280.h"

namespace {

const uint8_t REG_DIG_T1(0x88);
const uint8_t REG_DIG_T2(0x8A);
const uint8_t REG_DIG_T3(0x8C);

const uint8_t REG_DIG_P1(0x8E);
const uint8_t REG_DIG_P2(0x90);
const uint8_t REG_DIG_P3(0x92);
const uint8_t REG_DIG_P4(0x94);
const uint8_t REG_DIG_P5(0x96);
const uint8_t REG_DIG_P6(0x98);
const uint8_t REG_DIG_P7(0x9A);
const uint8_t REG_DIG_P8(0x9C);
const uint8_t REG_DIG_P9(0x9E);

const uint8_t REG_DIG_H1(0xA1);
const uint8_t REG_DIG_H2(0xE1);
const uint8_t REG_DIG_H3(0xE3);
const uint8_t REG_DIG_H4(0xE4);
const uint8_t REG_DIG_H5(0xE5);
const uint8_t REG_DIG_H6(0xE7);

const uint8_t REG_CHIPID(0xD0);
const uint8_t REG_VERSION(0xD1);
const uint8_t REG_SOFTRESET(0xE0);

const uint8_t REG_CAL26(0xE1); // R calibration stored in 0xE1-0xF0

const uint8_t REG_CONTROLHUMID(0xF2);
const uint8_t REG_CONTROL(0xF4);
const uint8_t REG_CONFIG(0xF5);
const uint8_t REG_PRESSUREDATA(0xF7);
const uint8_t REG_TEMPDATA(0xFA);
const uint8_t REG_HUMIDDATA(0xFD);

} // namespace

namespace Xilka {

unsigned long BME280::MaxI2CSpeed(400000UL);

BME280::BME280(int8_t csPin, SpiSoftReadWriter *spiSoft)
	: _spiSettings(10000000, MSBFIRST, SPI_MODE0) // 10 MHz
	, _cs(csPin)
	, _spiSoft(spiSoft)
	, _lastUpdate(0)
	, _pressure(-999.0)
	, _temp(-999.0)
	, _humidity(-999.0)
{
	if ( csPin != -1 )
		_cs.setHigh();
}

bool BME280::begin(uint8_t i2cAddr)
{
	_i2cAddr = i2cAddr;

	if ( read8(REG_CHIPID) == 0x60 )
	{
		readCoefficients();
		write8(REG_CONTROLHUMID, 0x03);
		write8(REG_CONTROL, 0x3F);
		return true;
	}

	return false;
}

void BME280::readCoefficients(void)
{
	_calib.dig_T1 = read16_LE(REG_DIG_T1);
	_calib.dig_T2 = readS16_LE(REG_DIG_T2);
	_calib.dig_T3 = readS16_LE(REG_DIG_T3);

	_calib.dig_P1 = read16_LE(REG_DIG_P1);
	_calib.dig_P2 = readS16_LE(REG_DIG_P2);
	_calib.dig_P3 = readS16_LE(REG_DIG_P3);
	_calib.dig_P4 = readS16_LE(REG_DIG_P4);
	_calib.dig_P5 = readS16_LE(REG_DIG_P5);
	_calib.dig_P6 = readS16_LE(REG_DIG_P6);
	_calib.dig_P7 = readS16_LE(REG_DIG_P7);
	_calib.dig_P8 = readS16_LE(REG_DIG_P8);
	_calib.dig_P9 = readS16_LE(REG_DIG_P9);

	_calib.dig_H1 = read8(REG_DIG_H1);
	_calib.dig_H2 = readS16_LE(REG_DIG_H2);
	_calib.dig_H3 = read8(REG_DIG_H3);
	_calib.dig_H4 = (read8(REG_DIG_H4) << 4) | (read8(REG_DIG_H4+1) & 0xF);
	_calib.dig_H5 = (read8(REG_DIG_H5+1) << 4) | (read8(REG_DIG_H5) >> 4);
	_calib.dig_H6 = (int8_t)read8(REG_DIG_H6);
}

bool BME280::i2cSpiRead(uint8_t reg, uint8_t *target, size_t count) const
{
	if ( isSpi() )
	{
		if ( isHwSpi() )
		{

			spiBegin();

#if defined(SPI_ACCEL)
			spiAccelWrite((uint8_t)(reg | 0x80)); // read, bit 7 high
#if defined(SPI_ACCELREADARRAY16ENDIAN)
		if ( ! ((intptr_t)target % 2) ) // if it's aligned
		{
			const size_t n(count / 2);

			if ( n )
			{
				spiAccelReadArray16Endian(reinterpret_cast<uint16_t*>(target)
						, n, IS_BIG_ENDIAN);

				const size_t nBytes(n * 2);
				target += nBytes;
				count -= nBytes;
			}
		}
#endif
			spiAccelRead(target, count);
#else
			SPI.transfer(reg | 0x80); // read, bit 7 high

			uint8_t *p(target);
			const uint8_t * const pEnd(p + count);

			while ( p != pEnd )
				*p++ = SPI.transfer(0);
#endif

			spiEnd();
		}
		else
		{
			_cs.setLow();
			_spiSoft->write(reg | 0x80); // read, bit 7 high

			uint8_t *p(target);
			const uint8_t * const pEnd(p + count);

			while ( p != pEnd )
				*p++ = _spiSoft->readWrite(0);

			_cs.setHigh();
		}

		return true;
	}
	else
	{
		return I2C::read(_i2cAddr, reg, target, count);
	}
}

bool BME280::write8(uint8_t reg, uint8_t value) const
{
	if ( isSpi() )
	{
		spiBegin();

#if defined(SPI_ACCEL)
		if ( isHwSpi() )
		{
			spiAccelWrite((uint8_t)(reg & ~0x80)); // write, bit 7 low
			spiAccelWrite(value);
		}
		else
#endif
		{
			spiXfer(reg & ~0x80); // write, bit 7 low
			spiXfer(value);
		}

		spiEnd();

		return true;
	}
	else
	{
		return I2C::writeUInt8(_i2cAddr, reg, value);
	}
}

void BME280::update(void)
{
	const unsigned long ms(millis());

	// wait at least 2 sec between updates
	if ( _lastUpdate + 2000 < ms )
	{
		uint8_t raw[3+3+2]; // pressure - 3, temp - 3, humidity 2

		if ( i2cSpiRead(REG_PRESSUREDATA, raw, sizeof(raw)) )
		{
			int32_t t_fine(0);

			_temp = calcTemp((int32_t)((raw[3] << 16) | (raw[4] << 8) | raw[5]), t_fine);

			int32_t adc_P((int32_t)((raw[0] << 16) | (raw[1] << 8) | raw[2]));
			adc_P >>= 4;
			_pressure = calcPressure(adc_P, t_fine);

			_humidity = calcHumidity((int32_t)(raw[6] << 8 | raw[7]), t_fine);

			_lastUpdate = ms;
		}
	}
}

float BME280::calcTemp(int32_t adc_T, int32_t &t_fine) const
{
	int32_t v1  = ((((adc_T>>3) - ((int32_t)_calib.dig_T1 <<1)))
					* ((int32_t)_calib.dig_T2)) >> 11;

	int32_t v2  = (((((adc_T>>4) - ((int32_t)_calib.dig_T1))
					* ((adc_T>>4) - ((int32_t)_calib.dig_T1))) >> 12)
					* ((int32_t)_calib.dig_T3)) >> 14;

	t_fine = v1 + v2;

	float T((t_fine * 5 + 128) >> 8);
	return T / 100.0;
}

float BME280::calcPressure(int32_t adc_P, int32_t t_fine) const
{
	int64_t v1(((int64_t)t_fine) - 128000);
	int64_t v2(v1 * v1 * (int64_t)_calib.dig_P6);

	v2 = v2 + ((v1*(int64_t)_calib.dig_P5)<<17);
	v2 = v2 + (((int64_t)_calib.dig_P4)<<35);
	v1 = ((v1 * v1 * (int64_t)_calib.dig_P3)>>8)
			+ ((v1 * (int64_t)_calib.dig_P2)<<12);
	v1 = (((((int64_t)1)<<47)+v1))*((int64_t)_calib.dig_P1)>>33;

	if ( v1 == 0 )
		return 0.0; // avoid division by zero exception

	int64_t p(1048576 - adc_P);

	p = (((p<<31) - v2)*3125) / v1;
	v1 = (((int64_t)_calib.dig_P9) * (p>>13) * (p>>13)) >> 25;
	v2 = (((int64_t)_calib.dig_P8) * p) >> 19;
	p = ((p + v1 + v2) >> 8) + (((int64_t)_calib.dig_P7)<<4);

	return (float)p / 256.0;
}

float BME280::calcHumidity(int32_t adc_H, int32_t t_fine) const
{
	int32_t v_x1_u32r(t_fine - ((int32_t)76800));

	v_x1_u32r = (((((adc_H << 14) - (((int32_t)_calib.dig_H4) << 20)
				- (((int32_t)_calib.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15)
					* (((((((v_x1_u32r * ((int32_t)_calib.dig_H6)) >> 10)
					* (((v_x1_u32r * ((int32_t)_calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10)
					+ ((int32_t)2097152)) * ((int32_t)_calib.dig_H2) + 8192) >> 14));

	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7)
				* ((int32_t)_calib.dig_H1)) >> 4));

	v_x1_u32r = ( v_x1_u32r < 0 ) ? 0 : v_x1_u32r;
	v_x1_u32r = ( v_x1_u32r > 419430400 ) ? 419430400 : v_x1_u32r;

	float h(v_x1_u32r >> 12);
	return  h / 1024.0;
}

} // namespace Xilka

