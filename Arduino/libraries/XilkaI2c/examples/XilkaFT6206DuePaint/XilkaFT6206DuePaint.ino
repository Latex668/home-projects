/***************************************************
  This is our touchscreen painting example for the Adafruit ILI9341
  captouch shield
  ----> http://www.adafruit.com/products/1947

  Check out the links above for our tutorials and wiring diagrams

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <ILI9341_due.h>
#include <XilkaFT6206.h>

Xilka::FT6206 ctp; // The FT6206 uses hardware I2C (SCL/SDA)

// The display also uses hardware SPI, plus #9 & #10
#define CS 10
#define DC 9
ILI9341_due tft(CS, DC);

// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;

void setup(void)
{
	while ( ! Serial ) // used for leonardo debugging
		;

	Serial.begin(115200);
	Serial.println(F("Xilka::FT6206 Touch Paint!"));

	tft.begin();

	Wire.begin();
	Wire.setClock(Xilka::FT6206::MaxI2CSpeed);

	if ( ! ctp.begin(40) ) // pass in 'sensitivity' coefficient
	{
		Serial.println("Couldn't start Xilka::FT6206 touchscreen controller");

		while (true)
			;
	}

	Serial.println("Capacitive touchscreen started");

	tft.fillScreen(ILI9341_BLACK);

	// make the color selection boxes
	tft.fillRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_RED);
	tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_YELLOW);
	tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, ILI9341_GREEN);
	tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, ILI9341_CYAN);
	tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, ILI9341_BLUE);
	tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, ILI9341_MAGENTA);

	// select the current color 'red'
	tft.drawRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
	currentcolor = ILI9341_RED;
}

void handleTouch(void)
{
	// Retrieve a point
	Xilka::TsPoint p(ctp.getPoint());

	/*
	// Print out raw data from screen touch controller
	Serial.print("X = "); Serial.print(p.x);
	Serial.print("\tY = "); Serial.print(p.y);
	Serial.print(" -> ");
	*/

	// flip it around to match the screen.
	p.x = map(p.x, 0, 240, 240, 0);
	p.y = map(p.y, 0, 320, 320, 0);

	// Print out the remapped (rotated) coordinates
	Serial.print("(");
	Serial.print(p.x);
	Serial.print(", ");
	Serial.print(p.y);
	Serial.println(")");

	if ( p.y < BOXSIZE )
	{
		oldcolor = currentcolor;

		if ( p.x < BOXSIZE )
		{
			currentcolor = ILI9341_RED;
			tft.drawRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
		}
		else if ( p.x < BOXSIZE * 2 )
		{
			currentcolor = ILI9341_YELLOW;
			tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
		}
		else if ( p.x < BOXSIZE * 3 )
		{
			currentcolor = ILI9341_GREEN;
			tft.drawRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
		}
		else if ( p.x < BOXSIZE * 4 )
		{
			currentcolor = ILI9341_CYAN;
			tft.drawRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
		}
		else if ( p.x < BOXSIZE * 5 )
		{
			currentcolor = ILI9341_BLUE;
			tft.drawRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
		}
		else if ( p.x <= BOXSIZE * 6 )
		{
			currentcolor = ILI9341_MAGENTA;
			tft.drawRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
		}

		if ( oldcolor != currentcolor )
		{
			if ( oldcolor == ILI9341_RED )
				tft.fillRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_RED);
			else if ( oldcolor == ILI9341_YELLOW )
				tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_YELLOW);
			else if ( oldcolor == ILI9341_GREEN )
				tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, ILI9341_GREEN);
			else if ( oldcolor == ILI9341_CYAN )
				tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, ILI9341_CYAN);
			else if ( oldcolor == ILI9341_BLUE )
				tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, ILI9341_BLUE);
			else if ( oldcolor == ILI9341_MAGENTA )
				tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, ILI9341_MAGENTA);
		}
	}

	if ( ((p.y-PENRADIUS) > BOXSIZE) && ((p.y+PENRADIUS) < tft.height()) )
		tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor);
}

void loop()
{
	if ( ctp.touched() )
		handleTouch();
}

