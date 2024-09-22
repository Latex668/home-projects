#pragma once

#include <Arduino.h>
#include <Print.h>
#include "../XilkaSpi/XilkaSpi.h"

namespace Xilka {

class GFX : public Print
{
public:
	class Pos
	{
	public:
		Pos() : x(0), y(0) {}
		//Pos(const Pos &o) : x(o.x), y(o.y) {}
		Pos(int16_t x, int16_t y) : x(x), y(y) {}
		Pos & operator = (const Pos &o) { x = o.x; y = o.y; return *this; }
		void set(int16_t x, int16_t y) { this->x = x; this->y = y; }

		int16_t x;
		int16_t y;
	};

	class Dim
	{
	public:
		Dim() : w(0), h(0) {}
		//Dim(const Dim &o) : w(o.w), h(o.h) {}
		Dim(int16_t w, int16_t h) : w(w), h(h) {}
		Dim & operator = (const Dim &o) { w = o.w; h = o.h; return *this; }
		void set(int16_t w, int16_t h) { this->w = w; this->h = h; }

		int16_t w;
		int16_t h;
	};

	GFX(const GFX::Dim &displayDimensions);

	void fillScreen(uint16_t color)
	{
		spiBeginTran();
		fillScreenNT(color);
		spiEndTran();
	}

	void drawPixel(const Pos &p, uint16_t color)
	{
		spiBeginTran();
		drawPixelNT(p, color);
		spiEndTran();
	}

	void drawLine(const Pos &p1, const Pos &p2, uint16_t color)
	{
		spiBeginTran();
		drawLineNT(p1, p2, color);
		spiEndTran();
	}

	void drawConnected(const Pos *p, size_t count, uint16_t color)
	{
		if ( count > 1 )
		{
			spiBeginTran();
			--count;

			while ( count-- )
			{
				drawLineNT(*p, *(p+1), color);
				++p;
			}

			spiEndTran();
		}
	}

	void drawRect(const Pos &p, const Dim &d, uint16_t color)
	{
		spiBeginTran();
		drawRectNT(p, d, color);
		spiEndTran();
	}

	void fillRect(const Pos &p, const Dim &d, uint16_t color)
	{
		spiBeginTran();
		fillRectNT(p, d, color);
		spiEndTran();
	}

	void drawCircle(const Pos &center, int16_t radius, uint16_t color)
	{
		spiBeginTran();
		drawCircleNT(center, radius, color);
		spiEndTran();
	}

	void fillCircle(const Pos &center, int16_t radius, uint16_t color)
	{
		spiBeginTran();
		fillCircleNT(center, radius, color);
		spiEndTran();
	}

	void drawTriangle(const Pos &p1
			, const Pos &p2, const Pos &p3
			, uint16_t color)
	{
		spiBeginTran();
		drawTriangleNT(p1, p2, p3, color);
		spiEndTran();
	}

	void fillTriangle(const Pos &p1
			, const Pos &p2, const Pos &p3
			, uint16_t color)
	{
		spiBeginTran();
		fillTriangleNT(p1, p2, p3, color);
		spiEndTran();
	}

	void drawFastVLine(const Pos &p
			, int16_t lineWidth, uint16_t color)
	{
		spiBeginTran();
		drawFastVLineNT(p, lineWidth, color);
		spiEndTran();
	}

	void drawFastHLine(const Pos &p
			, int16_t lineWidth, uint16_t color)
	{
		spiBeginTran();
		drawFastHLineNT(p, lineWidth, color);
		spiEndTran();
	}

	/*
	 * get the size of the display
	 *     raw initialized size
	 *     or current rotation
	 */
	const Dim &getRawDim(void) const { return _rawDim; }
	const Dim &getCurrentDim(void) const { return _currentDim; }

	/*
	 * These MUST be defined by the subclass:
	 */
	virtual void drawPixelNT(const Pos &p, uint16_t color) = 0;
	virtual void drawFastVLineNT(const Pos &p, int16_t lineWidth, uint16_t color) = 0;
	virtual void drawFastHLineNT(const Pos &p, int16_t lineWidth, uint16_t color) = 0;
	virtual void fillRectNT(const Pos &p, const Dim &d, uint16_t color) = 0;

	/*
	 * These MAY be overridden by the subclass
	 * to provide chip-specific optimizations.
	 */
	virtual void drawLineNT(const Pos &p1, const Pos &p2, uint16_t color);
	virtual void drawRectNT(const Pos &p, const Dim &d, uint16_t color);
	virtual void drawCircleNT(const Pos &p, int16_t radius, uint16_t color);
	virtual void fillCircleNT(const Pos &p, int16_t radius, uint16_t color);
	virtual void drawTriangleNT(const Pos &p1, const Pos &p2
					, const Pos &p3, uint16_t color);
	virtual void fillTriangleNT(const Pos &p1, const Pos &p2
					, const Pos &p3, uint16_t color);
	virtual void fillScreenNT(uint16_t color);
	virtual void invertDisplay(bool i);

	/*
	 * Override these in SPI drivers
	 */
	virtual void spiBeginTran(void);
	virtual void spiEndTran(void);

	/*
	 * These exist only with Xilka::GFX (no subclass overrides)
	 */
	void drawRoundRect(const Pos &p, const Dim &d
					, int16_t radius, uint16_t color);
	void fillRoundRect(const Pos &p, const Dim &d
					, int16_t radius, uint16_t color);
	void drawBitmap(const Pos &p, const Dim &d
					, const uint8_t *bitmap, uint16_t color);
	void drawBitmap(const Pos &p, const Dim &d
					, const uint8_t *bitmap, uint16_t color
					, uint16_t bgColor);
	void drawXBitmap(const Pos &p, const Dim &d
					, const uint8_t *bitmap, uint16_t color);

	virtual void drawChar(const Pos &pos, unsigned char c
					, uint16_t color, uint16_t bgColor
					, uint8_t size);

	void setTextColor(uint16_t c)
	{
		// For 'transparent' background, we'll set the bg
		// to the same as fg instead of using a flag
		textcolor = textbgcolor = c;
	}

	void setTextColor(uint16_t c, uint16_t b)
	{
		textcolor = c;
		textbgcolor = b;
	}

	void setTextSize(uint8_t s) { textsize = (s > 0) ? s : 1; }
	void setTextWrap(bool w) { _wrap = w; }

	void setRotation(uint8_t r);
	uint8_t getRotation(void) const { return rotation; }

	virtual size_t write(uint8_t);

	/*
	 * set/get current cursor position
	 * (get rotation safe maximum values,
	 * using: width() for x, height() for y)
	 */
	void setCursor(Pos pos) { _cursorPos = pos; }
	void setCursor(int16_t x, int16_t y) { _cursorPos.set(x, y); }
	const Pos &getCursor(void) const { return _cursorPos; }

	// Pass 8-bit (each) R,G,B, get back 16-bit packed color
	uint16_t getColor565(uint8_t r, uint8_t g, uint8_t b) const
	{
		return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
	}

protected:
	virtual void drawCircleHelperNT1(const Pos &center, int16_t radius, uint16_t color);
	virtual void drawCircleHelperNT2(const Pos &center, int16_t radius, uint16_t color);
	virtual void drawCircleHelperNT3(const Pos &center, int16_t radius, uint16_t color);
	virtual void drawCircleHelperNT4(const Pos &center, int16_t radius, uint16_t color);
	virtual void fillCircleHelperNT(const Pos &center, int16_t radius
			, uint8_t cornerName, int16_t delta, uint16_t color);
	/*
	 * void drawCharNT(const Pos &pos, unsigned char c
	 *		, uint16_t color, uint16_t bg, uint8_t size);
	 */

	Dim _rawDim;
	Dim _currentDim;
	Pos _cursorPos;

	uint16_t textcolor;
	uint16_t textbgcolor;
	uint8_t textsize;
	uint8_t rotation;
	bool _wrap; // If set, 'wrap' text at right edge of display
};

class GFX_Button
{
public:
	GFX_Button(void);
	void initButton(GFX *gfx, const GFX::Pos &p, const GFX::Dim &d
					, uint16_t outline, uint16_t fill
					, uint16_t textcolor, char *label
					, uint8_t textsize);
	void drawButton(bool inverted = false);
	bool contains(const GFX::Pos &p);

	void press(bool p)
	{
		laststate = currstate;
		currstate = p;
	}

	bool isPressed(void) { return currstate; }
	bool justPressed(void) { return (currstate && ! laststate); }
	bool justReleased(void) { return (!currstate && laststate); }


private:
	GFX *_gfx;
	GFX::Pos _pos;
	GFX::Dim _dim;
	uint8_t _textsize;
	uint16_t _outlinecolor;
	uint16_t _fillcolor;
	uint16_t _textcolor;
	char _label[10];

	bool currstate;
	bool laststate;
};

} // namespace Xilka

