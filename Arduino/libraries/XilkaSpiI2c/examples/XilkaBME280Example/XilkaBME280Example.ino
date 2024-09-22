/*
 * BME280 Humidity/Temp/Pressure Sensor example
 */

#include <Wire.h>
#include <SPI.h>
#include <XilkaI2c.h>
#include <XilkaSpi.h>
#include <XilkaBME280.h>

using namespace Xilka;

const int8_t PIN_CS(10);
const int8_t PIN_SCLK(13);
const int8_t PIN_MOSI(11);
const int8_t PIN_MISO(12);

#define WANT_I2C 1
//#define WANT_SPI_HARD 1
//#define WANT_SPI_SOFT 1

#if defined(WANT_I2C)
BME280 bme; // I2C
#elif defined(WANT_SPI_HARD)
BME280 bme(PIN_CS); // hardware SPI
#elif defined(WANT_SPI_SOFT)
SpiSoftReadWriter spiSoftReadWriter(PIN_SCLK, PIN_MOSI, PIN_MISO);
BME280 bme(PIN_CS, &spiSoftReadWriter);
#endif

void setup()
{
    Serial.begin(115200);
    Serial.println("Xilka::BME280 example");

#if defined(WANT_I2C)
    Wire.begin();
    Wire.setClock(BME280::MaxI2CSpeed);
#elif defined(WANT_SPI_HARD)
	SPI.begin();
#endif

    if ( ! bme.begin() )
    {
        Serial.println("Couldn't find BME280 sensor!");

        while ( true )
			;
    }
}

void loop()
{
	float temp(bme.getTempF());

	if ( temp != -999.0 )
	{
		Serial.print("Temp(C): ");
		Serial.print(temp);
		Serial.print("\t");
	}
	else
	{
		Serial.print("I2C error: ");
		Serial.print(I2C::getLastErrorMessage());
	}

	float hum(bme.getHumidity());

	if ( hum != -999.0 )
	{
		Serial.print("Hum(%): ");
		Serial.print(hum);
	}
	else
	{
		Serial.print("I2C error: ");
		Serial.print(I2C::getLastErrorMessage());
	}

	float pre(bme.getPressurePa());

	if (  pre != -999.0 )
	{
		Serial.print("Pressure(hPa): ");
		Serial.print(pre/100.0);
	}
	else
	{
		Serial.print("I2C error: ");
		Serial.print(I2C::getLastErrorMessage());
	}

	float alt(bme.getAltitudeMeters());

	if (  alt != -999.0 )
	{
		Serial.print("Altitude(m): ");
		Serial.print(alt);
	}
	else
	{
		Serial.print("I2C error: ");
		Serial.print(I2C::getLastErrorMessage());
	}

	Serial.println("");

    delay(1000);
}

