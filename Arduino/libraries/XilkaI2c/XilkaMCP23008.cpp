#include <inttypes.h>
#include <Arduino.h>
#include <Wire.h>

#include "XilkaI2c.h"
#include "XilkaMCP23008.h"

#define MAX_PIN 7

namespace {

const uint8_t IODIR(0x00);
const uint8_t IPOL(0x01);
const uint8_t GPINTEN(0x02);
const uint8_t DEFVAL(0x03);
const uint8_t INTCON(0x04);
const uint8_t IOCON(0x05);
const uint8_t GPPU(0x06);
const uint8_t INTF(0x07);
const uint8_t INTCAP(0x08);
const uint8_t GPIO(0x09);
const uint8_t OLAT(0x0A);

} // namespace

namespace Xilka {

unsigned long MCP23008::MaxI2CSpeed(400000UL);

MCP23008::MCP23008()
{
}

MCP23008::MCP23008(uint8_t i2cAddr)
{
	postSetup(i2cAddr);
}

void MCP23008::postSetup(uint8_t i2cAddr)
{
	if ( i2cAddr >= 0x20 && i2cAddr <= 0x27 )
	{
		_i2cAddr = i2cAddr;
		_error = false;
	}
	else
	{
		_error = true;
	}

}

void MCP23008::begin(bool protocolInitOverride)
{
	if ( ! protocolInitOverride && ! _error )
	{
		Wire.begin();
		Wire.setClock(400000UL); // Set I2C frequency to 400kHz
	}

	delay(100);
	I2C::writeUInt8(_i2cAddr, IOCON, 0b00100000);

	_gpioDirection = 0xFF; // all in
	_gpioState = 0x00; // all low

	setPinsMode(INPUT);
}

void MCP23008::setPinsMode(uint8_t mask)
{
	_gpioDirection = ( mask == INPUT )
						? 0xFF
						: ( ( mask == OUTPUT )
							? 0x00
							: mask);

	if ( mask == OUTPUT )
		_gpioState = 0x00;

	I2C::writeUInt8(_i2cAddr, IODIR, _gpioDirection);
}

void MCP23008::setPinInput(uint8_t pin)
{
	if ( pin <= MAX_PIN )
	{
		_gpioDirection |= (1 << pin);
		I2C::writeUInt8(_i2cAddr, IODIR, _gpioDirection);
	}
}

void MCP23008::setPinOutput(uint8_t pin)
{
	if ( pin <= MAX_PIN )
	{
		_gpioDirection &= ~(1 << pin);
		I2C::writeUInt8(_i2cAddr, IODIR, _gpioDirection);
	}
}

void MCP23008::setPins(uint8_t value)
{
	_gpioState = ( value == HIGH )
					? 0xFF
					: ( ( value == LOW )
						? 0x00
						: value);

	I2C::writeUInt8(_i2cAddr, GPIO, _gpioState);
}

uint8_t MCP23008::getPins(void)
{
	I2C::readUInt8(_i2cAddr, GPIO, _gpioState);

	return _gpioState;
}

void MCP23008::setPullup(uint8_t mask)
{
	uint8_t b = ( mask == HIGH )
					? 0xFF
					: ( ( mask == LOW )
						? 0x00
						: mask);

	I2C::writeUInt8(_i2cAddr, GPPU, b);
}

void MCP23008::setPin(uint8_t pin, bool value)
{
	if ( pin <= MAX_PIN )
	{
		( value )
			? _gpioState |= (1 << pin)
			: _gpioState &= ~(1 << pin);

		I2C::writeUInt8(_i2cAddr, GPIO, _gpioState);
	}
}

bool MCP23008::getPin(uint8_t pin)
{
	if ( pin <= MAX_PIN )
	{
		uint8_t pins(0);

		if ( I2C::readUInt8(_i2cAddr, GPIO, pins) )
			return (pins & (1 << pin));
	}

	return false;
}

void MCP23008::updatePins(void)
{
	I2C::writeUInt8(_i2cAddr, GPIO, _gpioState);
}

} // namespace Xilka

