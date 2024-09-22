#include "../XilkaI2c.h"
#include "error.h"

namespace Xilka {
namespace I2C {

int getLastError(void)
{
	return lastI2CError;
}

const char *getLastErrorMessage(void)
{
	switch ( lastI2CError )
	{
	case 0: return "success";
	case 1: return "data too long for transmit buffer";
	case 2: return "received NACK on address (disconnected?)";
	case 3: return "received NACK on data";
	case 4: return "other error";
	case I2C_TIMEOUT: return "timeout";
	default: return "unknown error";
	}
}

} // namespace I2C
} // namespace Xilka

