/*
 * Xilka::TSL2561 example

Product page: https://www.sparkfun.com/products/11824
Hook-up guide: https://learn.sparkfun.com/tutorials/getting-started-with-the-tsl2561-luminosity-sensor

Hardware connections:

3V3 to 3.3V
GND to GND

(WARNING: do not connect 3V3 to 5V
or the sensor will be damaged!)

You will also need to connect the I2C pins (SCL and SDA) to your Arduino.
The pins are different on different Arduinos:

                    SDA    SCL
Any Arduino         "SDA"  "SCL"
Uno, Redboard, Pro  A4     A5
Mega2560, Due       20     21
Leonardo            2      3

You do not need to connect the INT (interrupt) pin
for basic operation.

*/

#include <Wire.h>
#include <Time.h>
#include <XilkaTSL2561.h>

using namespace Xilka;

TSL2561 tsl;

void setup()
{
	Serial.begin(115200);
	Serial.println("Xilka::TSL2561 example");

	Wire.begin();
	Wire.setClock(TSL2561::MaxI2CSpeed);

	/*
	 * You can pass nothing to light.begin() for the default I2C address (0x39),
	 * or use one of the following presets if you have changed
	 * the ADDR jumper on the board:

	 * 0x29 with '0' shorted on board
	 * 0x39 default address
	 * 0x49 with '1' shorted on board
	 */

	// Initialize the TSL2561 sensor
	if ( !  tsl.begin() )
	{
		Serial.println("unable to detect TSL2561");

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

	// tsl.setGainLow(); default 1x gain
	// tsl.setGainHigh(); 16x gain
	// tsl.setIntegrationMs402(); default (best/slowest) integration time
	// tsl.setIntegrationMs101(); quicker integration time
	// tsl.setIntegrationMs14(); quickest integration time
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

