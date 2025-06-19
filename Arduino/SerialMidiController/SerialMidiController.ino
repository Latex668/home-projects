const int touchPins[] = {4,15,13,12,14,27,33,32};
int touchStates[] = {1,1,1,1,1,1,1,1,1,1,1,1};
const int sensorPins[] = {5,18,19,21};
const int Threshold = 30;
int lastVal[] = {};
int touchVal[] = {};
int Velocity = 69;
int lastVel;
int lastPitch;
const int bendPin = 35;
const int velocityPin = 34;
int bendVal;
int lastBend = 0;
void setup() {
  Serial.begin(115200);
  for(int k=0;k<4;k++){
    pinMode(sensorPins[k], INPUT_PULLDOWN);
  }
}
void sendNoteOn(int pitch, int vel){
  Serial.write(144);
  Serial.write(pitch);
  Serial.write(vel);
}
void sendNoteOff(int pitch, int vel){
  Serial.write(128);
  Serial.write(pitch);
  Serial.write(vel);
}
void sendPitchBend(byte pitchVal1, byte pitchVal2){
  Serial.write(224);
  Serial.write(pitchVal1);
  Serial.write(pitchVal2);
}
void loop() {
  //Velocity = map(analogRead(velocityPin), 0, 4095, 0, 127);
  bendVal = map(analogRead(bendPin), 0, 4095, 0, 127);
  if(abs(bendVal - lastBend)>80){
    sendPitchBend(0, bendVal);
  }
  bendVal = lastBend;
  for(int i=0;i<8;i++){
    touchVal[i] = touchRead(touchPins[i]);
    if((touchVal[i] < Threshold) && (touchStates[i] == 1)){
      sendNoteOn(60+i,Velocity);
      touchStates[i] = 0;
    }
    else if((touchVal[i] > Threshold) && touchStates[i] == 0){
      sendNoteOff(60+i,0);
      touchStates[i] = 1;
    }
    lastVal[i] = touchVal[i];
  }
  for(int j=0;j<4;j++){
    touchVal[j+8] = digitalRead(sensorPins[j]);
    if((touchVal[j+8] == 1) && (touchStates[j+8] == 1)){
      sendNoteOn(68+j,Velocity);
      touchStates[j+8] = 0;
    }
    else if((touchVal[j+8] == 0) && touchStates[j+8] == 0){
      sendNoteOff(68+j,0);
      touchStates[j+8] = 1;
    }
    lastVal[j+8] = touchVal[j+8];
  }

}

