/* vim: set expandtab tabstop=4: */

/**************************************************************************/
/*!
    @file     XilkaTCS34725.cpp
    @author   KTOWN (Adafruit Industries)
    enhanced by Kelly Anderson
    @license  BSD (see license.txt)

    Driver for the TCS34725 digital color sensors.

    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!

    @section  HISTORY

    v1.0 - First release
    v2.0 - Xilka enhancements 2015.07.19 Kelly Anderson
*/
/**************************************************************************/
#include <Arduino.h>
#include <Wire.h>

#if defined(ARDUINO_ARCH_AVR)
#include <avr/pgmspace.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <pgmspace.h>
#endif
#include <stdlib.h>
#include <math.h>

#include "XilkaTCS34725.h"

namespace {

const uint8_t REG_COMMAND_BIT(0x80);

const uint8_t REG_ENABLE(0x00);
const uint8_t REG_ENABLE_AIEN(0x10); // RGBC Interrupt Enable
const uint8_t REG_ENABLE_WEN(0x08);  // Wait enable - Writing 1 activates the wait timer
const uint8_t REG_ENABLE_AEN(0x02);  // RGBC Enable - Writing 1 actives the ADC, 0 disables it
const uint8_t REG_ENABLE_PON(0x01);  // Power on - Writing 1 activates the internal oscillator, 0 disables it
const uint8_t REG_ATIME(0x01);       // Integration time
#if 0
const uint8_t REG_WTIME(0x03);       // Wait time (if REG_ENABLE_WEN is asserted)
const uint8_t REG_WTIME_2_4MS(0xFF); // WLONG0 = 2.4ms   WLONG1 = 0.029s
const uint8_t REG_WTIME_204MS(0xAB); // WLONG0 = 204ms   WLONG1 = 2.45s
const uint8_t REG_WTIME_614MS(0x00); // WLONG0 = 614ms   WLONG1 = 7.4s
const uint8_t REG_AILTL(0x04);       // Clear channel lower interrupt threshold
const uint8_t REG_AILTH(0x05);
const uint8_t REG_AIHTL(0x06);       // Clear channel upper interrupt threshold
const uint8_t REG_AIHTH(0x07);

const uint8_t REG_PERS(0x0C);            // Persistence register - basic SW filtering mechanism for interrupts
const uint8_t REG_PERS_NONE(0b0000);     // Every RGBC cycle generates an interrupt
const uint8_t REG_PERS_1_CYCLE(0b0001);  // 1 clean channel value outside threshold range generates an interrupt
const uint8_t REG_PERS_2_CYCLE(0b0010);  // 2 clean channel values outside threshold range generates an interrupt
const uint8_t REG_PERS_3_CYCLE(0b0011);  // 3 clean channel values outside threshold range generates an interrupt
const uint8_t REG_PERS_5_CYCLE(0b0100);  // 5 clean channel values outside threshold range generates an interrupt
const uint8_t REG_PERS_10_CYCLE(0b0101);  // 10 clean channel values outside threshold range generates an interrupt
const uint8_t REG_PERS_15_CYCLE(0b0110);  // 15 clean channel values outside threshold range generates an interrupt
const uint8_t REG_PERS_20_CYCLE(0b0111);  // 20 clean channel values outside threshold range generates an interrupt
const uint8_t REG_PERS_25_CYCLE(0b1000);  // 25 clean channel values outside threshold range generates an interrupt
const uint8_t REG_PERS_30_CYCLE(0b1001);  // 30 clean channel values outside threshold range generates an interrupt
const uint8_t REG_PERS_35_CYCLE(0b1010);  // 35 clean channel values outside threshold range generates an interrupt
const uint8_t REG_PERS_40_CYCLE(0b1011);  // 40 clean channel values outside threshold range generates an interrupt
const uint8_t REG_PERS_45_CYCLE(0b1100);  // 45 clean channel values outside threshold range generates an interrupt
const uint8_t REG_PERS_50_CYCLE(0b1101);  // 50 clean channel values outside threshold range generates an interrupt
const uint8_t REG_PERS_55_CYCLE(0b1110);  // 55 clean channel values outside threshold range generates an interrupt
const uint8_t REG_PERS_60_CYCLE(0b1111);  // 60 clean channel values outside threshold range generates an interrupt

#endif

#if 0
const uint8_t REG_CONFIG(0x0D);
const uint8_t REG_CONFIG_WLONG(0x02);  // Choose between short and long (12x) wait times via REG_WTIME
#endif

const uint8_t REG_CONTROL(0x0F); // Set the gain level for the sensor
const uint8_t REG_ID(0x12);      // 0x44 = TCS34721/TCS34725, 0x4D = TCS34723/TCS34727
const uint8_t REG_STATUS(0x13);
const uint8_t REG_STATUS_AINT(0x10); // RGBC Clean channel interrupt
const uint8_t REG_STATUS_AVALID(0x01);  // Indicates that the RGBC channels have completed an integration cycle

const uint8_t REG_CLEAR_LOW(0x14);
const uint8_t REG_CLEAR_HIGH(0x15);
const uint8_t REG_RED_LOW(0x16);
const uint8_t REG_RED_HIGH(0x17);
const uint8_t REG_GREEN_LOW(0x18);
const uint8_t REG_GREEN_HIGH(0x19);
const uint8_t REG_BLUE_LOW(0x1A);
const uint8_t REG_BLUE_HIGH(0x1B);

uint16_t getReadDelay(uint8_t t)
{
    switch ( t )
    {
    case 0xFF: return 3; break;
    case 0xF6: return 24; break;
    case 0xEB: return 50; break;
    case 0xD5: return 101; break;
    case 0xC0: return 154; break;
    default: case 0x00: return 700; break;
    }
}

float getIntegrationTime(uint8_t t)
{
    switch ( t )
    {
    case 0xFF: return 2.4; break;
    case 0xF6: return 24.0; break;
    case 0xEB: return 50.0; break;
    case 0xD5: return 101.0; break;
    case 0xC0: return 154.0; break;
    default: case 0x00: return 700.0; break;
    }
}

float getGain(uint8_t g)
{
    switch ( g )
    {
    default:
    case 0x00: return 1.0; break;
    case 0x01: return 4.0; break;
    case 0x02: return 16.0; break;
    case 0x03: return 60.0; break;
    }
}

} // anonymous namespace

namespace Xilka {

unsigned long TCS34725::MaxI2CSpeed(400000UL);

TCS34725::TCS34725()
    : _i2cAddr(0)
    , _initialized(false)
    , _enabled(false)
    , _lastRead(millis())
    , _tcsGain(0x00)
    , _tcsIntegrationTime(0xFF)
    , _readDelay(0)
    , _integrationTime(1.0)
    , _gain(1.0)
    , _itgFactor(1.0)
    , _adjustedRed(0.0)
    , _adjustedGreen(0.0)
    , _adjustedBlue(0.0)
    , _adjustedClear(0.0)
{
}

bool TCS34725::begin(uint8_t i2cAddr)
{
    _i2cAddr = i2cAddr;

    uint8_t id(0);

    if ( ! I2C::readUInt8(_i2cAddr, REG_COMMAND_BIT | REG_ID, id)
            || id != 0x44 )
        return false;

    if ( id != 0x44 )
        return false;

    _initialized = true;

    setIntegrationTime(_tcsIntegrationTime);
    setGain(_tcsGain);
    powerUp();

    return true;
}

bool TCS34725::powerUp(void)
{
    if ( ! _enabled )
    {
        _enabled = true;

        if ( I2C::writeUInt8(_i2cAddr, REG_COMMAND_BIT | REG_ENABLE, REG_ENABLE_PON) )
        {
            delay(3);

            return I2C::writeUInt8(_i2cAddr, REG_COMMAND_BIT | REG_ENABLE
                                    , REG_ENABLE_PON | REG_ENABLE_AEN);
        }

        return false;
    }

    return true;
}

bool TCS34725::powerDown(void)
{
    if ( _enabled )
    {
        _enabled = false;
        _initialized = false; // force reset of integration time and gain

        // Turn the device off to save power
        uint8_t reg(0);

        if ( I2C::readUInt8(_i2cAddr, REG_COMMAND_BIT | REG_ENABLE, reg) )
            return I2C::writeUInt8(_i2cAddr, REG_COMMAND_BIT | REG_ENABLE
                                    , reg & ~(REG_ENABLE_PON | REG_ENABLE_AEN));

        return false;
    }

    return true;
}

bool TCS34725::setIntegrationTime(uint8_t it)
{
    if ( ! _initialized )
        begin();


    if ( I2C::writeUInt8(_i2cAddr, REG_COMMAND_BIT | REG_ATIME, it) )
    {
        _tcsIntegrationTime = it;
        _readDelay = getReadDelay(it);
        _integrationTime = getIntegrationTime(it);
        _itgFactor = 1.0 / (_integrationTime * _gain);

        return true;
    }

    return false;
}

bool TCS34725::setGain(uint8_t gain)
{
    if ( ! _initialized )
        begin();

    if ( I2C::writeUInt8(_i2cAddr, REG_COMMAND_BIT | REG_CONTROL, gain) )
    {
        _tcsGain = gain;
        _gain = getGain(gain);
        _itgFactor = 1.0 / (_integrationTime * _gain);

        return true;
    }

    return false;
}

bool TCS34725::getData(void)
{
    if ( ! _initialized )
        begin();

    if ( ! _enabled )
        powerUp();

    const unsigned long ms(millis());
    const unsigned long elapsed(ms - _lastRead);
    _lastRead = ms;

    if ( elapsed < _readDelay )
        delay(_readDelay - elapsed);

    uint16_t c(0);
    uint16_t r(0);
    uint16_t g(0);
    uint16_t b(0);

    if ( I2C::readUInt16LE(_i2cAddr, REG_COMMAND_BIT | REG_CLEAR_LOW, c)
        && I2C::readUInt16LE(_i2cAddr, REG_COMMAND_BIT | REG_RED_LOW, r)
        && I2C::readUInt16LE(_i2cAddr, REG_COMMAND_BIT | REG_GREEN_LOW, g)
        && I2C::readUInt16LE(_i2cAddr, REG_COMMAND_BIT | REG_BLUE_LOW, b) )
    {
        _adjustedRed = (float)r / 0.8; // 0.8 is responsivity @ 25C
        _adjustedGreen = (float)g / 0.6; // ""
        _adjustedBlue = (float)b / 0.5; // ""
        _adjustedClear = (float)c / 0.7; // ""

        return true;
    }

    return false;
}

float TCS34725::getColorTemp(void) const
{
/*
 * Return the results in degrees Kelvin
 */

    /*
     * 1. Map RGB values to their XYZ counterparts.
     * Based on 6500K fluorescent, 3000K fluorescent
     * and 60W incandescent values for a wide range.
     * Note: Y = Illuminance or lux
     */
    const float &r(_adjustedRed);
    const float &g(_adjustedGreen);
    const float &b(_adjustedBlue);

    const float X((-0.14282F * r) + (1.54924F * g) + (-0.95641F * b));
    const float Y((-0.32466F * r) + (1.57837F * g) + (-0.73191F * b));
    const float Z((-0.68202F * r) + (0.77073F * g) + ( 0.56332F * b));

    // 2. Calculate the chromaticity co-ordinates
    const float xc((X) / (X + Y + Z));
    const float yc((Y) / (X + Y + Z));

    // 3. Use McCamy's formula to determine the CCT
    const float n((xc - 0.3320F) / (0.1858F - yc));

    // Calculate the final CCT
    const float cct((449.0F * powf(n, 3.0F)) + (3525.0F * powf(n, 2.0F)) + (6823.3F * n) + 5520.33F);

    return cct;
}

bool TCS34725::setInterrupt(bool i) const
{
    uint8_t reg(0);

    if ( I2C::readUInt8(_i2cAddr, REG_COMMAND_BIT | REG_ENABLE, reg) )
    {
        if ( i )
            reg |= REG_ENABLE_AIEN;
        else
            reg &= ~REG_ENABLE_AIEN;

        return I2C::writeUInt8(_i2cAddr, REG_COMMAND_BIT | REG_ENABLE, reg);
    }

    return false;
}

bool TCS34725::clearInterrupt(void) const
{
    Wire.beginTransmission(_i2cAddr);
    Wire.write(0x66);
    return ! Wire.endTransmission();
}

bool TCS34725::setIntLimits(uint16_t low, uint16_t high) const
{
    return I2C::writeUInt8(_i2cAddr, REG_COMMAND_BIT | 0x04, low & 0xFF)
            && I2C::writeUInt8(_i2cAddr, REG_COMMAND_BIT | 0x05, low >> 8)
            && I2C::writeUInt8(_i2cAddr, REG_COMMAND_BIT | 0x06, high & 0xFF)
            && I2C::writeUInt8(_i2cAddr, REG_COMMAND_BIT | 0x07, high >> 8);
}

} // namespace Xilka

