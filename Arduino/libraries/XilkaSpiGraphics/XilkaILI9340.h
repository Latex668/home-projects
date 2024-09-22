#pragma once
/***************************************************
 * This is an Arduino Library for the Adafruit 2.2" SPI display.
 * This library works with the Adafruit 2.2" TFT Breakout w/SD card
 * ----> http://www.adafruit.com/products/1480
 *
 * Check out the links above for our tutorials and wiring diagrams
 * These displays use SPI to communicate, 4 or 5 pins are required to
 * interface (RST is optional)
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 * MIT license, all text above must be included in any redistribution
 */

#include "XilkaILI934X.h"

namespace Xilka {

class ILI9340 : public ILI934X
{
public:
	/*
	 * ESP8266 Hardware SPI w/Hardware CS:
	 *     ILI9340(resetPin, dcPin, csPin = -1)
	 *
	 * Hardware SPI:
	 *     ILI9340(resetPin, dcPin, csPin)
	 *
	 * Software SPI:
	 *     ILI9340(resetPin, dcPin, csPin, new SpiSoftWriter(sclkPin, mosiPin))
	 *
	 * resetPin may be set to -1 if connected to the microcontroller reset pin
	 *
	 * "dc" pin may also be referred to as "rs" pin in some references
	 *
	 */
	ILI9340(int8_t resetPin, int8_t dcPin, int8_t csPin
			, SpiSoftWriter *_spiSoft = 0);

	void begin(void);
};

} // namespace Xilka

