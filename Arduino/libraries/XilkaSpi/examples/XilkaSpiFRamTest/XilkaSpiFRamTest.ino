#include <SPI.h>
#include <XilkaSpiFRam.h>

using namespace Xilka;

#define WANT_SPI_HARD 1

#if defined(ARDUINO_ARCH_ESP8266)
const uint8_t FRAM_CS(15);
const uint8_t FRAM_SCK(14);
const uint8_t FRAM_MISO(12);
const uint8_t FRAM_MOSI(13);
#else
const uint8_t FRAM_CS(10);
const uint8_t FRAM_SCLK(13);
const uint8_t FRAM_MISO(12);
const uint8_t FRAM_MOSI(11);
#endif

#if defined(WANT_SPI_HARD)
SpiFRam storage(FRAM_CS);
#else
// Or use software SPI, any pins!
SpiSoftReadWriter spiSoft(FRAM_SCLK, FRAM_MOSI, FRAM_MISO);
SpiFRam storage(FRAM_CS, &spiSoft);
#endif

void setup(void)
{
	Serial.begin(115200);

#if defined(WANT_SPI_HARD)
	SPI.begin();
#endif

	if ( storage.begin() )
	{
		Serial.println("Found SPI FRAM");
	}
	else
	{
		Serial.println("No SPI FRAM found ... check your connections\r\n");
		while (1);
	}
}

void sequential256(int offset, bool up = true)
{
	Serial.print("sequential256 ");
	Serial.print(up ? "up ": "down ");
	Serial.print(offset, DEC);
	Serial.print("-");
	Serial.println(offset + 256, DEC);

	storage.enableWrite();

	for ( int i = 0; i < 256; ++i )
		storage.write(i + offset, up ? i : 255 - i);

	storage.disableWrite();

	for ( int i = 0; i < 256; ++i )
	{
		uint8_t d(storage.read(i + offset));

		if ( d != (up ? i : 255 - i) )
		{
			Serial.print("fail ");
			Serial.print(i+offset, DEC);
			Serial.print(" ");
			Serial.print(i, DEC);
			Serial.print("=");
			Serial.println(d, HEX);
		}
	}
}

void sequential4K(void)
{
/*
 * Test the first 4K of EEPROM
 * this test writes/reads 8K of data
 */
	Serial.println("sequential4K test 0-4096");

	for ( int i = 0; i < 4096; i += 256 )
	{
		sequential256(i, true);
		sequential256(i, false);
	}
}

void block256(int offset, bool up = true)
{
	Serial.print("block256 ");
	Serial.print(up ? "up ": "down ");
	Serial.print(offset, DEC);
	Serial.print("-");
	Serial.println(offset + 256, DEC);

	uint8_t out[256];
	uint8_t in[256];

	for ( int i = 0; i < 256; ++i )
		out[i] = up ? i : 255 - i;

	/*
	 * write the test block
	 */
	storage.enableWrite();

	storage.write(offset, out, sizeof(out));

	storage.disableWrite();

	/*
	 * read the test block
	 */
	storage.read(offset, in, sizeof(in));

	for ( int i = 0; i < 256; ++i )
	{
		if ( out[i] != in[i] )
		{
			Serial.print("fail ");
			Serial.print(i+offset, DEC);
			Serial.print(" ");
			Serial.println(out[i], HEX);
			Serial.print("!=");
			Serial.println(in[i], HEX);
		}
	}
}

void block4K(int offset = 0)
{
	Serial.print("block4K offset=");
	Serial.println(offset, DEC);

	for ( int i = 0; i < 4096; i += 256 )
	{
		block256(i+offset, true);
		block256(i+offset, false);
	}
}

void loop()
{
#if 0
	sequential4K();
	delay(2000);
#endif
	block4K();
	block4K(33);
	block4K(63);
	delay(5000);
}

