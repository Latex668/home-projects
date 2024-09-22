#include "XilkaILI9340.h"

#if defined(ARDUINO_ARCH_AVR)
#include <avr/pgmspace.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <pgmspace.h>
#else
#define PROGMEM
#endif

namespace {

const uint8_t CMD_NOP(0x00);
const uint8_t CMD_SWRESET(0x01);
const uint8_t CMD_RDDID(0x04);
const uint8_t CMD_RDDST(0x09);

const uint8_t CMD_SLPIN(0x10);
const uint8_t CMD_SLPOUT(0x11);
const uint8_t CMD_PTLON(0x12);
const uint8_t CMD_NORON(0x13);

const uint8_t CMD_RDMODE(0x0A);
const uint8_t CMD_RDMADCTL(0x0B);
const uint8_t CMD_RDPIXFMT(0x0C);
const uint8_t CMD_RDIMGFMT(0x0D);
const uint8_t CMD_RDSELFDIAG(0x0F);

const uint8_t CMD_INVOFF(0x20);
const uint8_t CMD_INVON(0x21);
const uint8_t CMD_GAMMASET(0x26);
const uint8_t CMD_DISPOFF(0x28);
const uint8_t CMD_DISPON(0x29);

const uint8_t CMD_CASET(0x2A);
const uint8_t CMD_PASET(0x2B);
const uint8_t CMD_RAMWR(0x2C);
const uint8_t CMD_RAMRD(0x2E);

const uint8_t CMD_PTLAR(0x30);
const uint8_t CMD_MADCTL(0x36);

const uint8_t MADCTL_MY(0x80);
const uint8_t MADCTL_MX(0x40);
const uint8_t MADCTL_MV(0x20);
const uint8_t MADCTL_ML(0x10);
const uint8_t MADCTL_RGB(0x00);
const uint8_t MADCTL_BGR(0x08);
const uint8_t MADCTL_MH(0x04);

const uint8_t CMD_PIXFMT(0x3A);

const uint8_t CMD_FRMCTR1(0xB1);
const uint8_t CMD_FRMCTR2(0xB2);
const uint8_t CMD_FRMCTR3(0xB3);
const uint8_t CMD_INVCTR(0xB4);
const uint8_t CMD_DFUNCTR(0xB6);

const uint8_t CMD_PWCTR1(0xC0);
const uint8_t CMD_PWCTR2(0xC1);
const uint8_t CMD_PWCTR3(0xC2);
const uint8_t CMD_PWCTR4(0xC3);
const uint8_t CMD_PWCTR5(0xC4);
const uint8_t CMD_VMCTR1(0xC5);
const uint8_t CMD_VMCTR2(0xC7);

const uint8_t CMD_RDID1(0xDA);
const uint8_t CMD_RDID2(0xDB);
const uint8_t CMD_RDID3(0xDC);
const uint8_t CMD_RDID4(0xDD);

const uint8_t CMD_GMCTRP1(0xE0);
const uint8_t CMD_GMCTRN1(0xE1);
// const uint8_t CMD_PWCTR6(0xFC);

const uint8_t DELAY(0x80);

const uint8_t PROGMEM cmdList[] = {
	21 // commands in list

	// 1
	, 0xEF, 3
	, 0x03, 0x80, 0x02

	// 2
	, 0xCF, 3
	, 0x00, 0XC1, 0X30

	// 3
	, 0xED, 4
	, 0x64, 0x03, 0X12, 0X81

	// 4
	, 0xE8, 3
	, 0x85, 0x00, 0x78

	// 5
	, 0xCB, 5
	, 0x39, 0x2C, 0x00, 0x34
	, 0x02

	// 6
	, 0xF7, 1
	, 0x20

	// 7
	, 0xEA, 2
	, 0x00, 0x00

	// 8
	, CMD_PWCTR1, 1
	, 0x23 //VRH[5:0]

	// 9
	, CMD_PWCTR2, 1
	, 0x10 //SAP[2:0];BT[3:0]

	// 10
	, CMD_VMCTR1, 2
	, 0x3e, 0x28

	// 11
	, CMD_VMCTR2, 1
	, 0x86 //--

	// 12
	, CMD_MADCTL, 1
	, (MADCTL_MX | MADCTL_BGR)

	// 13
	, CMD_PIXFMT, 1
	, 0x55

	// 14
	, CMD_FRMCTR1, 2
	, 0x00, 0x18

	// 15
	, CMD_DFUNCTR, 3
	,  0x08, 0x82, 0x27

	// 16
	, 0xF2, 1 // 3Gamma Function Disable
	, 0x00

	// 17
	, CMD_GAMMASET, 1
	, 0x01

	// 18
	, CMD_GMCTRP1, 15 // Set Gamma
	, 0x0F, 0x31, 0x2B, 0x0C
	, 0x0E, 0x08, 0x4E, 0xF1
	, 0x37, 0x07, 0x10, 0x03
	, 0x0E, 0x09, 0x00

	// 19
	, CMD_GMCTRN1, 15 // Set Gamma
	, 0x00, 0x0E, 0x14, 0x03
	, 0x11, 0x07, 0x31, 0xC1
	, 0x48, 0x08, 0x0F, 0x0C
	, 0x31, 0x36, 0x0F

	// 20
	, CMD_SLPOUT, (0 | DELAY)
	, 120 // ms delay

	// 21
	, CMD_DISPON, 0
};

} // namespace

namespace Xilka {

ILI9340::ILI9340(int8_t resetPin
		, int8_t dcPin
		, int8_t csPin
		, SpiSoftWriter *spiSoft)
		/*
		 * 15.1 MHz determined from spec sheet page 231
		 * min SCL write cycle time of 66nS
		 */
	: ILI934X(Dim(240, 320)
				, SPISettings(15100000, MSBFIRST, SPI_MODE0)
				, resetPin, dcPin, csPin, spiSoft)
{
}

void ILI9340::begin(void)
{
	if ( _reset.getPin() != -1 )
	{
		_reset.setHigh();
		delay(5);
		_reset.setLow();
		delay(20);
		_reset.setHigh();
		delay(150);
	}

	issueRomCommands(cmdList);
}

} // namespace Xilka

