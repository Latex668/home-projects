#if defined(ARDUINO_ARCH_SAMD)

#include <SPI.h>
#include "SpiAccelSamd.h"

namespace Xilka {

void spiAccelRead(uint8_t *target, size_t count)
{
	SercomSpi &spi(SERCOM4->SPI);

	while( ! spi.INTFLAG.bit.DRE )
		;

	while ( count-- )
	{
		spi.DATA.bit.DATA = 0;

		while( ! spi.INTFLAG.bit.TXC
				|| ! spi.INTFLAG.bit.RXC
				|| ! spi.INTFLAG.bit.DRE )
			;

		*target++ = spi.DATA.bit.DATA;
	}
}

void spiAccelFill16Endian(uint16_t value, size_t count
		, bool isBigEndian, bool isLast)
{
	uint8_t first;
	uint8_t second;

	if ( isBigEndian )
	{
		first = (value >> 8);
		second = (value & 0xFF);
	}
	else
	{
		first = (value & 0xFF);
		second = (value >> 8);
	}

	SercomSpi &spi(SERCOM4->SPI);

	while( ! spi.INTFLAG.bit.DRE )
		;

	while ( count-- )
	{
		spi.DATA.bit.DATA = first;

		while( ! spi.INTFLAG.bit.TXC
				|| ! spi.INTFLAG.bit.DRE )
			;

		spi.DATA.bit.DATA = second;

		while( ! spi.INTFLAG.bit.TXC
				|| ! spi.INTFLAG.bit.DRE )
			;
	}
}

} // namespace Xilka

#endif // defined(ARDUINO_ARCH_SAMD)

