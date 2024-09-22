#pragma once

#include "../XilkaBase/XilkaDigitalOutputPin.h"

namespace Xilka {

class MCP23008OutputPin : public DigitalOutputPin
{
public:
	MCP23008OutputPin(uint8_t pinId, bool state = false);
	virtual void setState(bool state);
};

} // namespace Xilka

