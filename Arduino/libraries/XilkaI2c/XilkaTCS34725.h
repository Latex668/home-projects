#pragma once
/* vim: set expandtab tabstop=4: */

/*
 * Typical sleep current 2.5uA, active current 235uA
 */

#include <Arduino.h>

#include "XilkaI2c.h"

namespace Xilka {

class TCS34725
{
public:
    TCS34725();

    bool begin(uint8_t i2cAddr=0x29);

    /**<  2.4ms - 1 cycle    - Max Count: 1024  */
    bool setIntegrationTime2_4(void) { return setIntegrationTime(0xFF); }
    /**<  24ms  - 10 cycles  - Max Count: 10240 */
    bool setIntegrationTime24(void) { return setIntegrationTime(0xF6); }
    /**<  50ms  - 20 cycles  - Max Count: 20480 */
    bool setIntegrationTime50(void) { return setIntegrationTime(0xEB); }
    /**<  101ms - 42 cycles  - Max Count: 43008 */
    bool setIntegrationTime101(void) { return setIntegrationTime(0xD5); }
    /**<  154ms - 64 cycles  - Max Count: 65535 */
    bool setIntegrationTime154(void) { return setIntegrationTime(0xC0); }
    /**<  700ms - 256 cycles - Max Count: 65535 */
    bool setIntegrationTime700(void) { return setIntegrationTime(0x00); }

    bool setGain1X(void) { return setGain(0x00); }
    bool setGain4X(void) { return setGain(0x01); }
    bool setGain16X(void) { return setGain(0x02); }
    bool setGain60X(void) { return setGain(0x03); }

    /*
     * Do this once (usually per loop)
     * then use the get functions
     * to evaluate the results
     */
    bool getData(void);

    float getRed(void) const { return _adjustedRed * _itgFactor; }
    float getGreen(void) const { return _adjustedGreen * _itgFactor; }
    float getBlue(void) const { return _adjustedBlue * _itgFactor; }
    float getClear(void) const { return _adjustedClear * _itgFactor; }
    float getLux(void) const { return getClear(); }
    float getColorTemp(void) const;

    /*
     * you may use powerUp/powerDown if you want to
     * put the sensor to sleep for extended periods
     *
     * Mostly just use powerDown since getData()
     * will auto powerUp up the device
     */
    bool powerUp(void);
    bool powerDown(void);

    bool setInterrupt(bool flag) const;
    bool clearInterrupt(void) const;
    bool setIntLimits(uint16_t low, uint16_t high) const;

    static unsigned long MaxI2CSpeed;

private:
    bool setIntegrationTime(uint8_t it);
    bool setGain(uint8_t gain);

    uint8_t _i2cAddr;
    bool _initialized;
    bool _enabled;
    unsigned long _lastRead;

    uint8_t _tcsGain;
    uint8_t _tcsIntegrationTime;

    uint16_t _readDelay;

    float _integrationTime;
    float _gain;

    float _itgFactor;
    float _adjustedRed;
    float _adjustedGreen;
    float _adjustedBlue;
    float _adjustedClear;
};

} // namespace Xilka

