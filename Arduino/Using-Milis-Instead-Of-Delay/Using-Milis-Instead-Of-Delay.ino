int ledPins[4] = {13,12,14,27};
const unsigned long eventLed1 = 1500; // First half of the leds will be turned on at this period
const unsigned long eventLed2 = 2600; // Second half at this period.
unsigned long previousTime1 = 0; // variable to store previous T1
unsigned long previousTime2 = 0; // variable to store previous T2

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  for(int i = 0; i<4;i++){
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentTime = millis(); // Read the current time
 
  if(currentTime - previousTime1 >= eventLed1){
    previousTime1 = currentTime;
    Serial.println("First half on");
    for(int i = 0; i<2;i++){
      digitalWrite(ledPins[i], !digitalRead(ledPins[i]));
    }
  }
  if(currentTime-previousTime2 >= eventLed2){
    previousTime2 = currentTime;
    Serial.println("Second half on");
    for(int i=2;i<4;i++){
      digitalWrite(ledPins[i],!digitalRead(ledPins[i]));
    }
  }
}
