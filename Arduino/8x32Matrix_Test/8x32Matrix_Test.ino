#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#define PIN 6

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);




void setup() {
  // put your setup code here, to run once:
  matrix.begin();
  matrix.setBrightness(40);

}
float angle = 0;
int color = 0;
void loop() {
  // put your main code here, to run repeatedly:

  const uint16_t colors[] = {
    matrix.Color(random(125,255),random(137,255),random(0,125)), matrix.Color(255,0,0)
  };

  matrix.fillScreen(0);
  for(int x = matrix.width(); x>=0;x--){
    if(x >= matrix.width()-3){
      color = 0;
    }else{
      color = 1;
    }
    int y = 4-(4 * sin(angle*3.14159/180));
    matrix.drawPixel(x,y,colors[color]);
    angle+=360/10;
  }
  delay(125);
  matrix.show();
}
