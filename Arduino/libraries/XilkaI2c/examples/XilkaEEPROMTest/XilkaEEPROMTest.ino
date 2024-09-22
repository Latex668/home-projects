#include <Wire.h>
#include <XilkaEEPROM.h>

/*
 * Connect SCL    to analog 5 (also connect through 1K resistor to +V)
 * Connect SDA    to analog 4 (also connect through 1K resistor to +V)
 * Connect VDD    to 3.3V or 5.0V (the processor native V)
 * Connect GROUND to common ground
 */

Xilka::EEPROM storage;

void setup()
{
	Serial.begin(115200);

	Wire.begin();
	Wire.setClock(Xilka::EEPROM::MaxI2CSpeed);

	// you may use an alternative address, e.g. storage.begin(0x51);
	if ( storage.begin() ) // default address is 0x50
	{
		Serial.println("Found I2C EEPROM");
	}
	else
	{
		Serial.println("No I2C EEPROM found ... check your connections\r\n");
		while ( true )
			;
	}
}

void sequential256(int offset, bool up = true)
{
	Serial.print("sequential256 ");
	Serial.print(up ? "up ": "down ");
	Serial.print(offset, DEC);
	Serial.print("-");
	Serial.println(offset + 256, DEC);

	for ( int i = 0; i < 256; ++i )
		storage.write(i + offset, up ? i : 255 - i);

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
	storage.write(offset, out, sizeof(out));

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

