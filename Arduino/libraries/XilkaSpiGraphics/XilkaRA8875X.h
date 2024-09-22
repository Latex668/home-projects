#pragma once
/*
 * Base class for the RA8875 family of chips
 */

#include <XilkaGFX.h>
#include <XilkaSpiGraphics.h>
#include "../XilkaSpi/XilkaSpi.h"

namespace Xilka {

class RA8875X : public SpiGraphics
{
public:

	void softReset(void);

protected:

	RA8875X(const GFX::Dim &dspDim
			, SPISettings spiSettings
			, int8_t resetPin
			, int8_t dcPin
			, int8_t csPin
			, SpiSoftWriter *spiSoft)
		: SpiGraphics(dspDim, spiSettings, resetPin
						, dcPin, csPin, spiSoft) {}

	virtual void drawPixelNT(const GFX::Pos &p, uint16_t color);

	virtual void drawFastVLineNT(const GFX::Pos &p
			, int16_t length, uint16_t color)
	{
		drawLineNT(p, GFX::Pos(p.x, p.y + length), color);
	}

	virtual void drawFastHLineNT(const GFX::Pos &p
			, int16_t length, uint16_t color)
	{
		drawLineNT(p, GFX::Pos(p.x + length, p.y), color);
	}

	virtual void drawLineNT(const GFX::Pos &p1
			, const GFX::Pos &p2, uint16_t color);

	virtual void drawRectNT(const GFX::Pos &p
			, const GFX::Dim &d, uint16_t color)
	{
		rectHelper(p, GFX::Pos(p.x + d.w, p.y + d.h), color, false);
	}

	virtual void fillRectNT(const GFX::Pos &p
			, const GFX::Dim &d, uint16_t color)
	{
		rectHelper(p, GFX::Pos(p.x + d.w, p.y + d.h), color, true);
	}

	virtual void fillScreenNT(uint16_t color)
	{
		const GFX::Pos p2(getCurrentDim().w - 1, getCurrentDim().h - 1);
		rectHelper(GFX::Pos(0, 0), p2, color, true);
	}

	/*
	 * Draw a unfilled/filled circle
	 */

	virtual void drawCircleNT(const GFX::Pos &center
			, int16_t radius, uint16_t color)
	{
		circleHelper(center, radius, color, false);
	}

	virtual void fillCircleNT(const GFX::Pos &center
			, int16_t radius, uint16_t color)
	{
		circleHelper(center, radius, color, true);
	}

	/*
	 * Draw a unfilled/filled triangle on the display
	 *
	 * p1, p2, p3 - specify the corners of the triangle
	 */

	virtual void drawTriangleNT(const GFX::Pos &p1
			, const GFX::Pos &p2
			, const GFX::Pos &p3
			, uint16_t color)
	{
		triangleHelper(p1, p2, p3, color, false);
	}

	virtual void fillTriangleNT(const GFX::Pos &p1
			, const GFX::Pos &p2
			, const GFX::Pos &p3
			, uint16_t color)
	{
		triangleHelper(p1, p2, p3, color, true);
	}


	/*
	 * Draw a unfilled/filled ellipse on the display
	 *
	 * center    - the ellipse's center
	 * longAxis  - The size in pixels of the ellipse's long axis
	 * shortAxis - The size in pixels of the ellipse's short axis
	 */

	void drawEllipse(const GFX::Pos &center
			, int16_t longAxis
			, int16_t shortAxis
			, uint16_t color)
	{
		ellipseHelper(center, longAxis, shortAxis, color, false);
	}

	void fillEllipse(const GFX::Pos &center
			, int16_t longAxis
			, int16_t shortAxis
			, uint16_t color)
	{
		ellipseHelper(center, longAxis, shortAxis, color, true);
	}

	/*
	 * Draw a unfilled/filled curve on the display
	 *
	 * center    - the curves's center
	 * longAxis  - The size in pixels of the ellipse's long axis
	 * shortAxis - The size in pixels of the ellipse's short axis
	 * curvePart - The corner to draw, where in clock-wise motion:
	 *                      0 = 180-270°
	 *                      1 = 270-0°
	 *                      2 = 0-90°
	 *                      3 = 90-180°
	 */

	void drawCurve(const GFX::Pos &center
		, int16_t longAxis
		, int16_t shortAxis
		, uint8_t curvePart
		, uint16_t color)
	{
		curveHelper(center, longAxis, shortAxis, curvePart, color, false);
	}

	void fillCurve(const GFX::Pos &center
		, int16_t longAxis
		, int16_t shortAxis
		, uint8_t curvePart
		, uint16_t color)
	{
		curveHelper(center, longAxis, shortAxis, curvePart, color, true);
	}

	/*
	 * Not exactly sure the versions match up, i.e. GFX::NT1->NT1
	 * directly to the functions in Xilka::GFX
	 * will have to test.
	 */
	virtual void drawCircleHelperNT1(const Pos &center
			, int16_t radius, uint16_t color)
	{
		curveHelper(center, radius, radius, 0, color, false);
	}

	virtual void drawCircleHelperNT2(const Pos &center
			, int16_t radius, uint16_t color)
	{
		curveHelper(center, radius, radius, 1, color, false);
	}

	virtual void drawCircleHelperNT3(const Pos &center
			, int16_t radius, uint16_t color)
	{
		curveHelper(center, radius, radius, 2, color, false);
	}

	virtual void drawCircleHelperNT4(const Pos &center
			, int16_t radius, uint16_t color)
	{
		curveHelper(center, radius, radius, 3, color, false);
	}

	/*
	 * have to test this function for correct behavior
	 */
	virtual void fillCircleHelperNT(const Pos &center
			, int16_t radius, uint8_t cornerName
			, uint16_t color)
	{
		if ( cornerName & 0x01 )
		{
			curveHelper(center, radius, radius, 0, color, true);
			curveHelper(center, radius, radius, 2, color, true);
		}

		if ( cornerName & 0x02 )
		{
			curveHelper(center, radius, radius, 1, color, true);
			curveHelper(center, radius, radius, 3, color, true);
		}
	}

	void displayOn(bool wantOn);
	void sleep(bool wantSleep);

private:

	void circleHelper(const GFX::Pos &center
		, int16_t radius
		, uint16_t color
		, bool wantFilled);

	void rectHelper(const GFX::Pos &p1
		, const GFX::Pos &p2
		, uint16_t color
		, bool wantFilled);

	void triangleHelper(const GFX::Pos &p1
		, const GFX::Pos &p2
		, const GFX::Pos &p3
		, uint16_t color
		, bool wantFilled);

	void ellipseHelper(const GFX::Pos &center
		, int16_t longAxis, int16_t shortAxis
		, uint16_t color
		, bool wantFilled);

	void curveHelper(const GFX::Pos &center
		, int16_t longAxis, int16_t shortAxis
		, uint8_t curvePart
		, uint16_t color
		, bool wantFilled);

	bool waitPoll(uint8_t regname, uint8_t waitflag);

	uint8_t readReg(uint8_t reg)
	{
		spiSetData(false);
		spiWrite(reg, true);
		return readData();
	}

	uint8_t readData(void)
	{
		_cs.setLow();

		SPI.transfer(0x40); // RA8875_DATAREAD
		uint8_t x(SPI.transfer(0x0));

		_cs.setHigh();
		return x;
	}
};

} // namespace Xilka

