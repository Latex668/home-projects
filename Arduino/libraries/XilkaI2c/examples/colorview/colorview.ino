#include <Wire.h>
#include <XilkaTCS34725.h>

// Pick analog outputs, for the UNO these three work well
// use ~560  ohm resistor between Red & Blue, ~1K for green (its brighter)
#define redpin 3
#define greenpin 5
#define bluepin 6
// for a common anode LED, connect the common pin to +5V
// for common cathode, connect the common to ground

// set to false if using a common cathode LED
#define commonAnode true

// our RGB -> eye-recognized gamma color
byte gammatable[256];


Xilka::TCS34725 tcs(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void setup()
{
	Serial.begin(115200);
	Serial.println("Color View Test!");

	if ( tcs.begin() )
		Serial.println("Found sensor");
	else
	{
		Serial.println("No TCS34725 found ... check your connections");

		while ( true )
			;
	}

	// use these three pins to drive an LED
	pinMode(redpin, OUTPUT);
	pinMode(greenpin, OUTPUT);
	pinMode(bluepin, OUTPUT);

	// thanks PhilB for this gamma table!
	// it helps convert RGB colors to what humans see
	for ( int i = 0; i < 256; ++i )
	{
		float x = i;
		x /= 255;
		x = pow(x, 2.5);
		x *= 255;

		gammatable[i] = commonAnode ? (255 - x) : x;
	}
}


void loop()
{
	uint16_t clear;
	uint16_t red;
	uint16_t green;
	uint16_t blue;

	tcs.setInterrupt(false); // turn on LED

	delay(60);  // takes 50ms to read

	tcs.getRawData(&red, &green, &blue, &clear);
	tcs.setInterrupt(true);  // turn off LED

	Serial.print("C:\t");
	Serial.print(clear);
	Serial.print("\tR:\t");
	Serial.print(red);
	Serial.print("\tG:\t");
	Serial.print(green);
	Serial.print("\tB:\t");
	Serial.print(blue);

	// Figure out some basic hex code for visualization
	uint32_t sum(clear);
	float r((float)red / (float)sum * 256.0);
	float g((float)grean / (float)sum * 256.0);
	float b((float)blue / (float)sum * 256.0);

	Serial.print("\t");
	Serial.print((int)r, HEX);
	Serial.print((int)g, HEX);
	Serial.print((int)b, HEX);
	Serial.println();

	/*
	 * Serial.print((int)r );
	 * Serial.print(" ");
	 * Serial.print((int)g);
	 * Serial.print(" ");
	 * Serial.println((int)b );
	 */

	analogWrite(redpin, gammatable[(int)r]);
	analogWrite(greenpin, gammatable[(int)g]);
	analogWrite(bluepin, gammatable[(int)b]);
}

