// Code for my DIY digital lab power supply (unfortunately it doesn't control the voltage regulators directly, it just controls all the other functions)

#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <SPI.h>

// Defining the pins of the LCD, I'll be using the predefined SPI pins of the nano board.
#define CS 10
#define RST 8
#define DC  9

// Set screen rotation
#define rotation 3

Adafruit_ST7735 lcd = Adafruit_ST7735(CS, DC, RST);

// Values of voltages and currents measured
float measurements[] = {0,0, 0,0}; // First 2 are V1,V2 last 2 are I1, I2
float AVcc = 4.72; // Measured voltage of the ADC reference
bool psuState = 0;
#define BUT 6
#define REL 12
float R1 = 992;
float R2 = 215;
float K = (R2/(R1+R2));
void setup() {
  Serial.begin(9600);
  pinMode(BUT, INPUT_PULLUP);
  pinMode(REL, OUTPUT);
  initLCD();
}

void loop() {
  Serial.println(measurements[0],2);
  float V1Val = ((float)analogRead(A0)/1024)*AVcc;
  measurements[0] = V1Val/K; // Measure value of V1
  measurements[1] = (analogRead(A1)/1023)*AVcc* ((990 + 217)/217); // Measure value of V2
  measurements[2] = (analogRead(A2)/1023)*AVcc; // Measure value of I1
  measurements[3] = (analogRead(A3)/1023)*AVcc; // Measure value of I2
  if(digitalRead(BUT) == 0){
    psuState = !psuState;
    digitalWrite(REL, psuState);
  }
  drawLCD(measurements[0], measurements[1], measurements[2], measurements[3], psuState);
}

void initLCD(){
  lcd.initR(INITR_GREENTAB); // Initializing with green tab because of offset. (colors aren't displayed right otherwise.)
  lcd.setRotation(rotation); 
  lcd.fillScreen(ST7735_BLACK);
  lcd.setTextColor(ST7735_YELLOW, ST7735_BLACK); // I really like yellow on black
  lcd.setTextSize(2);
}
void drawLCD(float V1Val, float V2Val, float I1Val, float I2Val, bool psuState){
  if(psuState){ // Check if PSU output is on/off
    lcd.drawRect(0,90,50,30, ST7735_GREEN);
    lcd.drawRect(1,91,50,30, ST7735_GREEN);
    lcd.setCursor(8,98);
    lcd.print("ON ");
  }else if(!psuState){
    lcd.drawRect(0,90,50,30, ST7735_RED);
    lcd.drawRect(1,91,50,30, ST7735_RED);
    lcd.setCursor(8,98);
    lcd.print("OFF");
  }
  // The code here from the bottom is just changing the text values of the measurements.
  // For voltages
  lcd.setCursor(0,0);
  lcd.print("V1: ");
  lcd.print(V1Val, 1);
  lcd.print(" ");
  lcd.setCursor(0,25);
  lcd.print("V2: ");
  lcd.print(V2Val, 1);
  lcd.print(" ");
  // For currents
  lcd.setCursor(0,50);
  lcd.print("I1: ");
  lcd.print(I1Val);
  lcd.print(" ");
  // Setting fixed unit of measurement positions
  lcd.setCursor(110,50); 
  lcd.print("A");
  lcd.setCursor(110,0);
  lcd.print("V");
  lcd.setCursor(110,25);
  lcd.print("V");
  delay(250);
}