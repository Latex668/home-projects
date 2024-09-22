#pragma once

/*
 * BME280 Temp/Humidity/Pressure Sensor
 *
 * Typical Temp accuracy 1 degree C
 * Typical Humidity accuracy 3%
 * Typical Pressure accuracy 1 hPa
 *
 * Typical power draw
 *     0.1uA in powerDown mode
 *     0.2uA in standby mode (ready but not measuring)
 *     0.350mA when measuring temp
 *     0.340mA when measuring humidity
 *     0.714mA when measuring pressure
 *
 * -- look these up Typical 100mS (up to 400mS) conversion time
 * (in effect if you're toggling between powerUp/powerDown
 * it takes this long to get a measurement)
 *
 * https://www.adafruit.com/products/1899
 *
 */

namespace Xilka {

class BME280
{
public:
	/*
	 * For I2C: Wire.begin() should be called in setup()
	 *     BME280();
	 *
	 * For Hardware SPI: SPI.begin() should be called in setup()
	 *     BME280(csPin);
	 *
	 * For Software SPI:
	 *     BME280(csPin, new SpiSoftReadWriter(sclkPin, mosiPin, misoPin));
	 */
	BME280(int8_t csPin = -1, SpiSoftReadWriter *spiSoft = 0);

	bool begin(uint8_t i2cAddr = 0x77);

	float getTempC(void) { update(); return _temp; }

	float getTempF(void)
	{
		const float f(getTempC());

		return ( f != -999.0 ) ? (f * 9.0 / 5.0 + 32.0) : -999.0;
	}

	float getPressurePa(void) { update(); return _pressure; }

    float getPressureHg(void)
	{
		const float f(getPressurePa());

		return ( f != -999.0 ) ? f / 3377.0 : -999.0;
	}

	float getHumidity(void) { update(); return _humidity; }

	/*
	 * Equation taken from BMP180 datasheet (page 16):
	 * http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf
	 *
	 * Note that using the equation from wikipedia can give bad results
	 * at high altitude.  See this thread for more information:
	 * http://forums.adafruit.com/viewtopic.php?f=22&t=58064
	 */

	float getAltitudeMeters(float seaLevelHPa = 1013.25)
	{
		const float pre(getPressurePa());

		if ( pre != -999.0 )
		{
			const float atmos(pre / 100.0);
			return 44330.0 * (1.0 - pow(atmos / seaLevelHPa, 0.1903));
		}

		return -999.0;
	}

	static unsigned long MaxI2CSpeed;

private:
	bool isSpi(void) const { return _cs.getPin() != -1; }
	bool isHwSpi(void) const { return ! _spiSoft; }

	void spiBegin(void) const
	{
#if defined (SPI_HAS_TRANSACTION)
		if ( isHwSpi() )
			SPI.beginTransaction(_spiSettings);
#endif

		_cs.setLow();
	}

	void spiEnd(void) const
	{
		_cs.setHigh();

#if defined (SPI_HAS_TRANSACTION)
		if ( isHwSpi() )
			SPI.endTransaction();
#endif
	}


	void readCoefficients(void);

	void update(void);
	float calcTemp(int32_t adc_T, int32_t &t_fine) const;
	float calcPressure(int32_t adc_P, int32_t t_fine) const;
	float calcHumidity(int32_t adc_H, int32_t t_fine) const;

	bool i2cSpiRead(uint8_t reg, uint8_t *target, size_t count) const;
	uint8_t spiXfer(uint8_t x) const { return isHwSpi() ? SPI.transfer(x) : _spiSoft->readWrite(x); }
	uint8_t read8(uint8_t reg) const { uint8_t v(0); i2cSpiRead(reg, (uint8_t*)&v, sizeof(v)); return v; }
	uint16_t read16(uint8_t reg) const { uint16_t v(0); i2cSpiRead(reg, (uint8_t*)&v, sizeof(v)); return v; }
	int16_t readS16(uint8_t reg) const { return (int16_t)read16(reg); }

	uint16_t read16_LE(uint8_t reg) const
	{
		const uint16_t temp(read16(reg));
		return (temp >> 8) | (temp << 8);
	}

	int16_t readS16_LE(uint8_t reg) const { return (int16_t)read16_LE(reg); }

	bool write8(uint8_t reg, uint8_t value) const;

	uint8_t _i2cAddr;
	int32_t _sensorID;

	SPISettings _spiSettings;
	PinPortOutput _cs;
	SpiSoftReadWriter *_spiSoft;

	unsigned long _lastUpdate;
	float _pressure;
	float _temp;
	float _humidity;

	struct
	{
		uint16_t dig_T1;
		int16_t  dig_T2;
		int16_t  dig_T3;

		uint16_t dig_P1;
		int16_t  dig_P2;
		int16_t  dig_P3;
		int16_t  dig_P4;
		int16_t  dig_P5;
		int16_t  dig_P6;
		int16_t  dig_P7;
		int16_t  dig_P8;
		int16_t  dig_P9;

		uint8_t  dig_H1;
		int16_t  dig_H2;
		uint8_t  dig_H3;
		int16_t  dig_H4;
		int16_t  dig_H5;
		int8_t   dig_H6;
	} _calib;
};

} // namespace Xilka

