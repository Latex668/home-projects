#include "XilkaSSD1306_128x32.h"
#include "XilkaSSD1306Commands.h"

namespace Xilka {

SSD1306_128x32::~SSD1306_128x32()
{
}

void SSD1306_128x32::initDisplay(bool useChargePump)
{
	uint8_t cmds[] =
	{
		CMD_DISPLAYOFF

		, CMD_SETDISPLAYCLOCKDIV
		, 0x80 // the suggested ratio 0x80

		, CMD_SETMULTIPLEX
		, 0x1F

		, CMD_SETDISPLAYOFFSET
		, 0x00 // no offset

		, CMD_SETSTARTLINE | 0x00 // line #0

		, CMD_CHARGEPUMP
		, 0x14

		, CMD_MEMORYMODE
		, 0x00 // act like ks0108

		, CMD_SEGREMAP | 0x01

		, CMD_COMSCANDEC

		, CMD_SETCOMPINS
		, 0x02

		, CMD_SETCONTRAST
		, 0x8F

		, CMD_SETPRECHARGE
		, 0xF1

		, CMD_SETVCOMDETECT
		, 0x40

		, CMD_DISPLAYALLON_RESUME

		, CMD_NORMALDISPLAY

		, CMD_DISPLAYON
	};

	if ( ! useChargePump )
	{
		cmds[9] = 0x10;
		cmds[19] = 0x22;
	}

	const uint8_t *cmdEnd(&cmds[0] + sizeof(cmds));

	for ( const uint8_t *cmd = &cmds[0]; cmd != cmdEnd; ++cmd )
		issueCommand(*cmd);
}

} // namespace Xilka

