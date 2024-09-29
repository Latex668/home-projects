#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#define PIN 6

#define sgn(x) ((x) < 0 ? -1 : ((x) > 0 ? 1 : 0))

float angle = 0;
int color = 0;
const int pi = 3.14159;

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  matrix.begin();
  matrix.setBrightness(50);
}

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t colors[] = {
    matrix.Color(random(125,255),random(137,255),random(0,125)), matrix.Color(255,0,0)
  };
  matrix.fillScreen(0);
  //sineWave(4, colors, 10);
  squareWave(4, colors, 1);
  matrix.show();
}

void sineWave(int amplitude, uint16_t colors[2], int frequency){ // Takes in the amplitude and frequency of the wave and the colors of the pixels
  for(int x = matrix.width(); x>=0;x--){ // cycle through all x pixels
    if(x >= matrix.width()-3){ // if these are the last pixels set them to first color, otherwise set it to the second
      color = 0;
    } else{
      color = 1;
    }
    int y = 4-(amplitude * sin(angle*pi/180)); // Calculates the y coordonates given an angle
    matrix.drawPixel(x,y,colors[color]);
    angle+=360/10; // angle increases by 36 degrees
  }
  delay(1000/frequency); // delay by the period in ms (T=1/f (s))
}

void squareWave(int amplitude, uint16_t colors[2], int frequency){
  for(int x = 0; x<=matrix.width();x++){
    for(int y = 0;y<=matrix.height();y++){
       if(x==0){
        matrix.drawPixel(x,y,colors[1]);
       }
       if(y==matrix.height()){
        matrix.drawPixel(x,y,colors[1]);
       }
      }
    }
  }
