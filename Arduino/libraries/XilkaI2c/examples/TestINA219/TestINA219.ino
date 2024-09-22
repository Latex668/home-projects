#include <Wire.h>
#include <XilkaINA219.h>

Xilka::INA219 ina219;

void setup(void)
{
    Serial.begin(115200);
    Serial.println("Hello!");

    Serial.println("Measuring voltage and current with INA219 ...");
    ina219.begin();
}

void loop(void)
{
    float mVShunt(ina219.getShuntVoltage_mV());
    float VBus(ina219.getBusVoltage_V());
    float VLoad(VBus + (mVShunt / 1000.0));
    float mACurrent(ina219.getCurrent_mA());

    Serial.print("Bus Voltage:   "); Serial.print(VBus); Serial.println(" V");
    Serial.print("Shunt Voltage: "); Serial.print(VShunt); Serial.println(" mV");
    Serial.print("Load Voltage:  "); Serial.print(VLoad); Serial.println(" V");
    Serial.print("Current:       "); Serial.print(mACurrent); Serial.println(" mA");
    Serial.println("");

    delay(2000);
}

