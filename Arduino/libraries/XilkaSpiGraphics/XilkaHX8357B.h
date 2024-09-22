#pragma once
/*
 * Supports the HX8357B chip (320x480)
 */

#include <XilkaGFX.h>
#include <XilkaSpiGraphics.h>
#include "XilkaHX8357X.h"

namespace Xilka {

class HX8357B : public HX8357X
{
public:

	/*
	 * ESP8266 Hardware SPI w/Hardware CS:
	 *     HX8357B(resetPin, dcPin, csPin = -1)
	 *
	 * Hardware SPI:
	 *     HX8357B(resetPin, dcPin, csPin)
	 *
	 * Software SPI:
	 *     HX8357B(resetPin, dcPin, csPin, new SpiSoftWriter(sclkPin, mosiPin))
	 *
	 * resetPin may be set to -1 if connected to the microcontroller reset pin
	 *
	 * "dc" pin may also be referred to as "rs" pin in some references
	 *
	 */
	HX8357B(int8_t resetPin, int8_t dcPin, int8_t csPin, SpiSoftWriter *_spiSoft = 0);

	void begin(void);
};

} // namespace Xilka

