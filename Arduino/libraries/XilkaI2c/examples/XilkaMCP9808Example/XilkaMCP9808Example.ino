/*
 * MCP9808 High Accuracy Sensor example
 *
 * https://www.adafruit.com/products/1782
 *
 * Connect Vin to 3-5VDC
 * Connect GND to ground
 * Connect SCL to I2C clock pin (A5 on UNO)
 * Connect SDA to I2C data pin (A4 on UNO)
 */

#include <Wire.h>
#include <XilkaMCP9808.h>

using namespace Xilka;

MCP9808 mcp;

void setup()
{
    Serial.begin(115200);
    Serial.println("Xilka::MCP9808 example");

	Wire.begin();
	Wire.setClock(MCP9808::MaxI2CSpeed);

    if ( ! mcp.begin() )
    {
        Serial.println("Couldn't find MCP9808 sensor!");

        while ( true )
			;
    }
}

void loop()
{
	mcp.powerUp();

	float temp(mcp.getTempF());

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

	Serial.println("");

	mcp.powerDown();

    delay(1000);
}

