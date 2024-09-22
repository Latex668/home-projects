#pragma once
/*************************************
 * Traditional Arduino (AVR)
 ************************************/

#if defined(ARDUINO_ARCH_AVR) && ! defined(__arm__)

#define SPI_ACCEL 1
#define SPI_ACCEL16 1
#define SPI_ACCELFILL16 1

inline __attribute__((always_inline))
void spiAccelWrite(uint8_t value, bool isLast)
{
	SPDR = value;
	asm volatile("nop");

	while ( ! (SPSR & _BV(SPIF)) )
		;
}

inline __attribute__((always_inline))
void spiAccelWrite(const uint8_t *source, size_t count, bool isLast)
{
	while ( count-- )
	{
		SPDR = *source++;
		asm volatile("nop");

		while ( ! (SPSR & _BV(SPIF)) )
			;
	}
}

inline __attribute__((always_inline))
void spiAccelFill16(uint16_t value, size_t count, bool isLast)
{
	uint8_t first;
	uint8_t second;

	if ( ! (SPCR & _BV(DORD)) )
	{
		first = (value >> 8);
		second = (value & 0xFF);
	}
	else
	{
		first = (value & 0xFF);
		second = (value >> 8);
	}

	while ( count-- )
	{
		SPDR = first;
		asm volatile("nop");

		while ( ! (SPSR & _BV(SPIF)) )
			;

		SPDR = second;
		asm volatile("nop");

		while ( ! (SPSR & _BV(SPIF)) )
			;
	}
}

inline __attribute__((always_inline))
void spiAccelWrite16(uint16_t value, bool isLast)
{
	spiAccelFill16(value, 1, isLast);
}

inline __attribute__((always_inline))
void spiAccelRead(uint8_t *dest, size_t count)
{
	while ( count-- )
	{
		SPDR = 0;

		asm volatile("nop");

		while ( ! (SPSR & _BV(SPIF)) )
			;

		*dest++ = SPDR;

		asm volatile("nop");

		while ( ! (SPSR & _BV(SPIF)) )
			;
	}
}

#endif // defined(ARDUINO_ARCH_AVR) && ! defined(__arm__)

