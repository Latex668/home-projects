#pragma once
/*
 * Base library for Spi Graphics displays
 */

#include <XilkaGFX.h>
#include "../XilkaSpi/XilkaSpi.h"
#if defined(__arm__)
#include <machine/endian.h>
#endif

namespace Xilka {

#if defined(PINK)
// mega has this defined
#undef PINK
#endif

class SpiGraphics : public GFX
{
public:

	enum Color
	{
		BLACK = 0x0000
		, NAVY = 0x000F
		, DARKGREEN = 0x03E0
		, DARKCYAN = 0x03EF
		, MAROON = 0x7800
		, PURPLE = 0x780F
		, OLIVE = 0x7BE0
		, LIGHTGRAY = 0xC618
		, DARKGRAY = 0x7BEF
		, BLUE = 0x001F
		, GREEN = 0x07E0
		, CYAN = 0x07FF
		, RED = 0xF800
		, MAGENTA = 0xF81F
		, YELLOW = 0xFFE0
		, WHITE = 0xFFFF
		, ORANGE = 0xFD20
		, GREENYELLOW = 0xAFE5
		, PINK = 0xF81F
	};

	SpiGraphics(const GFX::Dim &displayDimensions
					, SPISettings spiSettings
					, int8_t resetPin, int8_t dcPin
					, int8_t csPin, SpiSoftWriter *_spiSoft);

	/*
	 * These override function in Xilka::GFX
	 * so maintain their signature
	 */
	virtual void drawPixelNT(const GFX::Pos &p, uint16_t color);
	virtual void drawFastVLineNT(const GFX::Pos &p, int16_t lineWidth, uint16_t color);
	virtual void drawFastHLineNT(const GFX::Pos &p, int16_t lineWidth, uint16_t color);
	virtual void fillRectNT(const GFX::Pos &p, const GFX::Dim &d, uint16_t color);
	virtual void invertDisplay(bool wantInverse);

	union Data32
	{
		uint16_t w[2];
		uint32_t u32;
	};

	virtual void setWindow(const GFX::Pos &p1, const GFX::Pos &p2);
	void pushColor(uint16_t color);

protected:

	SPISettings _spiSettings;
	PinPortOutput _reset;
	PinPortOutput _dc; // "dc" pin may also be referred to as "rs" pin in some references
	PinPortOutput _cs;
	SpiSoftWriter *_spiSoft;

	bool isHwSpi(void) const { return ! _spiSoft; }

	virtual void spiBeginTran(void)
	{
#if defined (SPI_HAS_TRANSACTION)
		if ( isHwSpi() )
			SPI.beginTransaction(_spiSettings);
#endif

#if ! defined(USE_TEENSY_FIFO)
		_cs.setLow();
#endif
	}

	virtual void spiEndTran(void)
	{
#if defined(USE_TEENSY_FIFO)
		teensyFifo.write8(0, true); // NOP to finish the transfer
#else
		_cs.setHigh();
#endif

#if defined (SPI_HAS_TRANSACTION)
		if ( isHwSpi() )
			SPI.endTransaction();
#endif
	}

	void spiSetData(bool isData) const
	{
#if defined(USE_TEENSY_FIFO)
		teensyFifo.setData(isData);
#else
		_dc.set(isData);
#endif
	}

	void spiWrite(uint8_t c, bool isLast) const;

	void spiWrite(Data32 &d) const
	{
#if defined(SPI_ACCEL)
		if ( isHwSpi() )
		{
#if defined(SPI_ACCELWRITEARRAY16ENDIAN)
			spiAccelWriteArray16Endian(reinterpret_cast<const uint16_t*>(&d.w[0])
						, sizeof(d) / 2, false);
#else
#if BYTE_ORDER == LITTLE_ENDIAN
#if defined(ARDUINO_ARCH_ESP8266) || defined(__arm__)
			d.w[0] = __builtin_bswap16(d.w[0]);
			d.w[1] = __builtin_bswap16(d.w[1]);
#else
			d.w[0] = d.w[0] >> 8 | d.w[0] << 8;
			d.w[1] = d.w[1] >> 8 | d.w[1] << 8;
#endif
#endif
			spiAccelWrite(reinterpret_cast<const uint8_t*>(&d.w[0]), sizeof(d), false);
#endif
		}
		else
#endif
		{
			const uint8_t *p(reinterpret_cast<const uint8_t*>(&d.w[0]));

			spiWrite(*p++, false);
			spiWrite(*p++, false);
			spiWrite(*p++, false);
			spiWrite(*p, false);
		}
	}

#if defined(SPI_ACCEL)
	void spiWriteHard(Data32 &d) const
	{
#if defined(SPI_ACCELWRITEARRAY16ENDIAN)
			spiAccelWriteArray16Endian(reinterpret_cast<const uint16_t*>(&d.w[0])
						, sizeof(d) / 2, true);
#else
#if BYTE_ORDER == LITTLE_ENDIAN
#if defined(ARDUINO_ARCH_ESP8266) || defined(__arm__)
			d.w[0] = __builtin_bswap16(d.w[0]);
			d.w[1] = __builtin_bswap16(d.w[1]);
#else
			d.w[0] = d.w[0] >> 8 | d.w[0] << 8;
			d.w[1] = d.w[1] >> 8 | d.w[1] << 8;
#endif
#endif
			spiAccelWrite(reinterpret_cast<const uint8_t*>(&d.w[0]), sizeof(d), false);
#endif
	}
#else // defined(SPI_ACCEL)
	void spiWriteHard(Data32 &d) const
	{
#if BYTE_ORDER == LITTLE_ENDIAN
#if defined(ARDUINO_ARCH_ESP8266) || defined(__arm__)
			d.w[0] = __builtin_bswap16(d.w[0]);
			d.w[1] = __builtin_bswap16(d.w[1]);
#else
			d.w[0] = d.w[0] >> 8 | d.w[0] << 8;
			d.w[1] = d.w[1] >> 8 | d.w[1] << 8;
#endif
#endif
			SPI.transfer((uint8_t*)(&d.w[0]), sizeof(d));
	}
#endif // defined(SPI_ACCEL)

#if ! defined(ARDUINO_ARCH_ESP8266)
#if defined(SPI_HAS_TRANSACTION)
	void spiWriteHard(uint8_t d) const { SPI.transfer(d); }
#else
	void spiWriteHard(uint8_t d) const;
#endif
#endif // ! defined(ARDUINO_ARCH_ESP8266)

	void issueRomCommands(const uint8_t *addr);
	void issueRamCommands(const uint8_t *cmds);
};

} // namespace Xilka

