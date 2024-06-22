/*
DIY pov display, code remade by Alex in 2024
 * code written by Palak Mehta on March 29,2019
*/

#include "pain.h"
 
 int* alpha[]= {A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,N0,N1,N2,N3,N4,N5,N6,N7,N8,N9};
 int letterSpace;
 int delayTime;
 int startpin = 12; // the pin you start counting on for led input

const int IR_pin = 2;
 
 void setup()
 { 
         pinMode(IR_pin, INPUT_PULLDOWN);
         for(int i = startpin; i<17 ;i++) // setting the ports of the leds to OUTPUT
           {
            pinMode(i, OUTPUT);
           }
                
         letterSpace = 4;// defining the space between the letters (ms)         
         delayTime = 1;// defining the time dots appear (ms)
}


 void printLetter(int letter[])
 {
           int y;
           int nled = 5;
          // printing the first y row of the letter
           for (y=0; y<nled; y++)
           {
           digitalWrite(y+startpin, letter[y]);
           }
           delay(delayTime);
          // printing the second y row of the letter then add + n for the number of rows you have
           for (y=0; y<nled; y++)
           {
           digitalWrite(y+startpin, letter[y+nled]);
           }
           delay(delayTime);
          // printing the third y row of the letter
           for (y=0; y<nled; y++)
           {
           digitalWrite(y+startpin, letter[y+nled+5]);
           }
           delay(delayTime);
           for(y=0; y<nled; y++) {
           digitalWrite(y+startpin, letter[y+10]);
           }
           delay(delayTime);
           for(y=0; y<nled; y++) {
           digitalWrite(y+startpin, letter[y+15]);
           }
           delay(delayTime);
           // printing the space between the letters
           for (y=0; y<nled; y++)
           {
           digitalWrite(y+startpin, 0);
           }
           delay(letterSpace);
 }

 
 void loop()
 {
  if(digitalRead(IR_pin) == LOW)
  {
     printLetter(A);
     printLetter(L);
     printLetter(E);
     printLetter(X);
     printLetter(_);
  }
     
}
