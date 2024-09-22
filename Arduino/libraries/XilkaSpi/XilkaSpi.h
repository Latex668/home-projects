#pragma once

#include <Arduino.h>
#include <SPI.h>

#if defined(ARDUINO_ARCH_SAM)
#define HAS_SPI_TRANSFER_BYTES 1
#include "SpiAccelSam.h"
#elif defined(ARDUINO_ARCH_SAMD)
#define HAS_SPI_TRANSFER_BYTES 1
#include "SpiAccelSamd.h"
#elif defined(ARDUINO_ARCH_ESP8266)
#define HAS_SPI_TRANSFER_BYTES 1
#include "SpiAccelEsp8266.h"
#elif defined(ARDUINO_ARCH_AVR)
#if defined(__arm__)
#include "SpiAccelTeensy3X.h"
#else
#include "SpiAccelAvr.h"
#endif
#else // defined(ARDUINO_ARCH_SAM)

namespace Xilka {

#if defined(ARDUINO_ARCH_AVR)
#define HAS_SPI_TRANSFER_BYTES 1
#elif defined(__arm__)
#if defined(ARDUINO_ARCH_SAM)
#define HAS_SPI_TRANSFER_BYTES 1
#elif defined(ARDUINO_ARCH_SAMD)
#define HAS_SPI_TRANSFER_BYTES 1
#endif
#endif

#if defined(ARDUINO_ARCH_ESP8266)
#elif defined(HAS_SPI_TRANSFER_BYTES)
#define SPI_ACCEL 1
#undef HAS_SPI_TRANSFER_BYTES

inline __attribute__((always_inline))
void spiAccelWrite(const uint8_t value)
{
	SPI.transfer(value);
}

#if defined(ARDUINO_ARCH_AVR)
#define SPI_ACCEL16 1
#define SPI_ACCELFILL16 1

#if defined(__arm__)
/*
 * Teensy 3.x version
 */

#else // defined(__arm__)
/*
 * Normal AVR version
 */
inline __attribute__((always_inline))
void spiAccelFill16(uint16_t value, size_t count)
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

		while ( !(SPSR & _BV(SPIF)) )
			;

		SPDR = second;
		asm volatile("nop");

		while ( ! (SPSR & _BV(SPIF)) )
			;
	}
}

#endif // defined(__arm__)
#define spiAccelWrite16(value) spiAccelFill16(value, 1)
#endif // defined(ARDUINO_ARCH_AVR)

#if defined(ARDUINO_ARCH_SAMD)

#define SPI_ACCELFILL16ENDIAN 1
#define SPI_ACCEL16ENDIAN 1

inline __attribute__((always_inline))
void spiAccelWrite(const uint8_t *source, size_t count)
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

inline __attribute__((always_inline))
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

inline __attribute__((always_inline))
void spiAccelFill16Endian(uint16_t value, size_t count, bool bigEndian)
{
	uint8_t first;
	uint8_t second;

	if ( bigEndian )
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

#define spiAccelWrite16Endian(value, endian) spiAccelFill16Endian(value, 1, endian)

#elif defined(ARDUINO_ARCH_SAM)

#define SPI_ACCEL16ENDIAN 1
#define SPI_ACCELFILL16ENDIAN 1
#define SPI_ACCELREADARRAY16ENDIAN 1
#define SPI_ACCELWRITEARRAY16ENDIAN 1

inline __attribute__((always_inline))
void spiAccelWrite(const uint8_t *source, size_t count)
{
	uint32_t ch(BOARD_PIN_TO_SPI_CHANNEL(BOARD_SPI_DEFAULT_SS));
	Spi *spi(SPI_INTERFACE);

	while ( count-- )
	{
		// wait for write ready
		while ( ! (spi->SPI_SR & SPI_SR_TDRE) )
			;

		uint32_t b(*source++ | SPI_PCS(ch));

		spi->SPI_TDR = (count ? b : (b | SPI_TDR_LASTXFER));

		// wait for read ready
		while ( ! (spi->SPI_SR & SPI_SR_RDRF) )
			;

		uint8_t r = spi->SPI_RDR; // ignore the return value
	}
}

inline __attribute__((always_inline))
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

inline __attribute__((always_inline))
void spiAccelWrite16Endian(uint16_t value, bool bigEndian)
{
	uint32_t ch(BOARD_PIN_TO_SPI_CHANNEL(BOARD_SPI_DEFAULT_SS));
	Spi *spi(SPI_INTERFACE);

	spi->SPI_CSR[3] &= ~SPI_CSR_BITS_Msk;
	spi->SPI_CSR[3] |= SPI_CSR_BITS_16_BIT;

	uint32_t v( ( bigEndian )
				? (value
						| SPI_PCS(ch)
						| SPI_TDR_LASTXFER)
				: ((value << 8 & 0xFF00)
						| (value >> 8 & 0xFF)
						| SPI_PCS(ch)
						| SPI_TDR_LASTXFER));

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

inline __attribute__((always_inline))
void spiAccelReadArray16Endian(uint16_t *dest, size_t count, bool bigEndian)
{
	uint32_t ch(BOARD_PIN_TO_SPI_CHANNEL(BOARD_SPI_DEFAULT_SS));
	Spi *spi(SPI_INTERFACE);

	spi->SPI_CSR[3] &= ~SPI_CSR_BITS_Msk;
	spi->SPI_CSR[3] |= SPI_CSR_BITS_16_BIT;

	while ( count-- )
	{
		// wait for write ready
		while ( ! (spi->SPI_SR & SPI_SR_TDRE) )
			;

		// write
		spi->SPI_TDR = ((count) ? SPI_PCS(ch) : (SPI_PCS(ch) | SPI_TDR_LASTXFER));

		// wait for read ready
		while ( ! (spi->SPI_SR & SPI_SR_RDRF) )
			;

		//read
		uint16_t t = spi->SPI_RDR;

		if ( bigEndian )
			*dest = t;
		else
			*dest = (t >> 8) | (t << 8);

		++dest;
	}

	spi->SPI_CSR[3] &= ~SPI_CSR_BITS_Msk;
	spi->SPI_CSR[3] |= SPI_CSR_BITS_8_BIT;
}

#if 0
inline __attribute__((always_inline))
void spiAccelWriteArray16Endian(const uint16_t *source
		, size_t count, bool bigEndian)
{
	uint32_t ch(BOARD_PIN_TO_SPI_CHANNEL(BOARD_SPI_DEFAULT_SS));
	Spi *spi(SPI_INTERFACE);

	spi->SPI_MR &= ~SPI_MR_WDRBT;
	spi->SPI_CSR[3] &= ~SPI_CSR_BITS_Msk;
	spi->SPI_CSR[3] |= SPI_CSR_BITS_16_BIT;

	if ( bigEndian )
	{
		spi->SPI_TPR = source;
		spi->SPI_TCR = count;
		spi->SPI_PTCR |= SPI_PTCR_TXEN;
	}
	else
	{
		while ( count-- )
		{
			uint32_t v(__builtin_bswap16(*source++) | SPI_PCS(ch));

			if ( ! count )
				v |= SPI_TDR_LASTXFER;

			// wait for write ready
			while ( ! (spi->SPI_SR & SPI_SR_TDRE) )
				;

			// write
			spi->SPI_TDR = v;

			// wait for read ready
			while ( ! (spi->SPI_SR & SPI_SR_RDRF) )
				;

			//read
			uint16_t r = spi->SPI_RDR; // ignore the return value
		}
	}

	spi->SPI_CSR[3] &= ~SPI_CSR_BITS_Msk;
	spi->SPI_CSR[3] |= SPI_CSR_BITS_8_BIT;
	spi->SPI_MR |= SPI_MR_WDRBT;
}

#else
inline __attribute__((always_inline))
void spiAccelWriteArray16Endian(const uint16_t *source
		, size_t count, bool bigEndian)
{
	uint32_t ch(BOARD_PIN_TO_SPI_CHANNEL(BOARD_SPI_DEFAULT_SS));
	Spi *spi(SPI_INTERFACE);

	spi->SPI_MR &= ~SPI_MR_WDRBT;
	spi->SPI_CSR[3] &= ~SPI_CSR_BITS_Msk;
	spi->SPI_CSR[3] |= SPI_CSR_BITS_16_BIT;

	if ( bigEndian )
	{
		while ( count-- )
		{
			uint32_t v(*source++ | SPI_PCS(ch));

			if ( ! count )
				v |= SPI_TDR_LASTXFER;

			// wait for write ready
			while ( ! (spi->SPI_SR & SPI_SR_TDRE) )
				;

			// write
			spi->SPI_TDR = v;

			// wait for read ready
			while ( ! (spi->SPI_SR & SPI_SR_RDRF) )
				;

			uint16_t r = spi->SPI_RDR; // ignore the return value
		}
	}
	else
	{
		while ( count-- )
		{
			uint32_t v(__builtin_bswap16(*source++) | SPI_PCS(ch));

			if ( ! count )
				v |= SPI_TDR_LASTXFER;

			// wait for write ready
			while ( ! (spi->SPI_SR & SPI_SR_TDRE) )
				;

			// write
			spi->SPI_TDR = v;

			// wait for read ready
			while ( ! (spi->SPI_SR & SPI_SR_RDRF) )
				;

			//read
			uint16_t r = spi->SPI_RDR; // ignore the return value
		}
	}

	spi->SPI_CSR[3] &= ~SPI_CSR_BITS_Msk;
	spi->SPI_CSR[3] |= SPI_CSR_BITS_8_BIT;
	spi->SPI_MR |= SPI_MR_WDRBT;
}
#endif

inline __attribute__((always_inline))
void spiAccelFill16Endian(uint16_t value, size_t count, bool bigEndian)
{
	uint32_t ch(BOARD_PIN_TO_SPI_CHANNEL(BOARD_SPI_DEFAULT_SS));
	Spi *spi(SPI_INTERFACE);

	spi->SPI_MR &= ~SPI_MR_WDRBT;
	spi->SPI_CSR[3] &= ~SPI_CSR_BITS_Msk;
	spi->SPI_CSR[3] |= SPI_CSR_BITS_16_BIT;

	uint32_t v = ( bigEndian )
		? value | SPI_PCS(ch)
		: (value << 8 & 0xFF00) | (value >> 8 & 0xFF) | SPI_PCS(ch);

	while ( count-- )
	{
		const uint32_t w((count) ? v : (v | SPI_TDR_LASTXFER));

		// wait for write ready
		while ( ! (spi->SPI_SR & SPI_SR_TDRE) )
			;

		spi->SPI_TDR = w;

		// wait for read ready
		while ( ! (spi->SPI_SR & SPI_SR_RDRF) )
			;

		uint16_t r = spi->SPI_RDR; // ignore the return value
	}

	spi->SPI_CSR[3] &= ~SPI_CSR_BITS_Msk;
	spi->SPI_CSR[3] |= SPI_CSR_BITS_8_BIT;
	spi->SPI_MR |= SPI_MR_WDRBT;
}

#else // defined(ARDUINO_ARCH_SAM)

inline __attribute__((always_inline))
void spiAccelWrite(const uint8_t *source, size_t count)
{
	while ( count )
	{
		uint8_t *buf[32];
		size_t n = count < 32 ? count : 32;

		memcpy(buf, source, n);
		SPI.transfer(buf, n);
		count -= n;
	}
}

inline __attribute__((always_inline))
void spiAccelRead(uint8_t *target, size_t count)
{
	memset(target, 0, count);
	SPI.transfer(target, count);
}

#endif // defined(ARDUINO_ARCH_SAMD)
#endif // defined(ARDUINO_ARCH_ESP8266)

} // namespace Xilka

#endif // defined(ARDUINO_ARCH_SAM) "from way at the top, testing"

namespace Xilka {

#if defined(USE_TEENSY_FIFO)
inline __attribute__((always_inline))
void spiBegin(uint8_t cs, uint8_t dc)
{
	SPI.begin();
	teensyFifo.begin(cs, dc);
}
#else // defined(USE_TEENSY_FIFO)
inline __attribute__((always_inline))
void spiBegin(void)
{
#if defined(USE_SAM_DMA)
	samDmaBegin();
#else
	SPI.begin();
#endif
}
#endif // defined(USE_TEENSY_FIFO)

#if defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_SAMD)
typedef volatile RwReg PortReg;
typedef uint32_t PortMask;
#elif defined(ARDUINO_ARCH_ESP8266)
typedef volatile uint32_t PortReg;
typedef uint32_t PortMask;
#else
typedef volatile uint8_t PortReg;
typedef uint8_t PortMask;
#endif

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_SAMD)
class PinPortOutput
{
public:
	PinPortOutput(int8_t pin) : _pin(pin)
	{
		if ( pin != -1 )
			pinMode(pin, OUTPUT);
	}

	void setHigh(void) const { if ( _pin != -1 ) set(true); }
	void setLow(void) const { if ( _pin != -1 ) set(false); }
	void set(bool isHigh) const { digitalWrite(_pin, isHigh); }
	int8_t getPin(void) const { return _pin; }

private:
	int8_t _pin;
};

class PinPortInput
{
public:
	PinPortInput(int8_t pin) : _pin(pin)
	{
		if ( pin != -1 )
			pinMode(pin, INPUT);
	}

	operator bool() const { return ( _pin != -1 ) ? digitalRead(_pin) : false; }
	int8_t getPin(void) const { return _pin; }

private:
	int8_t _pin;
};

#else // defined(ARDUINO_ARCH_ESP8266)

class PinPortOutput
{
public:
	PinPortOutput(int8_t pin) : _pin(pin), _port(0), _mask(0), _notMask(0)
	{
		if ( pin != -1 )
		{
			pinMode(pin, OUTPUT);
			_port = portOutputRegister(digitalPinToPort(pin));
			_mask = digitalPinToBitMask(pin);
			_notMask = ~_mask;
		}
	}

	void setHigh(void) const { if ( _pin != -1 ) *_port |= _mask; }
	void setLow(void) const { if ( _pin != -1 ) *_port &= _notMask; }
	void set(bool isHigh) const { (isHigh) ? setHigh() : setLow(); }
	int8_t getPin(void) const { return _pin; }

private:
	int8_t _pin;
	PortReg *_port;
	PortMask _mask;
	PortMask _notMask;
};

class PinPortInput
{
public:
	PinPortInput(int8_t pin) : _pin(pin), _port(0), _mask(0)
	{
		if ( pin != -1 )
		{
			pinMode(pin, INPUT);
			_port = portOutputRegister(digitalPinToPort(pin));
			_mask = digitalPinToBitMask(pin);
		}
	}

	operator bool() const { return *_port & _mask; }
	int8_t getPin(void) const { return _pin; }

private:
	int8_t _pin;
	PortReg *_port;
	PortMask _mask;
};

#endif // defined(ARDUINO_ARCH_ESP8266)

class SpiSoftWriter
{
public:
	SpiSoftWriter(int8_t sclkPin, int8_t mosiPin)
		: _sclk(sclkPin)
		, _mosi(mosiPin)
	{
		_sclk.setLow();
		_mosi.setLow();
	}

	inline void write(uint8_t d)
	{
		for ( uint8_t bit = 0b10000000; bit; bit >>= 1 )
		{
			_sclk.setLow();
			_mosi.set(d & bit);
			_sclk.setHigh();
		}
	}

protected:

	PinPortOutput _sclk;
	PinPortOutput _mosi;
};

class SpiSoftReadWriter : public SpiSoftWriter
{
public:
	SpiSoftReadWriter(int8_t sclkPin, int8_t mosiPin, int8_t misoPin)
		: SpiSoftWriter(sclkPin, mosiPin)
		, _miso(misoPin)
	{}

	inline uint8_t readWrite(uint8_t d)
	{
		uint8_t reply(0);

		for ( uint8_t bit = 0b10000000; bit; bit >>= 1 )
		{
			reply <<= 1;

			_sclk.setLow();
			_mosi.set(d & bit);
			_sclk.setHigh();

			if ( _miso )
				reply |= 1;
		}

		return reply;
	}

protected:

	PinPortInput _miso;
};

} // namespace Xilka

