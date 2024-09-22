class LED{
  private:
    byte pin;
  public:
  LED(){} // don't use
  LED(byte pin){
    this->pin = pin; 
  }
  void init(){
    pinMode(pin,OUTPUT);
  }
  void init(byte defaultState){
    init();
    if(defaultState == 1){
      on();
    }else{
      off();
    }
  }
  void on(){
    digitalWrite(pin,1);
  }
  void off(){
    digitalWrite(pin,0);
  }
};

LED led(18);

void setup() {
  // put your setup code here, to run once:
  led.init(0);
}

void loop() {
  // put your main code here, to run repeatedly:
  led.on();
  delay(500);
  led.off();
  delay(500);
}
