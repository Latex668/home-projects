#pragma once
/*
 * Base class for the HX8357 family of chips
 */

#include <XilkaGFX.h>
#include <XilkaSpiGraphics.h>
#include "../XilkaSpi/XilkaSpi.h"

namespace Xilka {

class HX8357X : public SpiGraphics
{
public:

	void setRotation(uint8_t r);

protected:

	HX8357X(const GFX::Dim &dspDim
			, SPISettings spiSettings
			, int8_t resetPin
			, int8_t dcPin
			, int8_t csPin
			, SpiSoftWriter *spiSoft)
		: SpiGraphics(dspDim, spiSettings, resetPin
						, dcPin, csPin, spiSoft) {}
};

} // namespace Xilka

