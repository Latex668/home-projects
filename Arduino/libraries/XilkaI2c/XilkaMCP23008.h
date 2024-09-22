#pragma once
/*
 ___  _   _  _ __ ___    ___  | |_  ___   _   _
/ __|| | | || '_ ` _ \  / _ \ | __|/ _ \ | | | |
\__ \| |_| || | | | | || (_) || |_| (_) || |_| |
|___/ \__,_||_| |_| |_| \___/  \__|\___/  \__, |
                                          |___/

	gpio_expander - An attemp to create a fast and universal library for drive many GPIO chips

model:			company:		pins:		protocol:		Special Features:
---------------------------------------------------------------------------------------------------------------------
MCP23008		Microchip		  8			I2C					INT
---------------------------------------------------------------------------------------------------------------------
Version history:
0.5b1: first release, just coded and never tested
0.5b2: fixed 2wire version, added portPullup, tested output mode (ok)
0.5b3: added some drivers
0.5b4: ability to include library inside other libraries.
0.5b7: Changed functionalities of some function.
0.6b1: Changed gpioRegisterRead to gpioRegisterReadByte. Added gpioRegisterReadWord (for some GPIO)
0.8b3: Added 2 more commands and 2 gpio chip.
---------------------------------------------------------------------------------------------------------------------
		Copyright (c) 2013-2014, s.u.m.o.t.o.y [sumotoy(at)gmail.com]
---------------------------------------------------------------------------------------------------------------------

    gpio_expander Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    gpio_expander Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.


	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Version:0.8b3: Added 2 more commands and 2 gpio chip.
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

/* ------------------------------ MCP23008 WIRING ------------------------------------
Basic Address:  00100 A2 A1 A0 (from 0x20 to 0x27)
A2,A1,A0 tied to ground = 0x20
				__ __
	<->  scl  [|  U  |] ++++
	<->  sda  [|     |] IO-7
	    A2    [|     |] IO-6
		A1    [|     |] IO-5
		A0    [|     |] IO-4
 rst (con.+)  [|     |] IO-3
		-NC-  [|     |] IO-2
		int   [|     |] IO-1
		GND   [|_____|] IO-0
*/

#include <inttypes.h>

namespace Xilka {

class MCP23008
{

public:
	MCP23008();
	MCP23008(uint8_t i2cAddr);

	void postSetup(uint8_t i2cAddr);
	void begin(bool protocolInitOverride=false); // don't init Wire if true

	void setPullup(uint8_t mask); // HIGH, LOW or mask
	void setPinsMode(uint8_t mask); // OUTPUT, INPUT or mask
	void setPinInput(uint8_t pin);
	void setPinOutput(uint8_t pin);

	void setPin(uint8_t pin, bool value);
	bool getPin(uint8_t pin);

	void setPinFast(uint8_t pin, bool value)
	{
		if ( pin <= 7 ) // MAX_PIN
			( value ) ? _gpioState |= (1 << pin) : _gpioState &= ~(1 << pin);
	}

	bool getPinFast(uint8_t pin)
	{
		return ( pin <= 7 ) ? bitRead(_gpioState, pin) : 0;
	}

	void setPins(uint8_t mask); // HIGH, LOW or mask
	uint8_t getPins(void);
	uint8_t getPinsFast(void) { return _gpioState; }

	void updatePins(void);

	static unsigned long MaxI2CSpeed;

private:
	uint8_t _i2cAddr;
	uint8_t _gpioDirection;
	uint8_t _gpioState;
	bool _error;
};

} // namespace Xilka

