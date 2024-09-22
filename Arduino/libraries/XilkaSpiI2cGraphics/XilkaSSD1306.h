#pragma once

/*
 *   SSD1306 Displays
 *
 *   The driver is used in multiple displays (128x64, 128x32, etc.).
 *   Select the appropriate display below to create an appropriately
 *   sized framebuffer, etc.
 *
 *   SSD1306_128_64 128x64 pixel display
 *
 *   SSD1306_128_32 128x32 pixel display
 *
 *   SSD1306_96_16   96x16 pixel display
 *
 */
#include <Arduino.h>
#include <SPI.h>
#include <XilkaGFX.h>

#include "../XilkaSpi/XilkaSpi.h"

namespace Xilka {

class SSD1306 : public GFX
{
public:

	enum Color
	{
		BLACK = 0
		, WHITE
		, INVERSE
	};

	SSD1306(const GFX::Dim &dim
			, int8_t resetPin
			, int8_t dcPin
			, int8_t csPin
			, SpiSoftWriter *spiSoft);

	virtual ~SSD1306();

	/*
	 * These override function in GFX
	 * so maintain the signature
	 */
	virtual void drawPixelNT(const GFX::Pos &p, uint16_t color);
	virtual void drawFastVLineNT(const GFX::Pos &p
					, int16_t length, uint16_t color);
	virtual void drawFastHLineNT(const GFX::Pos &p
					, int16_t length, uint16_t color);
	virtual void fillRectNT(const GFX::Pos &p
					, const GFX::Dim &d, uint16_t color);
	virtual void invertDisplay(bool wantInverted);


	void clearDisplay(void);
	void display(void) const; // output to the display

	/*
	 * startScroll functions:
	 * Activate a scroll for rows start through stop
	 * Hint, the display is 16 rows tall.
	 *
	 * To scroll the whole display:
	 *
	 * display.startScrollRight(0x00, 0x0F)
	 *  or
	 * display.startScrollLeft(0x00, 0x0F)
	 *  or
	 * display.startScrollRightDiag(0x00, 0x0F)
	 *  or
	 * display.startScrollLeftDiag(0x00, 0x0F)
	 */
	void startScrollRight(uint8_t start = 0x00
			, uint8_t stop = 0x0F) const
	{
		startScroll(true, start, stop);
	}

	void startScrollLeft(uint8_t start = 0x00
			, uint8_t stop = 0x0F) const
	{
		startScroll(false, start, stop);
	}

	void startScrollRightDiag(uint8_t start = 0x00
			, uint8_t stop = 0x0F) const
	{
		startScrollDiag(true, start, stop);
	}

	void startScrollLeftDiag(uint8_t start = 0x00
			, uint8_t stop = 0x0F) const
	{
		startScrollDiag(false, start, stop);
	}

	void stopScroll(void) const;

	void dimDisplay(bool wantDim) const;

	static const unsigned long MaxI2CSpeed;

protected:
	void begin(bool useChargePump, uint8_t i2cAddr, bool reset);
	void issueCommand(uint8_t c) const;

private:
	void startScroll(bool isRight, uint8_t start, uint8_t stop) const;
	void startScrollDiag(bool isRight, uint8_t start, uint8_t stop) const;
	void drawFastVLineLocal(const GFX::Pos &p
					, int16_t length, uint16_t color);
	void drawFastHLineLocal(const GFX::Pos &p
					, int16_t length, uint16_t color);
	bool isSpi(void) const { return _dc.getPin() != -1; }
	bool isHwSpi(void) const { return ! _spiSoft; }

	void spiBeginTran(void) const
	{
#if defined(SPI_HAS_TRANSACTION)
		if ( isHwSpi() )
			SPI.beginTransaction(_spiSettings);
#endif

#if ! defined(USE_TEENSY_FIFO)
		_cs.setLow();
#endif
	}

	void spiEndTran(void) const
	{
#if ! defined(USE_TEENSY_FIFO)
		_cs.setHigh();
#endif

#if defined(SPI_HAS_TRANSACTION)
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

#if defined(SPI_ACCEL)
	void spiWriteHard(uint8_t d) const { spiAccelWrite(d, true); }
#elif defined(SPI_HAS_TRANSACTION)
	void spiWriteHard(uint8_t d) const { SPI.transfer(d); }
#else
	void spiWriteHard(uint8_t d) const;
#endif

	const size_t _bufferSize;
	uint8_t *_buffer;
#if defined(SPI_HAS_TRANSACTION)
	SPISettings _spiSettings;
#endif
	PinPortOutput _reset;
	PinPortOutput _dc;
	PinPortOutput _cs;
	SpiSoftWriter *_spiSoft;
	bool _useChargePump;
	int8_t _i2cAddr;
};

} // namespace Xilka

