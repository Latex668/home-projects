#include <Arduino.h>
#include <Wire.h>

#include "XilkaI2c.h"
#include "XilkaINA219.h"

namespace {

const uint8_t REG_CONFIG(0x00);       // R/W
const uint8_t REG_SHUNTVOLTAGE(0x01); // R
const uint8_t REG_BUSVOLTAGE(0x02);   // R
const uint8_t REG_POWER(0x03);        // R
const uint8_t REG_CURRENT(0x04);      // R
const uint8_t REG_CALIBRATION(0x05);  // R/W

const uint16_t CFG_RESET(0x8000);  // Reset Bit
const uint16_t CFG_BVOLTAGERANGE_MASK(0x2000);  // Bus Voltage Range Mask
const uint16_t CFG_BVOLTAGERANGE_16V(0x0000);  // 0-16V Range
const uint16_t CFG_BVOLTAGERANGE_32V(0x2000);  // 0-32V Range

const uint16_t CFG_GAIN_MASK(0x1800);  // Gain Mask
const uint16_t CFG_GAIN_1_40MV(0x0000);  // Gain 1, 40mV Range
const uint16_t CFG_GAIN_2_80MV(0x0800);  // Gain 2, 80mV Range
const uint16_t CFG_GAIN_4_160MV(0x1000);  // Gain 4, 160mV Range
const uint16_t CFG_GAIN_8_320MV(0x1800);  // Gain 8, 320mV Range

const uint16_t CFG_BADCRES_MASK(0x0780);  // Bus ADC Resolution Mask
const uint16_t CFG_BADCRES_9BIT(0x0080);  // 9-bit bus res = 0..511
const uint16_t CFG_BADCRES_10BIT(0x0100);  // 10-bit bus res = 0..1023
const uint16_t CFG_BADCRES_11BIT(0x0200);  // 11-bit bus res = 0..2047
const uint16_t CFG_BADCRES_12BIT(0x0400);  // 12-bit bus res = 0..4097

const uint16_t CFG_SADCRES_MASK(0x0078);  // Shunt ADC Resolution and Averaging Mask
const uint16_t CFG_SADCRES_9BIT_1S_84US(0x0000);  // 1 x 9-bit shunt sample
const uint16_t CFG_SADCRES_10BIT_1S_148US(0x0008);  // 1 x 10-bit shunt sample
const uint16_t CFG_SADCRES_11BIT_1S_276US(0x0010);  // 1 x 11-bit shunt sample
const uint16_t CFG_SADCRES_12BIT_1S_532US(0x0018);  // 1 x 12-bit shunt sample
const uint16_t CFG_SADCRES_12BIT_2S_1060US(0x0048);  // 2 x 12-bit shunt samples averaged together
const uint16_t CFG_SADCRES_12BIT_4S_2130US(0x0050);  // 4 x 12-bit shunt samples averaged together
const uint16_t CFG_SADCRES_12BIT_8S_4260US(0x0058);  // 8 x 12-bit shunt samples averaged together
const uint16_t CFG_SADCRES_12BIT_16S_8510US(0x0060);  // 16 x 12-bit shunt samples averaged together
const uint16_t CFG_SADCRES_12BIT_32S_17MS(0x0068);  // 32 x 12-bit shunt samples averaged together
const uint16_t CFG_SADCRES_12BIT_64S_34MS(0x0070);  // 64 x 12-bit shunt samples averaged together
const uint16_t CFG_SADCRES_12BIT_128S_69MS(0x0078);  // 128 x 12-bit shunt samples averaged together

const uint16_t CFG_MODE_MASK(0x0007);  // Operating Mode Mask
const uint16_t CFG_MODE_POWERDOWN(0x0000);
const uint16_t CFG_MODE_SVOLT_TRIGGERED(0x0001);
const uint16_t CFG_MODE_BVOLT_TRIGGERED(0x0002);
const uint16_t CFG_MODE_SANDBVOLT_TRIGGERED(0x0003);
const uint16_t CFG_MODE_ADCOFF(0x0004);
const uint16_t CFG_MODE_SVOLT_CONTINUOUS(0x0005);
const uint16_t CFG_MODE_BVOLT_CONTINUOUS(0x0006);
const uint16_t CFG_MODE_SANDBVOLT_CONTINUOUS(0x0007);

uint16_t getAveraging(int sampleCount)
{
	switch ( sampleCount )
	{
	case 1: return CFG_SADCRES_12BIT_1S_532US; break;
	default: case 2: return CFG_SADCRES_12BIT_2S_1060US; break;
	case 4: return CFG_SADCRES_12BIT_4S_2130US; break;
	case 8: return CFG_SADCRES_12BIT_8S_4260US; break;
	case 16: return CFG_SADCRES_12BIT_16S_8510US; break;
	case 32: return CFG_SADCRES_12BIT_32S_17MS; break;
	case 64: return CFG_SADCRES_12BIT_64S_34MS; break;
	case 128: return CFG_SADCRES_12BIT_128S_69MS; break;
	}
}

} // namespace


namespace Xilka {

unsigned long INA219::MaxI2CSpeed(3400000UL);

void INA219::begin(uint8_t i2cAddr)
{
	_i2cAddr = i2cAddr;
	setRange32V2A();
}

bool INA219::getRawBusVoltage(int16_t &result) const
{
	/*
	 * Shift to the right 3 to drop CNVR and OVF
	 * and multiply by LSB
	 */
	uint16_t tmp(0);

	if ( I2C::readUInt16(_i2cAddr, REG_BUSVOLTAGE, tmp) )
	{
		result = (int16_t)((tmp >> 3) * 4);
		return true;
	}

	return false;
}

bool INA219::getRawShuntVoltage(int16_t &result) const
{
	uint16_t tmp(0);

	if ( I2C::readUInt16(_i2cAddr, REG_SHUNTVOLTAGE, tmp) )
	{
		result = (int16_t)tmp;
		return true;
	}

	return false;
}

bool INA219::getRawCurrent(int16_t &result) const
{
	/*
	 * Sometimes a sharp load will reset the INA219, which will
	 * reset the cal register, meaning CURRENT and POWER will
	 * not be available ... avoid this by always setting a cal
	 * value even if it's an unfortunate extra step
	 */
	I2C::writeUInt16(_i2cAddr, REG_CALIBRATION, _calValue);

	uint16_t tmp(0);

	if ( I2C::readUInt16(_i2cAddr, REG_CURRENT, tmp) )
	{
		result = (int16_t)tmp;
		return true;
	}

	return false;
}

/**************************************************************************/
/*!
    @brief  Configures to INA219 to be able to measure up to 32V and 2A
            of current.  Each unit of current corresponds to 100uA, and
            each unit of power corresponds to 2mW. Counter overflow
            occurs at 3.2A.

    @note   These calculations assume a 0.1 ohm resistor is present
*/
/**************************************************************************/
bool INA219::setRange32V2A(int sampleCount)
{
	/*
	 * By default we use a pretty huge range for the input voltage,
     * which probably isn't the most appropriate choice for system
	 * that don't use a lot of power.  But all of the calculations
	 * are shown below if you want to change the settings.  You will
	 * also need to change any relevant register settings, such as
	 * setting the VBUS_MAX to 16V instead of 32V, etc.

	 * VBUS_MAX = 32V    (Assumes 32V, can also be set to 16V)
	 * VSHUNT_MAX = 0.32 (Assumes Gain 8, 320mV, can also be 0.16, 0.08, 0.04)
	 * RSHUNT = 0.1      (Resistor value in ohms)

	 * 1. Determine max possible current
	 * MaxPossible_I = VSHUNT_MAX / RSHUNT
	 * MaxPossible_I = 3.2A

	 * 2. Determine max expected current
	 * MaxExpected_I = 2.0A

	 * 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
	 * MinimumLSB = MaxExpected_I/32767
	 * MinimumLSB = 0.000061              (61uA per bit)
	 * MaximumLSB = MaxExpected_I/4096
	 * MaximumLSB = 0,000488              (488uA per bit)

	 * 4. Choose an LSB between the min and max values
	 *    (Preferrably a roundish number close to MinLSB)
	 * CurrentLSB = 0.0001 (100uA per bit)

	 * 5. Compute the calibration register
	 * Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
	 * Cal = 4096 (0x1000)
	 */

	_calValue = 4096;

	/*
	 * 6. Calculate the power LSB
	 * PowerLSB = 20 * CurrentLSB
	 * PowerLSB = 0.002 (2mW per bit)

	 * 7. Compute the maximum current and shunt voltage values before overflow

	 * Max_Current = Current_LSB * 32767
	 * Max_Current = 3.2767A before overflow

	 * If Max_Current > Max_Possible_I then
	 *    Max_Current_Before_Overflow = MaxPossible_I
	 * Else
	 *    Max_Current_Before_Overflow = Max_Current
	 * End If

	 * Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
	 * Max_ShuntVoltage = 0.32V

	 * If Max_ShuntVoltage >= VSHUNT_MAX
	 *    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
	 * Else
	 *    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
	 * End If

	 * 8. Compute the Maximum Power
	 * MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
	 * MaximumPower = 3.2 * 32V
	 * MaximumPower = 102.4W
	 */

	// Set multipliers to convert raw current/power values
	_currentDividerMilliAmp = 10.0; // Current LSB = 100uA per bit (1000/100 = 10)
	// _powerDividerMilliWatt = 2; // Power LSB = 1mW per bit (2/1)

	// Set Calibration register to 'Cal' calculated above
	if ( I2C::writeUInt16(_i2cAddr, REG_CALIBRATION, _calValue) )
	{
		// Set Config register to take into account the settings above
		_config = CFG_BVOLTAGERANGE_32V
					| CFG_GAIN_8_320MV
					| CFG_BADCRES_12BIT
					| getAveraging(sampleCount)
					| CFG_MODE_SANDBVOLT_CONTINUOUS;

		return I2C::writeUInt16(_i2cAddr, REG_CONFIG, _config);
	}

	return false;
}

/**************************************************************************/
/*!
    @brief  Configures to INA219 to be able to measure up to 32V and 1A
            of current.  Each unit of current corresponds to 40uA, and each
            unit of power corresponds to 800uW. Counter overflow occurs at
            1.3A.

    @note   These calculations assume a 0.1 ohm resistor is present
*/
/**************************************************************************/
bool INA219::setRange32V1A(int sampleCount)
{
	/*
	 * By default we use a pretty huge range for the input voltage,
	 * which probably isn't the most appropriate choice for system
	 * that don't use a lot of power.  But all of the calculations
	 * are shown below if you want to change the settings.  You will
	 * also need to change any relevant register settings, such as
	 * setting the VBUS_MAX to 16V instead of 32V, etc.

	 * VBUS_MAX = 32V    (Assumes 32V, can also be set to 16V)
	 * VSHUNT_MAX = 0.32 (Assumes Gain 8, 320mV, can also be 0.16, 0.08, 0.04)
	 * RSHUNT = 0.1      (Resistor value in ohms)

	 * 1. Determine max possible current
	 * MaxPossible_I = VSHUNT_MAX / RSHUNT
	 * MaxPossible_I = 3.2A

	 * 2. Determine max expected current
	 * MaxExpected_I = 1.0A

	 * 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
	 * MinimumLSB = MaxExpected_I/32767
	 * MinimumLSB = 0.0000305             (30.5uA per bit)
	 * MaximumLSB = MaxExpected_I/4096
	 * MaximumLSB = 0.000244              (244uA per bit)

	 * 4. Choose an LSB between the min and max values
	 *    (Preferrably a roundish number close to MinLSB)
	 * CurrentLSB = 0.0000400 (40uA per bit)

	 * 5. Compute the calibration register
	 * Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
	 * Cal = 10240 (0x2800)
	 */

	_calValue = 10240;

	/*
	 *
	 * 6. Calculate the power LSB
	 * PowerLSB = 20 * CurrentLSB
	 * PowerLSB = 0.0008 (800uW per bit)

	 * 7. Compute the maximum current and shunt voltage values before overflow

	 * Max_Current = Current_LSB * 32767
	 * Max_Current = 1.31068A before overflow

	 * If Max_Current > Max_Possible_I then
	 *    Max_Current_Before_Overflow = MaxPossible_I
	 * Else
	 *    Max_Current_Before_Overflow = Max_Current
	 * End If

	 * ... In this case, we're good though since Max_Current is less than MaxPossible_I

	 * Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
	 * Max_ShuntVoltage = 0.131068V

	 * If Max_ShuntVoltage >= VSHUNT_MAX
	 *    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
	 * Else
	 *    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
	 * End If

	 * 8. Compute the Maximum Power
	 * MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
	 * MaximumPower = 1.31068 * 32V
	 * MaximumPower = 41.94176W
	 */

	// Set multipliers to convert raw current/power values
	_currentDividerMilliAmp = 25.0; // Current LSB = 40uA per bit (1000/40 = 25)
	// _powerDividerMilliWatt = 1; // Power LSB = 800uW per bit

	// Set Calibration register to 'Cal' calculated above
	if ( I2C::writeUInt16(_i2cAddr, REG_CALIBRATION, _calValue) )
	{
		// Set Config register to take into account the settings above
		_config = CFG_BVOLTAGERANGE_32V
					| CFG_GAIN_8_320MV
					| CFG_BADCRES_12BIT
					| getAveraging(sampleCount)
					| CFG_MODE_SANDBVOLT_CONTINUOUS;

		return I2C::writeUInt16(_i2cAddr, REG_CONFIG, _config);
	}

	return false;
}

/**************************************************************************/
/*!
    @brief  Configures to INA219 to be able to measure up to 16V and 400mA
            of current.  Each unit of current corresponds to 20uA, and each
            unit of power corresponds to ???W. Counter overflow occurs at
            ???A.

    @note   These calculations assume a 0.1 ohm resistor is present
*/
/**************************************************************************/
bool INA219::setRange16V400mA(int sampleCount)
{
	/*
	 * By default we use a pretty huge range for the input voltage,
	 * which probably isn't the most appropriate choice for system
	 * that don't use a lot of power.  But all of the calculations
	 * are shown below if you want to change the settings.  You will
	 * also need to change any relevant register settings, such as
	 * setting the VBUS_MAX to 16V instead of 32V, etc.

	 * VBUS_MAX = 16V    (Assumes 32V, can also be set to 16V)
	 * VSHUNT_MAX = 0.16 (Assumes Gain 8, 320mV, can also be 0.16, 0.08, 0.04)
	 * RSHUNT = 0.1      (Resistor value in ohms)

	 * 1. Determine max possible current
	 * MaxPossible_I = VSHUNT_MAX / RSHUNT
	 * MaxPossible_I = 1.6A

	 * 2. Determine max expected current
	 * MaxExpected_I = 0.4A

	 * 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
	 * MinimumLSB = MaxExpected_I/32767
	 * MinimumLSB = 0.0000122             (30.5uA per bit)
	 * MaximumLSB = MaxExpected_I/4096
	 * MaximumLSB = 0.0000977             (244uA per bit)

	 * 4. Choose an LSB between the min and max values
	 *    (Preferrably a roundish number close to MinLSB)
	 * CurrentLSB = 0.0000200 (20uA per bit)

	 * 5. Compute the calibration register
	 * Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
	 * Cal = 20480
	 */

	_calValue = 20480;

	/*
	 *
	 * 6. Calculate the power LSB
	 * PowerLSB = 20 * CurrentLSB
	 * PowerLSB = 0.0004 (800uW per bit)

	 * 7. Compute the maximum current and shunt voltage values before overflow

	 * Max_Current = CurrentLSB * 32767
	 * Max_Current = 0.65534A before overflow

	 * If Max_Current > Max_Possible_I then
	 *    Max_Current_Before_Overflow = MaxPossible_I
	 * Else
	 *    Max_Current_Before_Overflow = Max_Current
	 * End If

	 * ... In this case, we're good though since Max_Current is less than MaxPossible_I

	 * Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
	 * Max_ShuntVoltage = 0.065534V

	 * If Max_ShuntVoltage >= VSHUNT_MAX
	 *    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
	 * Else
	 *    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
	 * End If

	 * 8. Compute the Maximum Power
	 * MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
	 * MaximumPower = 0.65534A * 16V
	 * MaximumPower = 10.48544W
	 */

	// Set multipliers to convert raw current/power values
                                    // CurrentLSB = 0.0000200 (20uA per bit)
	_currentDividerMilliAmp = 50.0; // Current LSB = 20uA per bit (1000/20 = 50)
	// _powerDividerMilliWatt = 1;  // Power LSB = ???uW per bit

	// Set Calibration register to 'Cal' calculated above
	if ( I2C::writeUInt16(_i2cAddr, REG_CALIBRATION, _calValue) )
	{
		// Set Config register to take into account the settings above
		_config = CFG_BVOLTAGERANGE_16V
					| CFG_GAIN_4_160MV
					| CFG_BADCRES_12BIT
					| getAveraging(sampleCount)
					| CFG_MODE_SANDBVOLT_CONTINUOUS;

		return I2C::writeUInt16(_i2cAddr, REG_CONFIG, _config);
	}

	return false;
}

void INA219::delayRead(void) const
{
	switch ( _config & CFG_SADCRES_MASK )
	{
	case CFG_SADCRES_9BIT_1S_84US: delayMicroseconds(84); break;
	case CFG_SADCRES_10BIT_1S_148US: delayMicroseconds(148); break;
	case CFG_SADCRES_11BIT_1S_276US: delayMicroseconds(276); break;
	case CFG_SADCRES_12BIT_1S_532US: delayMicroseconds(532); break;
	case CFG_SADCRES_12BIT_2S_1060US: delayMicroseconds(1060); break;
	case CFG_SADCRES_12BIT_4S_2130US: delayMicroseconds(2130); break;
	case CFG_SADCRES_12BIT_8S_4260US: delayMicroseconds(4260); break;
	case CFG_SADCRES_12BIT_16S_8510US: delay(9); break;
	case CFG_SADCRES_12BIT_32S_17MS: delay(17); break;
	case CFG_SADCRES_12BIT_64S_34MS: delay(34); break;
	case CFG_SADCRES_12BIT_128S_69MS: delay(69); break;
	default: break;
	}
}

} // namespace Xilka

