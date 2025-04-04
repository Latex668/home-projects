#include "XilkaILI934X.h"

namespace Xilka {

void ILI934X::setRotation(uint8_t m)
{
	const uint8_t CMD_MADCTL(0x36);
	const uint8_t MY(0x80);
	const uint8_t MX(0x40);
	const uint8_t MV(0x20);
	//const uint8_t ML(0x10);
	//const uint8_t RGB(0x00);
	const uint8_t BGR(0x08);
	//const uint8_t MH(0x04);


	spiBeginTran();

	spiSetData(false);
	spiWrite(CMD_MADCTL, false);
	rotation = m % 4; // can't be higher than 3

	spiSetData(true);

	switch ( rotation )
	{
	case 0:
		spiWrite(MX | BGR, true);
		_currentDim = _rawDim;
		break;
	case 1:
		spiWrite(MV | BGR, true);
		_currentDim.w = _rawDim.h;
		_currentDim.h = _rawDim.w;
		break;
	case 2:
		spiWrite(MY | BGR, true);
		_currentDim = _rawDim;
		break;
	case 3:
		spiWrite(MV | MY | MX | BGR, true);
		_currentDim.w = _rawDim.h;
		_currentDim.h = _rawDim.w;
		break;
	}

	spiEndTran();
}

} // namespace Xilka

