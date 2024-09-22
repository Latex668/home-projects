/*
 * Xilka::TSL2591 example
 *
 * http://www.adafruit.com/products/1980
 */

#include <Wire.h>
#include <Time.h>
#include <XilkaTSL2591.h>

using namespace Xilka;

TSL2591 tsl;

void setup()
{
	Serial.begin(115200);
	Serial.println("Xilka::TSL2591 example");

	Wire.begin();
	Wire.setClock(TSL2591::MaxI2CSpeed);

	/*
	 * You can pass nothing to light.begin() for the default I2C address (0x39),
	 * or use one of the following presets if you have changed
	 * the ADDR jumper on the board:

	 * 0x29 with '0' shorted on board
	 * 0x39 default address
	 * 0x49 with '1' shorted on board
	 */

	// Initialize the TSL2591 sensor
	if ( !  tsl.begin() )
	{
		Serial.println("unable to detect TSL2591");

		while ( true )
			;
	}

	// Get factory ID from sensor:
	// (Just for fun, you don't need to do this to operate the sensor)

	uint8_t id;

	if ( tsl.getId(id) )
	{
		Serial.print("Got factory ID: 0X");
		Serial.print(id, HEX);
		Serial.println(", should be 0X5X");
	}
	else
	{
		Serial.print("I2C error: ");
		Serial.print(I2C::getLastError(), DEC);
		Serial.print(", ");
		Serial.println(I2C::getLastErrorMessage());
	}

	// tsl.setGainLow(); // 1x
	// tsl.setGainMedium(); // 25x
	// tsl.setGainHigh(); // 428x
	// tsl.setGainMax(); // 9876x
	// tsl.setIntegrationMs100();
	// tsl.setIntegrationMs200();
	// tsl.setIntegrationMs300();
	// tsl.setIntegrationMs400();
	// tsl.setIntegrationMs500();
	// tsl.setIntegrationMs600();
}

void loop()
{
	double lux(0.0);

	if ( tsl.getLux(lux) )
	{
		Serial.print(" lux: ");
		Serial.println(lux);
	}
	else
	{
		Serial.print("I2C error: ");
		Serial.print(I2C::getLastError(), DEC);
		Serial.print(", ");
		Serial.println(I2C::getLastErrorMessage());
	}

	delay(500);
}

