#ifndef LED_H
#define LED_H

#include <Arduino.h>

class LED{
  private:
    byte pin;
  public:
  LED(){} // don't use
  LED(byte pin);
  void init();
  void init(byte defaultState);
  void state(bool state);
  void brightness(byte brightness);
};


#endif