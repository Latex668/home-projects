int A = 16;
int B = 17;


void setup() {
  // put your setup code here, to run once:
pinMode(A, OUTPUT);
pinMode(B, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
//analogWrite (10, 126);
digitalWrite(A, 1);
digitalWrite(B, 0);
delay(1000);
digitalWrite(A, 0);
digitalWrite(B, 1);
delay(1000);

}
