#if defined(ARDUINO_ARCH_AVR) && defined(__arm__)

#include "SpiAccelTeensy3X.h"

namespace Xilka {

void TeensyFifo::waitForSpace(void)
{
	uint32_t sr;

	do
	{
		uint32_t tmp __attribute__((unused));

		sr = KINETISK_SPI0.SR;

		if ( sr & 0xF0 )
			tmp = KINETISK_SPI0.POPR;  // drain RX FIFO
	} while ( (sr & (15 << 12)) > (3 << 12) );
}

void TeensyFifo::waitForEmpty(void)
{
	uint32_t sr;

	do
	{
		uint32_t tmp __attribute__((unused));

		sr = KINETISK_SPI0.SR;

		if ( sr & 0xF0 )
			tmp = KINETISK_SPI0.POPR;  // drain RX FIFO

	} while ( (sr & 0xF0F0) > 0 ); // wait both RX & TX empty
}

void TeensyFifo::waitForComplete(void)
{
	while ( ! (KINETISK_SPI0.SR & SPI_SR_TCF) ) // wait until final output done
		;

	uint32_t tmp __attribute__((unused));
	tmp = KINETISK_SPI0.POPR; // drain the final RX FIFO word
}

void TeensyFifo::waitForComplete(uint32_t mcr)
{
	uint32_t tmp __attribute__((unused));

	for ( ; ; )
	{
		uint32_t sr = KINETISK_SPI0.SR;

		if ( sr & SPI_SR_EOQF ) // wait for last transmit
			break;

		if ( sr &  0xF0 )
			tmp = KINETISK_SPI0.POPR;
	}

	KINETISK_SPI0.SR = SPI_SR_EOQF;
	SPI0_MCR = mcr;

	while ( KINETISK_SPI0.SR & 0xF0 )
		tmp = KINETISK_SPI0.POPR;
}

TeensyFifo teensyFifo;

} // namespace Xilka

#endif // defined(ARDUINO_ARCH_AVR) && defined(__arm__)

