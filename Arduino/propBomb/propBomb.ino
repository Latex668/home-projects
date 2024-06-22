// code i made for a prop bomb 
#define SW 2
#define BUZ 4
int n = 10;
int tickfreq = 100;
bool bval = 0;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(SW, INPUT_PULLUP);
pinMode(BUZ, OUTPUT);
while(bval != digitalRead(SW)){}
}

void loop() {
  // put your main code here, to run repeatedly:
// For every pass, decrease it by 1 and also decrease the delay between ticks by n*100
kaboom(n);
if(n != 0){
  n--;
}
if(n > 1){
  analogWrite(BUZ, tickfreq);
  delay(n * 100);
  analogWrite(BUZ, 0);
  delay(n * tickfreq);
}
}
// Function that starts when n = 1 and makes ticking except for the last one faster
void kaboom(int n){
  if(n == 1){  
    for(int i = 0; i <= 8; i++){
      Serial.println(i);
      analogWrite(BUZ, 128);
      delay(100);
      if(i == 8){
        delay(1200);
      }
      analogWrite(BUZ, 0);
      delay(100);
    }
  }
}