#include "LED.h"

LED::LED(byte pin){ // pull pin as private pin
  this->pin = pin; 
}
void LED::init(){
  pinMode(pin,OUTPUT); // init led
}
void LED::init(byte defaultState){ // init led with def state
  init();
  if(defaultState == 1){
    state(1);
  }else{
    state(0);
  }
}
void LED::state(bool state){ // change led state
  digitalWrite(pin,state);
}
void LED::brightness(byte brightness){ // change led brightness
  analogWrite(pin,brightness);
}
