#pragma once
/*
 * ILI9163C SPI display library
 */

#include <XilkaGFX.h>
#include <XilkaSpiGraphics.h>
#include "../XilkaSpi/XilkaSpi.h"

namespace Xilka {

class ILI9163CRed : public SpiGraphics
{
public:

	/*
	 * ESP8266 Hardware SPI w/Hardware CS:
	 *     ILI9163CRed(resetPin, dcPin, csPin = -1)
	 *
	 * Hardware SPI:
	 *     ILI9163CRed(resetPin, dcPin, csPin)
	 *
	 * Software SPI:
	 *     ILI9163CRed(resetPin, dcPin, csPin, new SpiSoftWriter(sclkPin, mosiPin))
	 *
	 * resetPin may be set to -1 if connected to the microcontroller reset pin
	 *
	 * "dc" pin may also be referred to as "rs" pin in some references
	 *
	 */
	ILI9163CRed(int8_t resetPin, int8_t dcPin, int8_t csPin, SpiSoftWriter *_spiSoft = 0);

	void init(void);

	void setRotation(uint8_t r);
};

} // namespace Xilka

