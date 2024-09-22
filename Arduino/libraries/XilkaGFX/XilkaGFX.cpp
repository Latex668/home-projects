/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef __AVR__
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#else
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif

#include "XilkaGFX.h"
#include "glcdfont.c"

#ifndef min
#define min(a,b) ((a < b) ? a : b)
#endif

#if defined(swap)
#undef swap
#endif

#if defined(ARDUINO_ARCH_ESP8266)
#define swap(a, b) { auto t(a); a = b; b = t; }
#else
#define swap(a, b) { typeof(a) t = a; a = b; b = t; }
#endif

namespace Xilka {

GFX::GFX(const GFX::Dim &dspDim)
	: Print()
	, _rawDim(dspDim)
	, _currentDim(dspDim)
	, _cursorPos()
	, textcolor(0xFFFF)
	, textbgcolor(0xFFFF)
	, textsize(1)
	, rotation(0)
	, _wrap(true)
{
}

// functions that default to doing nothing
void GFX::spiBeginTran(void) {}
void GFX::spiEndTran(void) {}
void GFX::invertDisplay(bool i) {}

void GFX::drawCircleNT(const GFX::Pos &p, int16_t r, uint16_t color)
{
	drawPixelNT(Pos(p.x, p.y + r), color);
	drawPixelNT(Pos(p.x, p.y - r), color);
	drawPixelNT(Pos(p.x + r, p.y), color);
	drawPixelNT(Pos(p.x - r, p.y), color);

	int16_t f(1 - r);
	int16_t ddF_x(1);
	int16_t ddF_y(-2 * r);
	int16_t y(r);

	for ( int16_t x = 0; x < y; )
	{
		if ( f >= 0 )
		{
			--y;
			ddF_y += 2;
			f += ddF_y;
		}

		++x;
		ddF_x += 2;
		f += ddF_x;

		drawPixelNT(Pos(p.x + x, p.y + y), color);
		drawPixelNT(Pos(p.x - x, p.y + y), color);
		drawPixelNT(Pos(p.x + x, p.y - y), color);
		drawPixelNT(Pos(p.x - x, p.y - y), color);
		drawPixelNT(Pos(p.x + y, p.y + x), color);
		drawPixelNT(Pos(p.x - y, p.y + x), color);
		drawPixelNT(Pos(p.x + y, p.y - x), color);
		drawPixelNT(Pos(p.x - y, p.y - x), color);
	}
}

void GFX::fillCircleNT(const Pos &p, int16_t r, uint16_t color)
{
	drawFastVLineNT(Pos(p.x, p.y - r), 2 * r + 1, color);
	fillCircleHelperNT(p, r, 3, 0, color);
}

void GFX::fillRoundRect(const Pos &p, const Dim &d
		, int16_t r, uint16_t color)
{
	spiBeginTran();

	const int16_t r2(r*2);

	// smarter version
	fillRectNT(Pos(p.x + r, p.y), Dim(d.w - r2, d.h), color);

	// draw four corners
	fillCircleHelperNT(Pos(p.x + d.w - r - 1, p.y + r), r, 1, d.h - r2 - 1, color);
	fillCircleHelperNT(Pos(p.x + r, p.y + r), r, 2, d.h - r2 - 1, color);

	spiEndTran();
}

// Used to do circles and roundrects
void GFX::fillCircleHelperNT(const Pos &p
	, int16_t r, uint8_t cornername
	, int16_t delta, uint16_t color)
{
	int16_t f(1 - r);
	int16_t ddF_x(1);
	int16_t ddF_y(-2 * r);
	int16_t x(0);
	int16_t y(r);
	int16_t ylm(p.x -r);

	while ( x < y )
	{
		if ( f >= 0 )
		{
			const int16_t lineWidth(2 * x + 1 + delta);
			const int16_t pYminusX(p.y - x);

			if ( cornername & 0x1 )
				drawFastVLineNT(Pos(p.x + y, pYminusX), lineWidth, color);

			if ( cornername & 0x2 )
				drawFastVLineNT(Pos(p.x - y, pYminusX), lineWidth, color);

			ylm = p.x - y;
			--y;
			ddF_y += 2;
			f += ddF_y;
		}

		++x;
		ddF_x += 2;
		f += ddF_x;

		if ( (p.x - x) > ylm )
		{
			const int16_t lineWidth(2 * y + 1 + delta);
			const int16_t pYminusY(p.y - y);

			if ( cornername & 0x1 )
				drawFastVLineNT(Pos(p.x + x, pYminusY), lineWidth, color);

			if ( cornername & 0x2 )
				drawFastVLineNT(Pos(p.x - x, pYminusY), lineWidth, color);
		}
	}
}

// Bresenham's algorithm - thx wikpedia
void GFX::drawLineNT(const Pos &p1In, const Pos &p2In, uint16_t color)
{
	Pos p1(p1In);
	Pos p2(p2In);

	if ( p1.y == p2.y )
	{
		if ( p2.x > p1.x )
			drawFastHLineNT(Pos(p1.x, p1.y), p2.x - p1.x + 1, color);
		else if ( p2.x < p1.x )
			drawFastHLineNT(Pos(p2.x, p1.y), p1.x - p2.x + 1, color);
		else
			drawPixelNT(p1, color);

		return;
	}
	else if ( p1.x == p2.x )
	{
		( p2.y > p1.y )
			? drawFastVLineNT(Pos(p1.x, p1.y), p2.y - p1.y + 1, color)
			: drawFastVLineNT(Pos(p1.x, p2.y), p1.y - p2.y + 1, color);

		return;
	}

	bool steep(abs(p2.y - p1.y) > abs(p2.x - p1.x));

	if ( steep )
	{
		swap(p1.x, p1.y);
		swap(p2.x, p2.y);
	}

	if ( p1.x > p2.x )
	{
		swap(p1.x, p2.x);
		swap(p1.y, p2.y);
	}

	int16_t dx(p2.x - p1.x);
	int16_t dy(abs(p2.y - p1.y));

	int16_t err(dx >> 1);
	int16_t ystep((p1.y < p2.y) ? 1 : -1);

	int16_t xbegin(p1.x);

	//fillScanline16(color);

	if ( steep )
	{
		for ( ; p1.x <= p2.x; ++p1.x )
		{
			err -= dy;

			if ( err < 0 )
			{
				int16_t len(p1.x - xbegin);

				( len )
					? drawFastVLineNT(Pos(p1.y, xbegin), len + 1, color)
					: drawPixelNT(Pos(p1.y, p1.x), color);

				xbegin = p1.x + 1;
				p1.y += ystep;
				err += dx;
			}
		}

		if ( p1.x > xbegin + 1 )
			drawFastVLineNT(Pos(p1.y, xbegin), p1.x - xbegin, color);

	}
	else
	{
		for ( ; p1.x <= p2.x; ++p1.x )
		{
			err -= dy;

			if ( err < 0 )
			{
				int16_t len(p1.x - xbegin);

				( len )
					? drawFastHLineNT(Pos(xbegin, p1.y), len + 1, color)
					: drawPixelNT(p1, color);

				xbegin = p1.x + 1;
				p1.y += ystep;
				err += dx;
			}
		}

		if ( p1.x > xbegin + 1 )
			drawFastHLineNT(Pos(xbegin, p1.y), p1.x - xbegin, color);
	}
}

void GFX::fillScreenNT(uint16_t color)
{
	fillRectNT(Pos(0, 0), _currentDim, color);
}

void GFX::drawRectNT(const Pos &p, const Dim &d, uint16_t color)
{
	drawFastHLineNT(p, d.w, color);
	drawFastHLineNT(Pos(p.x, p.y + d.h - 1), d.w, color);
	drawFastVLineNT(p, d.h, color);
	drawFastVLineNT(Pos(p.x + d.w - 1, p.y), d.h, color);
}

void GFX::fillRectNT(const Pos &p, const Dim &d, uint16_t color)
{
/*
 * You probably want to override this in
 * the subclass with an optimized version
 */
	const int16_t iEnd(p.x + d.w);

	for ( int16_t i = p.x; i < iEnd; ++i )
		drawFastVLineNT(Pos(i, p.y), d.h, color);
}

// Draw a rounded rectangle
void GFX::drawRoundRect(const Pos &p, const Dim &d, int16_t r, uint16_t color)
{
	spiBeginTran();

	const int16_t r2(r*2);

	// smarter version
	drawFastHLineNT(Pos(p.x + r, p.y), d.w - r2, color); // Top
	drawFastHLineNT(Pos(p.x + r, p.y + d.h - 1), d.w - r2, color); // Bottom
	drawFastVLineNT(Pos(p.x, p.y + r), d.h - r2, color); // Left
	drawFastVLineNT(Pos(p.x + d.w - 1, p.y + r), d.h - r2, color); // Right

	// draw four corners
	drawCircleHelperNT1(Pos(p.x + r, p.y + r), r, color);
	drawCircleHelperNT2(Pos(p.x + d.w - r - 1, p.y + r), r, color);
	drawCircleHelperNT3(Pos(p.x + d.w - r - 1, p.y + d.h - r - 1), r, color);
	drawCircleHelperNT4(Pos(p.x + r, p.y + d.h - r - 1), r, color);

	spiEndTran();
}

void GFX::drawCircleHelperNT1(const Pos &p, int16_t r, uint16_t color)
{
	int16_t f(1 - r);
	int16_t ddF_x(1);
	int16_t ddF_y(-2 * r);
	int16_t y(r);

	for ( int16_t x = 0; x < y; )
	{
		if ( f >= 0 )
		{
			--y;
			ddF_y += 2;
			f += ddF_y;
		}

		++x;
		ddF_x += 2;
		f += ddF_x;

		drawPixelNT(Pos(p.x - y, p.y - x), color);
		drawPixelNT(Pos(p.x - x, p.y - y), color);
	}
}
void GFX::drawCircleHelperNT2(const Pos &p, int16_t r, uint16_t color)
{
	int16_t f(1 - r);
	int16_t ddF_x(1);
	int16_t ddF_y(-2 * r);
	int16_t y(r);

	for ( int16_t x = 0; x < y; )
	{
		if ( f >= 0 )
		{
			--y;
			ddF_y += 2;
			f += ddF_y;
		}

		++x;
		ddF_x += 2;
		f += ddF_x;

		drawPixelNT(Pos(p.x + x, p.y - y), color);
		drawPixelNT(Pos(p.x + y, p.y - x), color);
	}
}

void GFX::drawCircleHelperNT3(const Pos &p, int16_t r, uint16_t color)
{
	int16_t f(1 - r);
	int16_t ddF_x(1);
	int16_t ddF_y(-2 * r);
	int16_t y(r);

	for ( int16_t x = 0; x < y; )
	{
		if ( f >= 0 )
		{
			--y;
			ddF_y += 2;
			f += ddF_y;
		}

		++x;
		ddF_x += 2;
		f += ddF_x;

		drawPixelNT(Pos(p.x + x, p.y + y), color);
		drawPixelNT(Pos(p.x + y, p.y + x), color);
	}
}

void GFX::drawCircleHelperNT4(const Pos &p, int16_t r, uint16_t color)
{
	int16_t f(1 - r);
	int16_t ddF_x(1);
	int16_t ddF_y(-2 * r);
	int16_t y(r);

	for ( int16_t x = 0; x < y; )
	{
		if ( f >= 0 )
		{
			--y;
			ddF_y += 2;
			f += ddF_y;
		}

		++x;
		ddF_x += 2;
		f += ddF_x;

		drawPixelNT(Pos(p.x - y, p.y + x), color);
		drawPixelNT(Pos(p.x - x, p.y + y), color);
	}
}

void GFX::drawTriangleNT(const Pos &p1
		, const Pos &p2, const Pos &p3
		, uint16_t color)
{
	drawLineNT(p1, p2, color);
	drawLineNT(p2, p3, color);
	drawLineNT(p3, p1, color);
}

void GFX::fillTriangleNT(const Pos &p1In
			, const Pos &p2In, const Pos &p3In
			, uint16_t color)
{
	Pos p1(p1In);
	Pos p2(p2In);
	Pos p3(p3In);

	// Sort coordinates by Y order (p3.y >= p2.y >= p1.y)
	if ( p1.y > p2.y )
	{
		swap(p1.y, p2.y);
		swap(p1.x, p2.x);
	}

	if ( p2.y > p3.y )
	{
		swap(p3.y, p2.y);
		swap(p3.x, p2.x);
	}

	if ( p1.y > p2.y )
	{
		swap(p1.y, p2.y);
		swap(p1.x, p2.x);
	}

	if ( p1.y == p3.y ) // Handle awkward all-on-same-line case as its own thing
	{
		int16_t a(p1.x);
		int16_t b(p1.x);

		if ( p2.x < a )
			a = p2.x;
		else if ( p2.x > b )
			b = p2.x;

		if ( p3.x < a )
			a = p3.x;
		else if ( p3.x > b )
			b = p3.x;

		drawFastHLine(Pos(a, p1.y), b - a + 1, color);
		return;
	}

	int16_t dx01(p2.x - p1.x);
	int16_t dy01(p2.y - p1.y);
	int16_t dx02(p3.x - p1.x);
	int16_t dy02(p3.y - p1.y);
	int16_t dx12(p3.x - p2.x);
	int16_t dy12(p3.y - p2.y);

	/*
	 * For upper part of triangle, find scanline crossings for segments
	 * 0-1 and 0-2.  If p2.y=p3.y (flat-bottomed triangle), the scanline p2.y
	 * is included here (and second loop will be skipped, avoiding a /0
	 * error there), otherwise scanline p2.y is skipped here and handled
	 * in the second loop...which also avoids a /0 error here if p1.y=p2.y
	 * (flat-topped triangle).
	 */
	const int16_t last( ( p2.y == p3.y )
					? p2.y // Include p2.y scanline
					: p2.y - 1); // Skip it

	int16_t y;
	int32_t sa(0);
	int32_t sb(0);

	for( y = p1.y; y <= last; ++y )
	{
		int16_t a = p1.x + sa / dy01;
		int16_t b = p1.x + sb / dy02;
		sa += dx01;
		sb += dx02;
		/* longhand:
		 * a = p1.x + (p2.x - p1.x) * (y - p1.y) / (p2.y - p1.y);
		 * b = p1.x + (p3.x - p1.x) * (y - p1.y) / (p3.y - p1.y);
		 */
		if ( a > b )
			swap(a,b);

		drawFastHLineNT(Pos(a, y), b - a + 1, color);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.  This loop is skipped if p2.y=p3.y.
	sa = dx12 * (y - p2.y);
	sb = dx02 * (y - p1.y);

	for( ; y <= p3.y; ++y )
	{
		int16_t a = p2.x + sa / dy12;
		int16_t b = p1.x + sb / dy02;
		sa += dx12;
		sb += dx02;
		/* longhand:
		 * a = p2.x + (p3.x - p2.x) * (y - p2.y) / (p3.y - p2.y);
		 * b = p1.x + (p3.x - p1.x) * (y - p1.y) / (p3.y - p1.y);
		 */
		if ( a > b )
			swap(a,b);

		drawFastHLineNT(Pos(a, y), b - a + 1, color);
	}
}

void GFX::drawBitmap(const Pos &p, const Dim &d
		, const uint8_t *bitmap, uint16_t color)
{
	spiBeginTran();

	const int16_t byteWidth((d.w + 7) / 8);

	for ( int16_t j = 0; j < d.h; ++j )
		for ( int16_t i = 0; i < d.w; ++i )
			if ( pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7)) )
				drawPixelNT(Pos(p.x + i, p.y + j), color);

	spiEndTran();
}

/*
 * Draw a 1-bit color bitmap at the specified x, y position from the
 * provided bitmap buffer (must be PROGMEM memory) using color as the
 * foreground color and bg as the background color.
 */
void GFX::drawBitmap(const Pos &p, const Dim &d
		, const uint8_t *bitmap, uint16_t color
		, uint16_t bg)
{
	spiBeginTran();

	const int16_t byteWidth((d.w + 7) / 8);

	for( int16_t j = 0; j < d.h; ++j )
		for( int16_t i = 0; i < d.w; ++i )
			drawPixelNT(Pos(p.x + i, p.y + j)
				, ( pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7)) )
					? color
					: bg);

	spiEndTran();
}

/*
 * Draw XBitMap Files (*.xbm), exported from GIMP,
 * Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
 * C Array can be directly used with this function
 */
void GFX::drawXBitmap(const Pos &p, const Dim &d
		, const uint8_t *bitmap, uint16_t color)
{
	spiBeginTran();

	const int16_t byteWidth((d.w + 7) / 8);

	for ( int16_t j = 0; j < d.h; ++j )
		for ( int16_t i = 0; i < d.w; ++i )
			if ( pgm_read_byte(bitmap + j * byteWidth + i / 8) & (1 << (i % 8)) )
				drawPixelNT(Pos(p.x + i, p.y + j), color);

	spiEndTran();
}

size_t GFX::write(uint8_t c)
{
	if ( c != '\n' )
	{
		if ( c != '\r' ) // skip returns
		{
			drawChar(_cursorPos, c, textcolor, textbgcolor, textsize);
			_cursorPos.x += textsize * 6;

			if ( _wrap && (_cursorPos.x > (_currentDim.w - textsize*6)) )
			{
				_cursorPos.y += textsize * 8;
				_cursorPos.x = 0;
			}
		}
	}
	else
	{
		_cursorPos.y += textsize * 8;
		_cursorPos.x = 0;
	}

	return 1;
}

// Draw a character
void GFX::drawChar(const Pos &pIn
		, unsigned char c, uint16_t color
		, uint16_t bg, uint8_t size)
{
	if ( (pIn.x < _currentDim.w) // Clip right
			&& (pIn.y < _currentDim.h) // Clip bottom
			&& ((pIn.x + 6 * size - 1) >= 0) // Clip left
			&& ((pIn.y + 8 * size - 1) >= 0) ) // Clip top
	{
		Pos p(pIn);

		spiBeginTran();

		for ( int8_t i = 0; i < 6; ++i, p.x += size )
		{
			uint8_t line(( i == 5 )
							? 0x0
							: pgm_read_byte(font + (c*5) + i));

			bool Fill(line & 0x1);
			uint8_t SF(0);

			for ( int8_t j = 1, jy = size; j < 9; ++j, jy += size )
			{
				line >>= 1;

				if ( (Fill == (line & 0x1)) && j != 8 )
					continue;

				if ( Fill )
					fillRect(Pos(p.x, p.y + SF), Dim(size, jy - SF), color);
				else if ( bg != color )
					fillRect(Pos(p.x, p.y + SF), Dim(size, jy - SF), bg);

				Fill = line & 0x1;
				SF = jy;
			}
		}

		spiEndTran();
	}
}

void GFX::setRotation(uint8_t x)
{
	rotation = (x & 3);

	switch ( rotation )
	{
	case 0:
	case 2:
		_currentDim = _rawDim;
		break;
	case 1:
	case 3:
		_currentDim.w = _rawDim.h;
		_currentDim.h = _rawDim.w;
		break;
	}
}

/*
 * code for the GFX button UI element
 */

GFX_Button::GFX_Button(void)
	: _gfx(0)
{
}

void GFX_Button::initButton(GFX *gfx
	, const GFX::Pos &p
	, const GFX::Dim &d
	, uint16_t outline, uint16_t fill
	, uint16_t textcolor
	, char *label, uint8_t textsize)
{
	_pos = p;
	_dim = d;
	_outlinecolor = outline;
	_fillcolor = fill;
	_textcolor = textcolor;
	_textsize = textsize;
	_gfx = gfx;
	strncpy(_label, label, 9);
	_label[9] = 0;
}

void GFX_Button::drawButton(bool inverted)
{
	uint16_t fill;
	uint16_t outline;
	uint16_t text;

	if ( ! inverted )
	{
		fill = _fillcolor;
		outline = _outlinecolor;
		text = _textcolor;
	}
	else
	{
		fill =  _textcolor;
		outline = _outlinecolor;
		text = _fillcolor;
	}

	const GFX::Pos p(_pos.x - (_dim.w/2), _pos.y - (_dim.h/2));
	int16_t min4(min(_dim.w, _dim.h)/4);

	_gfx->fillRoundRect(p, _dim, min4, fill);
	_gfx->drawRoundRect(p, _dim, min4, outline);

	_gfx->setCursor(_pos.x - strlen(_label) * 3 * _textsize, _pos.y - 4 * _textsize);
	_gfx->setTextColor(text);
	_gfx->setTextSize(_textsize);
	_gfx->print(_label);
}

bool GFX_Button::contains(const GFX::Pos &p)
{
	return
		(p.x >= (_pos.x - _dim.w/2))
			&& (p.x <= (_pos.x + _dim.w/2))
			&& (p.y >= (_pos.y - _dim.h/2))
			&& (p.y <= (_pos.y + _dim.h/2));
}

} // namespace Xilka

