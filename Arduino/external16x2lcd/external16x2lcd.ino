// Code for my board that has an external 16x2 lcd and a pi pico.
// I'll try to use serial comms for displaying info such as time/temp from the pc.
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x26,16,2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.setTimeout(1);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  while(!Serial){ // While serial isnt open, write a starting message and add a . to the end every sec
    String startMessage = "Starting";
    for(int j=0;j<3;j++){
      for(int i=1;i<5;i++){
        lcd.clear();
        lcd.print(startMessage);
        startMessage = startMessage+".";
        delay(1000);
      }
      startMessage = "Starting";
  if(j==2){ // Added another for loop so that if it goes on for 3x without connecting to the COM port the backlight and display turns off.
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("No COM!");
    lcd.setCursor(0, 1);
    lcd.print("Turning off!");
    delay(3000);
    lcd.noBacklight();
    lcd.noDisplay();
    lcd.clear();
  }}
  }
  lcd.display();
  lcd.backlight();
  lcd.clear();
  lcd.print("COM Started!");
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!Serial){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("COM closed!");
    lcd.setCursor(0, 1);
    lcd.print("Restarting!");
    delay(1000);
    setup();
  }
  if(Serial.available()>0){ // If serial is available, save time and date till the separators and then display them (it was such a pain to come to this solution)
    lcd.clear();
    String Time = Serial.readStringUntil(',');
    String Date = Serial.readStringUntil('.');
    lcd.setCursor(0,0);
    lcd.print(Time);
    lcd.setCursor(0,1);
    lcd.print(Date);
    delay(100);
  }
}
  

