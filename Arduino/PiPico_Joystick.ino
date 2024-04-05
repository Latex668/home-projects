#include <Joystick.h>
// Setting the joystick pins for x, y axis and the button.
int X = 26;
int Y = 27;
const byte but = 16;

void setup() {
  // put your setup code here, to run once:
Joystick.begin(); // initializing joystick stuff
pinMode(but, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  Joystick.X(analogRead(X));
  delay(2);
  Joystick.Y(analogRead(Y));
  delay(2);
  Joystick.button(1, !digitalRead(but)); // setting the value of the joystick button as the inverse cuz of the pullup
  delay(10); // delay cuz pc might puke from data overload

}
