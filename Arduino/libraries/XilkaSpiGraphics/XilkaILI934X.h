#pragma once

/*
 * Base library for ILI934X Graphics displays
 */

#include <SPI.h>
#include <XilkaGFX.h>
#include "XilkaSpiGraphics.h"
#include "../XilkaSpi/XilkaSpi.h"

namespace Xilka {

class ILI934X : public SpiGraphics
{
public:

	void setRotation(uint8_t r);

protected:

	ILI934X(const GFX::Dim &dspDim
			, SPISettings spiSettings
			, int8_t resetPin
			, int8_t dcPin
			, int8_t csPin
			, SpiSoftWriter *spiSoft)
		: SpiGraphics(dspDim, spiSettings, resetPin
						, dcPin, csPin, spiSoft) {}
};

} // namespace Xilka

