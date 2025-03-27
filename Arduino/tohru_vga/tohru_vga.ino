//This example shows the three methods of how sprites can be rendered on a VGA screen
//You need to connect a VGA screen cable and an external DAC (simple R2R does the job) to the pins specified below.
//cc by-sa 4.0 license
//bitluni

//include libraries
#include <ESP32Video.h>
#include <Ressources/Font6x8.h>

//include the sprites converted the SpriteConverter. Check the documentation for further infos.
#include "tooru.h"

//pin configuration
const int redPins[] = {2, 4, 12, 13, 14};
const int greenPins[] = {15, 16, 17, 18, 19};
const int bluePins[] = {21, 22, 23, 27};
const int hsyncPin = 32;
const int vsyncPin = 33;

//VGA Device
VGA14Bit videodisplay;

//initial setup
void setup()
{
	//need double buffering
	videodisplay.setFrameBufferCount(2);
	//initializing i2s vga
	videodisplay.init(VGAMode::MODE200x150, redPins, greenPins, bluePins, hsyncPin, vsyncPin);
	//setting the font
}

//just draw each frame
void loop()
{

	tooru.draw(videodisplay, 0, videodisplay.xres / 4, videodisplay.yres / 2);

	videodisplay.show();
}
