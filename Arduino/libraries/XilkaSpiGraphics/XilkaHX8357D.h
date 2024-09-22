#pragma once
/*
 * Supports the HX8357D chip (320x480) used on
 *  ----> http://www.adafruit.com/products/2050
 */

#include <XilkaGFX.h>
#include <XilkaSpiGraphics.h>
#include "XilkaHX8357X.h"

namespace Xilka {

class HX8357D : public HX8357X
{
public:

	/*
	 * ESP8266 Hardware SPI w/Hardware CS:
	 *     HX8357D(resetPin, dcPin, csPin = -1)
	 *
	 * Hardware SPI:
	 *     HX8357D(resetPin, dcPin, csPin)
	 *
	 * Software SPI:
	 *     HX8357D(resetPin, dcPin, csPin, new SpiSoftWriter(sclkPin, mosiPin))
	 *
	 * resetPin may be set to -1 if connected to the microcontroller reset pin
	 *
	 * "dc" pin may also be referred to as "rs" pin in some references
	 *
	 */
	HX8357D(int8_t resetPin, int8_t dcPin, int8_t csPin, SpiSoftWriter *_spiSoft = 0);

	void begin(void);
};

} // namespace Xilka

