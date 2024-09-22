#pragma once
/*
 * Base class for the RA8875 family of chips
 */

#include "XilkaRA8875X.h"

namespace Xilka {

class RA8875_480x272 : public RA8875X
{
public:

	/*
	 * ESP8266 Hardware SPI w/Hardware CS:
	 *     RA8875_480x272(resetPin, dcPin, csPin = -1)
	 *
	 * Hardware SPI:
	 *     RA8872_480x272(resetPin, dcPin, csPin)
	 *
	 * Software SPI:
	 *     RA8875_480x272(resetPin, dcPin, csPin, new SpiSoftWriter(sclkPin, mosiPin))
	 *
	 * resetPin may be set to -1 if connected to the microcontroller reset pin
	 *
	 * "dc" pin may also be referred to as "rs" pin in some references
	 *
	 */
	RA8875_480x272(int8_t resetPin, int8_t dcPin, int8_t csPin, SpiSoftWriter *_spiSoft = 0);

	void begin(void);

private:
};

} // namespace Xilka

