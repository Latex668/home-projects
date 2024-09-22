#include <Adafruit_GFX.h>               // Adafruit LCD Graphics library
#include <Adafruit_ST7735.h>            // ST7735 LCD Display library
#include <SPI.h>                        // Arduino SPI and I2C library

#define TFT_CS      9       // using SPI1 CSn
#define TFT_RST     8
#define TFT_DC      7 // A0

// Used for SPI connectivity on SPI1
#define TFT_SCK     10
#define TFT_MOSI    11 // SDA
// #define TFT_MISO         // not used with this display

// Setup the ST7735 LCD Display and variables
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST);

int rotation = 0; // Screen rotation
int gridSize = 10;

byte SC_W = 128;
byte SC_H = 160; 

int buttons[4] = {18,19,20,21}; // U,L,D,R
bool butStates[4];


#define SNAKE_MAX_LEN 12
int snake_x[SNAKE_MAX_LEN];
int snake_y[SNAKE_MAX_LEN];
int snake_len = 2;
int score = 0;
int snake_dir = 0; // 0=R,1=U,2=L,3=D
int food_x;
int food_y;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  for(int i=0;i<4;i++){
    pinMode(buttons[i],INPUT_PULLUP);
  }
  tft.initR(INITR_GREENTAB);
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(rotation);
  drawGrid();
  food_x = random(0,SC_W/gridSize);
  food_y = random(0,SC_H/gridSize);
  snake_x[0] = 0;
  snake_y[0] = 0;
  snake_x[1] = 1;
  snake_y[1] = 0;
  drawSnake();
  drawFood();
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int j=0;j<4;j++){
    butStates[j] = digitalRead(buttons[j]);
  }

  if(!butStates[3] && snake_dir != 2){
    snake_dir = 0;
  }else if(!butStates[1] && snake_dir != 0){
    snake_dir = 2;
  }else if(!butStates[2] && snake_dir != 1){
    snake_dir = 3;
  }else if(!butStates[0] && snake_dir != 3){
    snake_dir = 1;
  }
  for(int i = snake_len-1;i>0;i--){
    snake_x[i] = snake_x[i-1];
    snake_y[i] = snake_y[i-1];
  }
  switch(snake_dir){
    case 0:
      snake_x[0]++;
      break;
    case 1:
      snake_y[0]--;
      break;
    case 2:
      snake_x[0]--;
      break;
    case 3:
    snake_y[0]++;
    break;
  }

  if(snake_x[0]<0){
    snake_x[0] - SC_W/gridSize-1;
  }else if(snake_x[0]>=SC_W/gridSize){
    snake_x[0]=0;
  }

  if(snake_y[0]<0){
    snake_y[0]= SC_H/gridSize-1;
  }else if(snake_y[0]>=SC_H/gridSize){
    snake_y[0]=0;
  }

  // eat apple
  if(snake_x[0] == food_x && snake_y[0] == food_y){
    if(snake_len<SNAKE_MAX_LEN){
      snake_len++;
    }
    score +=10;
    food_x = random(0, SC_W/gridSize);
    food_y = random(0,SC_H/gridSize);
  }
  for(int i=1;i<snake_len;i++){
    if(snake_x[0]==snake_x[i]&&snake_y[0]==snake_y[i]){
      gameOver();
      Reset();
    }
  }
  tft.fillRect(0,0,SC_W,SC_H,0x0000);
  drawGrid();
  drawSnake();
  drawFood();
}

void drawSnake(){
  for(int i=0;i<snake_len;i++){
    tft.fillRect(snake_x[i]*gridSize,snake_y[i]*gridSize,gridSize,gridSize,ST7735_GREEN);
  }
}
void drawFood(){
  tft.fillRect(food_x*gridSize,food_y*gridSize,gridSize,gridSize,ST7735_RED);
}
void gameOver(){
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(5);
  tft.setTextColor(0xf815);
  tft.setCursor(5,20);
  tft.println("Game Over!");
  tft.setCursor(5,30);
  tft.print("Score: ");
  tft.println(score);
  delay(3000);
}
void Reset(){
  score = 0;
  food_x = random(0, SC_W/gridSize);
  food_y = random(0,SC_H/gridSize);
  snake_len = 2;
  snake_dir = 0;
  snake_x[0] = 0;
  snake_y[0] = 0;
  snake_x[1] = 1;
  snake_y[1] = 0;
}

void drawGrid(){
  for(int x=0;x<SC_W;x+=gridSize){
    for(int y=0;y<SC_H;y+=gridSize){
      tft.drawRect(x,y,gridSize,gridSize,0x9cf3);
    }
  }
}
