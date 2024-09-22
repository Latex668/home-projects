#pragma once

#if defined(ARDUINO_ARCH_ESP8266)

#define SPI_ACCEL 1
#define SPI_ACCEL16 1
#define SPI_ACCELFILL16ENDIAN 1

namespace Xilka {

inline __attribute__((always_inline))
void spiAccelWrite(const uint8_t value, bool isLast)
{
	SPI.write(value);
}

inline __attribute__((always_inline))
void spiAccelWrite16(const uint16_t value, bool isLast)
{
	SPI.write16(value);
}

inline __attribute__((always_inline))
void spiAccelWrite(const uint8_t *buf, size_t count, bool isLast)
{
	SPI.writeBytes((uint8_t *)buf, (uint32_t)count);
}

inline __attribute__((always_inline))
void spiAccelRead(uint8_t *target, size_t count)
{
	uint8_t *p(target);
	uint8_t * const pEnd(p + count);

	// align to 32 bits prior to calling transferBytes
	while ( ((intptr_t)p % 4) && p != pEnd )
	{
		*p++ = SPI.transfer(0);
		--count;
	}

	if ( count )
		SPI.transferBytes(0, p, (uint32_t)count);
}

inline __attribute__((always_inline))
void esp8266_setDataBits(uint16_t bits)
{
	const uint32_t mask( ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO)) );

	--bits;

	SPI1U1 = ((SPI1U1 & mask) | ((bits << SPILMOSI) | (bits << SPILMISO)));
}


inline __attribute__((always_inline))
void spiAccelFill16Internal(uint16_t value, size_t count)
{
	while ( SPI1CMD & SPIBUSY )
		;

	esp8266_setDataBits(count * 16);

	const uint32_t v((uint32_t)value << 16 | value);
	volatile uint32_t *fifoPtr(&SPI1W0);
	size_t n(count / 2 + count % 2);

	while ( n-- )
		*fifoPtr++ = v;

	SPI1CMD |= SPIBUSY;

	while ( SPI1CMD & SPIBUSY )
		;
}

inline __attribute__((always_inline))
void spiAccelFill16Endian(uint16_t value, size_t count
	, bool isBigEndian, bool isLast)
{
	if ( isBigEndian )
		value = __builtin_bswap16(value);

	while ( count )
	{
		/*
		 * fifo is 64 bytes in size
		 * so we can only transfer 32 16 bit values
		 * at a time
		 */
		const size_t n((count > 32) ? 32 : count);

		spiAccelFill16Internal(value, n);

		count -= n;
	}
}

} // namespace Xilka

#endif // defined(ARDUINO_ARCH_ESP8266)

