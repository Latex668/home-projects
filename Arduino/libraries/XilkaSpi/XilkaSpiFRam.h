#pragma once

#include "XilkaSpi.h"

namespace Xilka {

class SpiFRam
{
public:
	/*
	 * For Hardware SPI:
	 *     SpiFRam(csPin)
	 *
	 * For Software SPI:
	 *     SpiFRam(csPin, new SpiSoftReadWriter(sclkPin, mosiPin, misoPin));
	 */

	SpiFRam(int8_t csPin, SpiSoftReadWriter *spiSoft = 0);

	/*
	 * for hardware SPI - SPI.begin() should be called in setup()
	 */

	bool begin(void);
	void enableWrite(void) const { enableWrite(true); }
	void disableWrite(void) const { enableWrite(false); }

    void writeFloat(unsigned int address, float f) const
    {
        write(address, (const uint8_t*)&f, sizeof(f));
    }

    float readFloat(unsigned int address) const
    {
        float f;
        read(address, (uint8_t*)&f, sizeof(f));
        return f;
    }

	void write(uint16_t addr, uint8_t value) const;
	void write(uint16_t addr, const uint8_t *source, size_t count) const;
	uint8_t read(uint16_t addr) const;
	void read(uint16_t addr, uint8_t *target, size_t count) const;
	void getDeviceId(uint8_t &mId, uint16_t &pId) const;
	uint8_t getStatusRegister(void) const;
	void setStatusRegister(uint8_t value) const;

private:
	bool isHwSpi(void) const { return ! _spiSoft; }
	void enableWrite(bool enable) const;

	void spiBegin(void) const
	{
#if defined (SPI_HAS_TRANSACTION)
		if ( isHwSpi() )
			SPI.beginTransaction(_spiSettings);
#endif

		_cs.setLow();
	}

	void spiEnd(void) const
	{
		_cs.setHigh();

#if defined (SPI_HAS_TRANSACTION)
		if ( isHwSpi() )
			SPI.endTransaction();
#endif
	}

	//uint8_t spiXferSoft(uint8_t x) const;
	uint8_t spiXfer(uint8_t x) const
	{
		return isHwSpi()
				? SPI.transfer(x)
				: _spiSoft->readWrite(x);
	}

	SPISettings _spiSettings;
	PinPortOutput _cs;
	SpiSoftReadWriter *_spiSoft;
};

} // namespace Xilka

