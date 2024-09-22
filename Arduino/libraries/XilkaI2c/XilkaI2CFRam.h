#pragma once

namespace Xilka {

class I2CFRam
{
public:
	I2CFRam();

	bool begin(uint8_t i2cAddr = 0x50); // 1010 + A2 + A1 + A0 = 0x50 default
	uint8_t read(uint16_t framAddr);
	void read(uint16_t framAddr, uint8_t *target, size_t count);
	void write(uint16_t framAddr, uint8_t value);
	void write(uint16_t framAddr, uint8_t *source, size_t count);
	void getDeviceId(uint16_t *manufacturerId, uint16_t *productId);

	static unsigned long MaxI2CSpeed;

private:
	void readStride(uint16_t framAddr, uint8_t *target, uint8_t count);
	void writeStride(uint16_t framAddr, uint8_t *source, uint8_t count);

	uint8_t _i2cAddr;
};

} // namespace Xilka

