#if defined(ARDUINO_ARCH_SAM) && ! defined(USE_SAM_DMA)

#include <SPI.h>
#include "SpiAccelSam.h"

namespace {

void writeNoLast(Spi *spi, uint32_t ch, const uint8_t *source, size_t count)
{
	while ( count-- )
	{
		// wait for write ready
		while ( ! (spi->SPI_SR & SPI_SR_TDRE) )
			;

		spi->SPI_TDR = (*source++ | SPI_PCS(ch));

		// wait for read ready
		while ( ! (spi->SPI_SR & SPI_SR_RDRF) )
			;

		uint8_t r = spi->SPI_RDR; // ignore the return value
	}
}

void fillNoLast(Spi *spi, const uint32_t regVal, size_t count)
{
	while ( count-- )
	{
		// wait for write ready
		while ( ! (spi->SPI_SR & SPI_SR_TDRE) )
			;

		spi->SPI_TDR = regVal;

		// wait for read ready
		while ( ! (spi->SPI_SR & SPI_SR_RDRF) )
			;

		uint16_t r = spi->SPI_RDR; // ignore the return value
	}
}

}

namespace Xilka {

void spiAccelWrite(const uint8_t *source, size_t count, bool isLast)
{
	uint32_t ch(BOARD_PIN_TO_SPI_CHANNEL(BOARD_SPI_DEFAULT_SS));
	Spi *spi(SPI_INTERFACE);

	if ( ! isLast )
	{
		writeNoLast(spi, ch, source, count);
		return;
	}
	else if ( count > 1 )
	{
		writeNoLast(spi, ch, source, count - 1);
		source += count - 1;
		count = 1;
	}

	// wait for write ready
	while ( ! (spi->SPI_SR & SPI_SR_TDRE) )
		;

	spi->SPI_TDR = (*source | SPI_PCS(ch) | SPI_TDR_LASTXFER);

	// wait for read ready
	while ( ! (spi->SPI_SR & SPI_SR_RDRF) )
		;

	uint8_t r = spi->SPI_RDR; // ignore the return value
}

void spiAccelFill16Endian(uint16_t value, size_t count
		, bool isBigEndian, bool isLast)
{
	uint32_t ch(BOARD_PIN_TO_SPI_CHANNEL(BOARD_SPI_DEFAULT_SS));
	Spi *spi(SPI_INTERFACE);

	spi->SPI_MR &= ~SPI_MR_WDRBT;
	spi->SPI_CSR[3] &= ~SPI_CSR_BITS_Msk;
	spi->SPI_CSR[3] |= SPI_CSR_BITS_16_BIT;

	uint32_t regVal = ( isBigEndian )
		? value | SPI_PCS(ch)
		: (value << 8 & 0xFF00) | (value >> 8 & 0xFF) | SPI_PCS(ch);

	if ( ! isLast )
	{
		fillNoLast(spi, regVal, count);
		return;
	}
	else if ( count > 1 )
	{
		fillNoLast(spi, regVal, count - 1);
	}

	// wait for write ready
	while ( ! (spi->SPI_SR & SPI_SR_TDRE) )
		;

	spi->SPI_TDR = regVal | SPI_TDR_LASTXFER;

	// wait for read ready
	while ( ! (spi->SPI_SR & SPI_SR_RDRF) )
		;

	uint16_t r = spi->SPI_RDR; // ignore the return value

	spi->SPI_CSR[3] &= ~SPI_CSR_BITS_Msk;
	spi->SPI_CSR[3] |= SPI_CSR_BITS_8_BIT;
	spi->SPI_MR |= SPI_MR_WDRBT;
}

void spiAccelRead(uint8_t *target, size_t count)
{
	uint32_t ch(BOARD_PIN_TO_SPI_CHANNEL(BOARD_SPI_DEFAULT_SS));
	Spi *spi(SPI_INTERFACE);

	while ( count-- )
	{
		// wait for write ready
		while ( ! (spi->SPI_SR & SPI_SR_TDRE) )
			;

		spi->SPI_TDR = (count ? SPI_PCS(ch) : (SPI_PCS(ch) | SPI_TDR_LASTXFER));

		// wait for read ready
		while ( ! (spi->SPI_SR & SPI_SR_RDRF) )
			;

		*target++ = spi->SPI_RDR;
	}
}

} // namespace Xilka

#endif // defined(ARDUINO_ARCH_SAM) && ! defined(USE_SAM_DMA)

