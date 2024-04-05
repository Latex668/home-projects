/*
  Code for opening and closing an electronic gate with a relay.
  Made by Iana Alexandru 2023
  inspired from https://miliohm.com/mfrc522-rfid-reader-with-arduino-tutorial-the-simplest-way-to-read-rfid-tag/
  RC522 module pins:
  
  RST  - 9
  SDA  - 10
  MOSI - 11
  MISO - 12
  SCK  - 13
*/

#include <SPI.h>
#include <RFID.h>
#define SS 10
#define RST 9
RFID rfid(SS, RST);

String rfidCard; // variable to store read data
String tag1 = "115 205 154 145";
String tag2 = "163 177 137 145";
String tag3 = "97 192 173 16"  ;

int noteDuration = 250; // 1/4 notes
const byte buzzer = 5;
const byte relay  = 8;

void setup() {
  //Serial.begin(9600);
  pinMode(relay,  OUTPUT);
  pinMode(buzzer, OUTPUT);
  SPI.begin();
  rfid.init();
  digitalWrite(relay, 1);
}

void loop() {
  // If there's an rfid tag read its serial and store it in rfidCard
  if (rfid.isCard()) {
    if (rfid.readCardSerial()) {
      rfidCard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " + String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);
      //Serial.println(rfidCard);
      
      // Check if rfid is equal to any of the tags, turn on relay and make a sound else turn off relay and make another sound
      if ((rfidCard == tag1 || rfidCard == tag2 || rfidCard == tag3)){
        //Serial.println("access granted");
        digitalWrite(relay, 0  );
        delay(500              );
        analogWrite(buzzer, 494); // B4
        delay(noteDuration     );
        analogWrite(buzzer, 740); // F#5
        delay(noteDuration     );
        analogWrite(buzzer, 0  );
        digitalWrite(relay, 1  );
        delay(1000             ); 
      }
      else{ //Serial.println("access denied");
        analogWrite(buzzer, 494); // B4
        delay(noteDuration     );
        analogWrite(buzzer, 494); // B4
        delay(noteDuration     );
        analogWrite(buzzer, 0  );
        digitalWrite(relay, 1  );
        delay(1000             );
      }
    }
    rfid.halt();
  }
}