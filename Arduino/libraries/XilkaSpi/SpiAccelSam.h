#pragma once

#if defined(ARDUINO_ARCH_SAM) && ! defined(USE_SAM_DMA)

#define SPI_ACCEL 1
#define SPI_ACCEL16ENDIAN 1
#define SPI_ACCELFILL16ENDIAN 1

namespace Xilka {

inline __attribute__((always_inline))
void spiAccelWrite(uint8_t value, bool isLast)
{
	uint32_t ch(BOARD_PIN_TO_SPI_CHANNEL(BOARD_SPI_DEFAULT_SS));
	Spi *spi(SPI_INTERFACE);

	// wait for write ready
	while ( ! (spi->SPI_SR & SPI_SR_TDRE) )
		;

	spi->SPI_TDR = static_cast<uint32_t>(value)
					| SPI_PCS(ch)
					| (!isLast ? 0 : SPI_TDR_LASTXFER);

	// wait for read ready
	while ( ! (spi->SPI_SR & SPI_SR_RDRF) )
		;

	uint8_t r = spi->SPI_RDR; // ignore the return value
}

inline __attribute__((always_inline))
void spiAccelWrite16Endian(uint16_t value
		, bool isBigEndian, bool isLast)
{
	uint32_t ch(BOARD_PIN_TO_SPI_CHANNEL(BOARD_SPI_DEFAULT_SS));
	Spi *spi(SPI_INTERFACE);

	spi->SPI_CSR[3] &= ~SPI_CSR_BITS_Msk;
	spi->SPI_CSR[3] |= SPI_CSR_BITS_16_BIT;

	uint32_t v( ( isBigEndian )
				? (value
						| SPI_PCS(ch)
						| (!isLast ? 0 : SPI_TDR_LASTXFER))
				: ((value << 8 & 0xFF00)
						| (value >> 8 & 0xFF)
						| SPI_PCS(ch)
						| (!isLast ? 0 : SPI_TDR_LASTXFER)));

	// wait for write ready
	while ( ! (spi->SPI_SR & SPI_SR_TDRE) )
		;

	// write
	spi->SPI_TDR = v;

	// wait for read ready
	while ( ! (spi->SPI_SR & SPI_SR_RDRF) )
		;

	// read
	uint16_t r = spi->SPI_RDR; // ignore the return value

	spi->SPI_CSR[3] &= ~SPI_CSR_BITS_Msk;
	spi->SPI_CSR[3] |= SPI_CSR_BITS_8_BIT;
}

void spiAccelWrite(const uint8_t *source, size_t count, bool isLast);
void spiAccelFill16Endian(uint16_t value, size_t count, bool bigEndian, bool isLast);
void spiAccelRead(uint8_t *target, size_t count);

} // namespace Xilka

#endif // defined(ARDUINO_ARCH_SAM) && ! defined(USE_SAM_DMA)

