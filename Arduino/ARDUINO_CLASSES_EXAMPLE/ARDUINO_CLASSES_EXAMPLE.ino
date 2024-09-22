#include "LED.h"

LED led(18);
byte n = 0;


void setup() {
  // put your setup code here, to run once:
  led.init(0);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(n>255){
    n=0;
  }
  led.brightness(n);
  n++;
  delay(30);
}
