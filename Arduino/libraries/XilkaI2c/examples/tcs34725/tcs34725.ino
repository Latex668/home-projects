#include <Wire.h>
#include <XilkaTCS34725.h>

/* Example code for the Adafruit TCS34725 breakout library */

/* Connect SCL    to analog 5
   Connect SDA    to analog 4
   Connect VDD    to 3.3V DC
   Connect GROUND to common ground */

/* Initialise with default values (int time = 2.4ms, gain = 1x) */
// Xilka::TCS34725 tcs();

/* Initialise with specific int time and gain values */
Xilka::TCS34725 tcs(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

void setup(void)
{
	Serial.begin(115200);

	if ( tcs.begin() )
		Serial.println("Found sensor");
	else
	{
		Serial.println("No TCS34725 found ... check your connections");
		while ( true )
			;
	}
}

void loop(void)
{
	uint16_t r;
	uint16_t g;
	uint16_t b;
	uint16_t c;

	tcs.getRawData(&r, &g, &b, &c);
	uint16_t colorTemp(tcs.calculateColorTemperature(r, g, b));
	uint16_t lux(tcs.calculateLux(r, g, b));

	Serial.print("Color Temp: ");
	Serial.print(colorTemp, DEC);
	Serial.print(" K - ");

	Serial.print("Lux: ");
	Serial.print(lux, DEC);
	Serial.print(" - ");

	Serial.print("R: ");
	Serial.print(r, DEC);
	Serial.print(" ");

	Serial.print("G: ");
	Serial.print(g, DEC);
	Serial.print(" ");

	Serial.print("B: ");
	Serial.print(b, DEC);
	Serial.print(" ");

	Serial.print("C: ");
	Serial.print(c, DEC);
	Serial.print(" ");

	Serial.println(" ");
}

