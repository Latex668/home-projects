#pragma once

#include "XilkaSSD1306.h"

namespace Xilka {

class SSD1306_128x64 : public SSD1306
{
public:

	/*
	 * Hardware SPI:
	 *     SSD1306_128x64(resetPin, dcPin, csPin)
	 *
	 * Software SPI:
	 *     SSD1306_128x64(resetPin, dcPin, csPin, new SpiSoftWriter(sclkPin, mosiPin))
	 *
	 * I2C:
	 *     SSD1306_128x64(resetPin)
	 */
	SSD1306_128x64(int8_t resetPin, int8_t dcPin = -1, int8_t csPin = -1
			, SpiSoftWriter *spiSoft = 0)
		: SSD1306(GFX::Dim(128, 64), resetPin, dcPin, csPin, spiSoft)
	{
	}

	virtual ~SSD1306_128x64();

	/*
	 * Address for 128x32 is 0x3C
	 * Address for 128x64 is 0x3D (default) or 0x3C (if SA0 is grounded)
	 */
	void begin(bool useChargePump = true
				, uint8_t i2cAddr = 0x3D
				, bool reset = true)
	{
		SSD1306::begin(useChargePump, i2cAddr, reset);
		initDisplay(useChargePump);
	}

private:

	void initDisplay(bool useChargePump);
};

} // namespace Xilka

