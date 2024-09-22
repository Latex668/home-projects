#pragma once

#if defined(ARDUINO_ARCH_AVR) && defined(__arm__)
#include <Arduino.h>
#include <SPI.h>

#define USE_TEENSY_FIFO 1
#define SPI_ACCEL 1
#define SPI_ACCEL16 1
#define SPI_ACCELFILL16ENDIAN 1

namespace Xilka {

class TeensyFifo
{
public:
	TeensyFifo()
		: _modeData(0)
		, _modeCommand(0)
		, _modeCurrent(0)
	{
	}

	void begin(uint8_t cs, uint8_t dc)
	{
		if ( SPI.pinIsChipSelect(cs, dc) )
		{
			_modeData = SPI.setCS(cs);
			_modeCommand = _modeData | SPI.setCS(dc);
		}
	}

	void setData(bool isData) { _modeCurrent = (isData) ? _modeData : _modeCommand; }

	void write8(uint8_t v, bool isLast)
	{
		KINETISK_SPI0.PUSHR = ( ! isLast )
					? (v | (_modeCurrent << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_CONT)
					: (v | (_modeCurrent << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_EOQ);

		if ( ! isLast )
		{
			waitForSpace();
		}
		else
		{
			uint32_t mcr(SPI0_MCR);
			waitForComplete(mcr);
		}
	}

	void write16(uint16_t v, bool isLast)
	{
		KINETISK_SPI0.PUSHR = ( ! isLast )
					? (v | (_modeCurrent << 16) | SPI_PUSHR_CTAS(1) | SPI_PUSHR_CONT)
					: (v | (_modeCurrent << 16) | SPI_PUSHR_CTAS(1) | SPI_PUSHR_EOQ);

		waitForSpace();

		if ( ! isLast )
		{
			waitForSpace();
		}
		else
		{
			uint32_t mcr(SPI0_MCR);
			waitForComplete(mcr);
		}
	}

	void write(const uint8_t *source, size_t count, bool isLast)
	{
		if ( count )
		{
			while ( count-- )
			{
				KINETISK_SPI0.PUSHR = ( ! isLast || count )
							? (*source | (_modeCurrent << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_CONT)
							: (*source | (_modeCurrent << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_EOQ);

				++source;
				waitForSpace();
			}

			if ( isLast )
			{
				uint32_t mcr(SPI0_MCR);
				waitForComplete(mcr);
			}
		}
	}

	void fill16Endian(uint16_t value, size_t count, bool bigEndian, bool isLast)
	{
		if ( count )
		{
			if ( ! bigEndian )
				value = __builtin_bswap16(value);

			while ( count-- )
			{
				KINETISK_SPI0.PUSHR
					= ( ! isLast || count )
						? value
							| (_modeData << 16)
							| SPI_PUSHR_CTAS(1)
							| SPI_PUSHR_CONT
						: value
							| (_modeData << 16)
							| SPI_PUSHR_CTAS(1)
							| SPI_PUSHR_EOQ;
				waitForSpace();
			}

			if ( isLast )
			{
				uint32_t mcr(SPI0_MCR);
				waitForComplete(mcr);
			}
		}
	}

private:
	void waitForSpace(void);
	void waitForEmpty(void);
	void waitForComplete(void);
	void waitForComplete(uint32_t mcr);

	uint8_t _modeData;
	uint8_t _modeCommand;
	uint8_t _modeCurrent;
};

extern TeensyFifo teensyFifo;

inline __attribute__((always_inline))
void spiAccelWrite(uint8_t value, bool isLast)
{
	teensyFifo.write8(value, isLast);
}

inline __attribute__((always_inline))
void spiAccelWrite16(const uint16_t value, bool isLast)
{
	teensyFifo.write16(value, isLast);
}

inline __attribute__((always_inline))
void spiAccelWrite(const uint8_t *source, size_t count, bool isLast)
{
	teensyFifo.write(source, count, isLast);
}

inline __attribute__((always_inline))
void spiAccelFill16Endian(uint16_t value, size_t count
		, bool isBigEndian, bool isLast)
{
	teensyFifo.fill16Endian(value, count, isBigEndian, isLast);
}

inline __attribute__((always_inline))
void spiAccelRead(uint8_t *dest, size_t count)
{
	if ( count )
	{
		while ( count-- )
		{
			while ( ! (SPSR & _BV(SPIF)) ) // wait
				;

			SPDR = 0;

			while ( ! (SPSR & _BV(SPIF)) ) // wait
				;

			*dest++ = SPDR;
		}
	}
}

} // namespace Xilka

#endif

