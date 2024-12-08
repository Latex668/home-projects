// Code for my board that has an external 16x2 lcd and a pi pico.
// I'll try to use serial comms for displaying info such as time/temp from the pc.
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x26,16,2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.setTimeout(1);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  while(!Serial){ // While serial isnt open, write a starting message and add a . to the end every sec
    String startMessage = "Starting";
    for(int i=1;i<5;i++){
      lcd.clear();
      lcd.print(startMessage);
      startMessage = startMessage+".";
      delay(1000);
    }
    startMessage = "Starting";
  }
  lcd.clear();
  lcd.print("COM Started!");
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()>0){ // If serial is available, save time and date till the separators and then display them (it was such a pain to come to this solution)
    String Time = Serial.readStringUntil(',');
    Serial.read();
    String Date = Serial.readStringUntil('\n');
    Serial.read();
    lcd.setCursor(0,0);
    lcd.println(Time);
    lcd.setCursor(0,1);
    lcd.println(Date);
  }
}
  

