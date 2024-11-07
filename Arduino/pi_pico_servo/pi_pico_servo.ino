#include <ESP32Servo.h>
#define SERVO 12
#define BUTON 14

const int ledPins[4] = {13,12,14,27};


void setup() {
  // put your setup code here, to run once:
  pinMode(BUTON, INPUT_PULLUP);
  for(int i = 0; i<4; i++){
    pinMode(ledPins[i],OUTPUT);
  }
}
int a = 0;
void loop() {
  // put your main code here, to run repeatedly:
  for(int i=0;i<4;i++){
    digitalWrite(ledPins[i],1);
    delay(500);
  }
  delay(1000);
  for(int j=4;j<0;j--){
    digitalWrite(ledPins[j],0);
    delay(500);
  }

  }

