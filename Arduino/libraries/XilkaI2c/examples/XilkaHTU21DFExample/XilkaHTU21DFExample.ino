/*
 * HTU21D-F Humidity/Temp Sensor example
 *
 * https://www.adafruit.com/products/1899
 *
 * Connect Vin to 3-5VDC
 * Connect GND to ground
 * Connect SCL to I2C clock pin (A5 on UNO)
 * Connect SDA to I2C data pin (A4 on UNO)
 */

#include <Wire.h>
#include <XilkaHTU21DF.h>

using namespace Xilka;

HTU21DF htu;

void setup()
{
    Serial.begin(115200);
    Serial.println("Xilka::HTU21DF example");

	Wire.begin();
	Wire.setClock(HTU21DF::MaxI2CSpeed);

    if ( ! htu.begin() )
    {
        Serial.println("Couldn't find HTU21D-F sensor!");

        while ( true )
			;
    }
}

void loop()
{
	float temp(htu.getTempF());

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

	float hum(htu.getHumidity());

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

