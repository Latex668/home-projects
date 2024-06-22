int digitalLpin = 8; // define L signal pin
int digitalCpin = 9; // define C signal pin
int digitalRpin = 10; // define R signal pin
int digitalL; // define variable to store value read from pin
int digitalC; // define variable to store value read from pin
int digitalR; // define variable to store value read from pin

void setup() {
pinMode(digitalLpin, INPUT); // set the L signal pin as an input
pinMode(digitalCpin, INPUT); // set the C signal pin as an input
pinMode(digitalRpin, INPUT); // set the R signal pin as an input
Serial.begin(9600); // launch the serial monitor
}

void loop() {
digitalL = digitalRead(digitalLpin); // read the voltage level on the D7
digitalC = digitalRead(digitalCpin); // read the voltage level on the D6
digitalR = digitalRead(digitalRpin); // read the voltage level on the D5
if(digitalL == 1){
  Serial.println("Tilting left");
}

delay(500); // pause for a moment before repeating
}