#include <ESP32Lib.h>
#include <Ressources/Font6x8.h>
#include "VGA/VGA6MonochromeVGAMadnessMultimonitor.h"

//pin configuration
//red pins
const int redPin0 = 17;
const int redPin1 = 2;
const int redPin2 = 19;
const int redPin3 = 12;
const int redPin4 = 23;
const int redPin5 = 26;
//green pins
const int greenPin0 = 16;
const int greenPin1 = 15;
const int greenPin2 = 18;
const int greenPin3 = 14;
const int greenPin4 = 22;
const int greenPin5 = 25;
//blue pins
const int bluePin0 = 4;
const int bluePin1 = 13;
const int bluePin2 = 5;
const int bluePin3 = 27;
const int bluePin4 = 21;
const int bluePin5 = 0;
const int hsyncPin = 32;
const int vsyncPin = 33;

//VGA Device
VGA6MonochromeVGAMadnessMultimonitor gfx;

const int xres = 320 * 3;
const int yres = 200 * 2;

const int starCount = 4000;
float stars[starCount][3];
Mesh<VGA6MonochromeVGAMadnessMultimonitor> model(starCount, stars, 0, 0, 0, 0, 0);

void initStars()
{
  for(int i = 0; i < starCount; i++)
  {
    stars[i][0] = random(201) - 100;
    stars[i][1] = random(201) - 100;
    stars[i][2] = random(201) - 100;
  }
}

//3D engine
Engine3D<VGA6MonochromeVGAMadnessMultimonitor> engine(1);

void setup()
{
  const int pinCount = 3 * 6 + 2;
  Serial.begin(115200);
  //initializing vga at the specified pins
  gfx.setFrameBufferCount(2);
  gfx.init(VGAMode::MODE320x240,
                    redPin0,greenPin0,bluePin0,
                    redPin1,greenPin1,bluePin1,
                    redPin2,greenPin2,bluePin2,
                    redPin3,greenPin3,bluePin3,
                    redPin4,greenPin4,bluePin4,
                    redPin5,greenPin5,bluePin5,
                    hsyncPin, vsyncPin, -1, 3, 2);
  //selecting the font
  gfx.setFont(Font6x8);
  initStars();
}


void loop()
{
  gfx.clear();
  //perspective transformation
  static Matrix perspective = Matrix::translation(gfx.xres / 2, gfx.yres / 2, 0) * Matrix::scaling(1000 * gfx.pixelAspect(), 1000, 1000) * Matrix::perspective(90, 1, 10);
  static float u = 0;
  static float v = 0;
  static float w = 0;
  u += 0.02;
  v += 0.01;
  w += 0.005;
  //rotate model
  Matrix rotation = Matrix::rotation(u, 1, 0, 0) * Matrix::rotation(v, 0, 1, 0) * Matrix::rotation(w, 0, 0, 1);
  Matrix m0 = perspective * Matrix::translation(0, 1.7 * 0, 200) * rotation;
  //transform the vertices and normals
  model.transform(m0, rotation);
  //begin adding triangles to render pipeline
  //engine.begin();
  //add this model to the render pipeline. it will sort the triangles from back to front and remove backfaced. The tiangle shader will determine the color of the tirangle.
  //the RGB color gien in the second parameter is not used in this case but could be used for calculations in the triangle shader
  model.drawVertices(gfx, gfx.RGB(255, 255, 255));
  //render all triangles in the pipeline. if you render multiple models you want to do this once at the end
  //engine.end(gfx);
  gfx.show();
}
