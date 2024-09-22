/*
 * HDC1008 Humidity/Temp Sensor example
 *
 * https://www.adafruit.com/products/2635
 *
 * Connect Vin to 3-5VDC
 * Connect GND to ground
 * Connect SCL to I2C clock pin (A5 on UNO)
 * Connect SDA to I2C data pin (A4 on UNO)
 */

#include <Wire.h>
#include <XilkaHDC1008.h>

using namespace Xilka;

HDC1008 hdc;

void setup()
{
    Serial.begin(115200);
    Serial.println("Xilka::HDC1008 example");

	Wire.begin();
	Wire.setClock(HDC1008::MaxI2CSpeed);

    if ( ! hdc.begin() )
    {
        Serial.println("Couldn't find HDC1008 sensor!");

        while ( true )
			;
    }
}

void loop()
{
	float temp(hdc.getTempF());

	if ( temp != -999.0 )
	{
		Serial.print("Temp: ");
		Serial.print(temp);
		Serial.print("\t");
	}
	else
	{
		Serial.print("I2C error: ");
		Serial.print(I2C::getLastErrorMessage());
	}

	float hum(hdc.getHumidity());

	if ( hum != -999.0 )
	{
		Serial.print("Hum: ");
		Serial.print(hum);
	}
	else
	{
		Serial.print("I2C error: ");
		Serial.print(I2C::getLastErrorMessage());
	}

	Serial.println("");

    delay(1000);
}

