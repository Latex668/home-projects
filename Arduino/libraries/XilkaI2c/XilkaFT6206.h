#pragma once

#include "XilkaI2c.h"

namespace Xilka {

class TsPoint
{
public:
	TsPoint(void) : x(0), y(0), z(0) {}
	TsPoint(int16_t x, int16_t y, int16_t z) : x(x) , y(y), z(z) {}

	bool operator==(const TsPoint &o)
	{
		return  ((o.x == x) && (o.y == y) && (o.z == z));
	}

	bool operator!=(const TsPoint &o)
	{
		return  ((o.x != x) || (o.y != y) || (o.z != z));
	}

	int16_t x;
	int16_t y;
	int16_t z;
};

class FT6206
{
public:
	FT6206() {}
	bool begin(uint8_t thresh = 128);
	bool touched(void);
	TsPoint getPoint(void);

	static unsigned long MaxI2CSpeed;

private:
	void readData(uint16_t *x, uint16_t *y);

	uint8_t touches;
	uint16_t touchX[2];
	uint16_t touchY[2];
	uint16_t touchID[2];
};

} // namespace Xilka

