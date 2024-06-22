// Code used for the line following robot.
// Lat updated: 27/1/2024
const int  MotorControl[] = {5,6,7,8,9,10}; // Define the motor driver input controls
const int  LineSens    [] = {A0,A1,A2    }; // Define Line sensor's L, C, R outputs
int        LineVal     [] = {0,0,0       }; // Variable to check the sensor's values
byte brightnessVal = 127;                   // Set the min brightness val for car to move
char Direction;                             // Define the direction of the car
int LineAVG;

// PID stuff
float Kp = 5;    // proportional constant
float Ki = 8;    // integral constant
float Kd = 0.01; // derivate constant
int P, I, D;
int error;
int lastError = 0;

void setup() {
pinMode(MotorControl, OUTPUT);
pinMode(LineSens    , INPUT );
Serial.begin(9600);
}

void loop() {
  // Read the sensor's values and then average them out
  LineVal[0] = analogRead(LineSens[0]);
  LineVal[1] = analogRead(LineSens[1]);
  LineVal[2] = analogRead(LineSens[2]);
  LineAVG = (LineVal[0] + LineVal[1] + LineVal[2]) / 12;
  LineSensor(LineAVG);
  Control(Direction);
}


void Control(char x) {

  // Switch between the possible directions
  switch(x){
    case 'F':
      //set motor1 forward
    digitalWrite(MotorControl[0], 1 );
    digitalWrite(MotorControl[1], 0 );
      //set motor2 forward
    digitalWrite(MotorControl[2], 1 );
    digitalWrite(MotorControl[3], 0 );
    break;
    case 'B':
     //set motor1 backward
    digitalWrite(MotorControl[0], 0 );
    digitalWrite(MotorControl[1], 1 );
      //set motor2 backward
    digitalWrite(MotorControl[2], 0 );
    digitalWrite(MotorControl[3], 1 );
    break;
    case 'L':
      //set motor1 off
    digitalWrite(MotorControl[0], 0 );
    digitalWrite(MotorControl[1], 0 );
      //set motor2 forward
    digitalWrite(MotorControl[2], 1 );
    digitalWrite(MotorControl[3], 0 );
    break;
    case 'R':
      //set motor1 forward
    digitalWrite(MotorControl[0], 1 );
    digitalWrite(MotorControl[1], 0 );
      //set motor2 off
    digitalWrite(MotorControl[2], 0 );
    digitalWrite(MotorControl[3], 0 );
    break;}

  }
  
void LineSensor(int LineAVG){
  error = LineAVG - 10;
  P = error;
  I = I + error;
  D = error - lastError;
  lastError = error;
  int pwm = P*Kp + I*Ki + D*Kd; 
  int pwma = 100 + pwm;
  int pwmb = 100 - pwm;
  if(pwma > 200){
    pwma = 200;
  }else if(pwmb > 200){
    pwmb = 200;
  }else if(pwma < 0){
    pwma = 0;
  }else if(pwmb < 0){
    pwmb = 0;
  }
  analogWrite(MotorControl[4], pwma);
  analogWrite(MotorControl[5], pwmb);
  /*
  if       (LineVal[0] < brightnessVal || (LineVal[0] < brightnessVal && LineVal[1] < brightnessVal)){
    Direction = 'R';
  }else if (LineVal[2] < brightnessVal || (LineVal[2] < brightnessVal && LineVal[1] < brightnessVal)){
    Direction = 'L';
  }else if (LineVal[1] < brightnessVal){
    Direction = 'F';
  }
  */
  Direction = 'F'; // Test to see if it works with just varying the speed of each motor and not manually changing direction
  return Direction;
}


