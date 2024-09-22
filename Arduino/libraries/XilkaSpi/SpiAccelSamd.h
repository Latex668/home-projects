#pragma once

#if defined(ARDUINO_ARCH_SAMD)

#define SPI_ACCEL 1
#define SPI_ACCEL16ENDIAN 1
#define SPI_ACCELFILL16ENDIAN 1

namespace Xilka {

inline __attribute__((always_inline))
void spiAccelWrite(const uint8_t value, bool isLast)
{
	SercomSpi &spi(SERCOM4->SPI);

	while( ! spi.INTFLAG.bit.DRE )
		;

	spi.DATA.bit.DATA = value;

	while( ! spi.INTFLAG.bit.TXC
			|| ! spi.INTFLAG.bit.DRE )
		;
}

inline __attribute__((always_inline))
void spiAccelWrite(const uint8_t *source, size_t count, bool isLast)
{
	SercomSpi &spi(SERCOM4->SPI);

	while( ! spi.INTFLAG.bit.DRE )
		;

	while ( count-- )
	{
		spi.DATA.bit.DATA = *source++;

		while( ! spi.INTFLAG.bit.TXC
				|| ! spi.INTFLAG.bit.DRE )
			;
	}
}

void spiAccelRead(uint8_t *target, size_t count);
void spiAccelFill16Endian(uint16_t value, size_t count
		, bool isBigEndian, bool isLast);

inline __attribute__((always_inline))
void spiAccelWrite16Endian(uint16_t value
		, bool isBigEndian, bool isLast)
{
	spiAccelFill16Endian(value, 1, isBigEndian, isLast);
}

} // namespace Xilka

#endif // defined(ARDUINO_ARCH_SAMD)

