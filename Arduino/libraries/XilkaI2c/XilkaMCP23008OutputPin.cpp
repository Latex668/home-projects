#include <Arduino.h>
#include "XilkaMCP23008.h"
#include "XilkaMCP23008OutputPin.h"

/* MCP23008 WIRING
Basic Address:  00100 A2 A1 A0 (from 0x20 to 0x27)
A2,A1,A0 tied to ground = 0x20
typically tie RST to V+
				__ __
	<->  scl  [|  U  |] V+
	<->  sda  [|     |] IO-7
	    A2    [|     |] IO-6
		A1    [|     |] IO-5
		A0    [|     |] IO-4
 RST (con.+)  [|     |] IO-3
		-NC-  [|     |] IO-2
		int   [|     |] IO-1
		GND   [|_____|] IO-0
*/

namespace {

bool isInit;
Xilka::MCP23008 mcp(0x20);

} // namespace

namespace Xilka {

MCP23008OutputPin::MCP23008OutputPin(uint8_t pinId, bool state)
	: DigitalOutputPin()
{
	if ( ! isInit )
	{
		isInit = true;
		mcp.begin(true); // require Wire.begin() in main sketch
		/*
		 * Make sure the gpio expander
		 * is initilized properly on reset.
		 *
		 * This is necessary when
		 * the microcontroller resets (e.g. watchdog).
		 * We need to force a reset here
		 * to set the pins into a known state.
		 */
		mcp.setPinsMode(OUTPUT);
		mcp.updatePins();
	}

	_pinId = pinId;
	setState(state);
}

void MCP23008OutputPin::setState(bool state)
{
	_state = state;
	mcp.setPin(_pinId, state);
}

} // namespace Xilka

