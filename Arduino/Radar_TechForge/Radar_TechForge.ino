#include <ESP32Servo.h>
#define servoPin 13
#define echoPin 18
#define trigPin 5 
#define button 4

#define speedSound 0.034 // speed of sound in cm/us

long duration;
int distanceCm;

Servo servo;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  servo.attach(servoPin);
  servo.write(0);
  pinMode(echoPin, INPUT);
  pinMode(button, INPUT_PULLDOWN);
  pinMode(trigPin, OUTPUT);
}

void sendDistance(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * speedSound/2;
  Serial.print(distanceCm);
}

void start(){
  for(;;){
    for(int angle=0;angle<180;angle++){
      servo.write(angle);
      Serial.print(angle);
      Serial.print(",");
      sendDistance();
      Serial.print(".");
      delay(20);
    }
      delay(250);
    for(int angle=180;angle>0;angle--){
        servo.write(angle);
        Serial.print(angle);
        Serial.print(",");
        sendDistance();
        Serial.print(".");
        delay(20);
      }
    delay(250);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(button) == true){
    start();
  }
}
