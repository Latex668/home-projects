// Cod facut pentru licenta. Vlad Diamandescu, Iana Alexandru 1/05/2024
#include <ESP32Servo.h>  // Folosim libraria esp32 servo in loc de servo.h pentru ca procesorul espressif nu este suportat pentru servo.h

// Librarii pentru wifi
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
// SSID si parola pentru wifi. Daca este folosit ca access point schimba-le.
const char* SSID = "Alex-home";
const char* PASS = "AlexIana2005";

// Pagina web pe care ruleaza site-ul
String webpage = "<!DOCTYPE html><html> <head> <title>fucking bitch site</title> <meta name='viewport' content='width=device-width, initial-scale=1.0'> <style> body{ background-color: #212121; color:gold; font-family:Arial; } button{ background-color: #212121; padding: 15px 15px; border:solid; border-radius:100vw; color:gold; text-decoration: none; font-size:xx-large; } button:hover{ background-color:gold; color:black; } .buttons{ position:relative; top:2vw; } </style> </head> <body> <h1>The motor direction is: <span id='direction'>-</span></h1> <h2>Speed of the motor is: <span id='speed'>-</span></h2> <h2>Servo degrees are: <span id='deg'>-</span>*</h2> <h2>Distance from object in front of car is: <span id='dis'>-</span> cm</h2> <div class='buttons'style='text-align:center; width:480px'> <button onclick='motorControl(1)'>UP</button><br> <button onclick='servoControl(1); motorControl(2)'>LEFT</button><button onclick='servoControl(2); motorControl(3)'>RIGHT</button><br> <button onclick='motorControl(4)'>DOWN</button><br><br><br> <button onclick='speedControl(0)'>-Speed</button><button onclick='motorControl(0)'>STOP</button><button onclick='speedControl(1)'>+Speed</button><br> <button onclick='motorControl(5)'>M3 Reverse</button><button onclick='motorControl(6)'>M3 Forward</button> </div> </body> <script> var Socket; let dirOut = document.getElementById('direction'); let deg = 45; let speed = 127; function init(){ Socket = new WebSocket('ws://' + window.location.hostname + ':81/'); Socket.onmessage = function(event){ processCommand(event); }; } function servoControl(degToInc){ if(degToInc == 1){ deg-=15; }else if(degToInc == 2){ deg+=15; } if(deg>90){ deg = 90; }else if(deg<0){ deg = 0; } let msg = { type: 'degrees', value: deg }; console.log(deg); Socket.send(JSON.stringify(msg)); } function speedControl(code){ if(code == 0){ speed-=25; }else if(code == 1){ speed+=25; } if(speed>255){ speed = 255; }else if(speed<0){ speed = 0; } let msg = { type: 'speed', value: speed }; Socket.send(JSON.stringify(msg)); } function motorControl(dir){ let msg = { type: 'direction', value: dir }; Socket.send(JSON.stringify(msg)); } function processCommand(event){ let obj = JSON.parse(event.data); let type = obj.type; if(type.localeCompare('direction') == 0){ let l_dir = parseInt(obj.value); if(l_dir == 0){ dirOut.innerHTML = 'STOP'; }else if(l_dir == 1){ dirOut.innerHTML = 'UP'; }else if(l_dir == 2){ dirOut.innerHTML = 'LEFT'; }else if(l_dir == 3){ dirOut.innerHTML = 'RIGHT'; }else if(l_dir == 4){ dirOut.innerHTML = 'DOWN'; }else if(l_dir == 5){ dirOut.innerHTML = 'M3 REVERSE'; }else if(l_dir == 6){ dirOut.innerHTML = 'M3 FORWARD'; } console.log(l_dir); } if(type.localeCompare('degrees') == 0){ let l_deg = parseInt(obj.value); document.getElementById('deg').innerHTML = obj.value; } if(type.localeCompare('speed') == 0){ let l_speed = parseInt(obj.value); document.getElementById('speed').innerHTML = obj.value; } if(type.localeCompare('distance') == 0){ let l_dis = parseInt(obj.value); document.getElementById('dis').innerHTML = obj.value; } } window.onload = function(event){ init(); } </script></html>";

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

TaskHandle_t Task1; // Creeaza task pentru a folosi si nucleul 0, in loc de a folosi un singur nucleu pentru tot programul

const int piniMotor[] = {22, 23, 16, 21};   // Seteaza pinii pentru driver motor
const int pinEnable[] = {13, 25};           // Pini pentru viteza (enable1, enable2)
const int pinSensor[] = {15, 12};           // Pini pentru senzor, primul este trigger, al doilea echo
#define pinServo 2                          // Pin-ul la care este conectat servo-ul
Servo servo;                                // Initializeaza servomotorul
int unghi = 45;                             // Seteaza unghiul servo-ului
int direction = 0;                          // Set initial direction to stop
int accel = 127;                            // Seteaza acceleratia initiala
#define speedSound 0.034                    // viteza sunetului in cm/us
long duration;
float distanceCm;

void setup() {
  Serial.begin(115200);
  servo.attach(pinServo);
  // Pentru fiecare item din lista piniMotor, seteaza-i ca iesire
  for (int i = 0; i <= sizeof(piniMotor); i++) {
    pinMode(piniMotor[i], OUTPUT);
  }
  for (int j = 0; j <= sizeof(pinEnable); j++) {
    pinMode(pinEnable[j], OUTPUT);
  }
  pinMode(pinSensor[0], OUTPUT); // Seteaza pinii senzorului ca iesire
  pinMode(pinSensor[1], OUTPUT);

            // Cod pentru WiFI ca acess point. Daca este folosit ca server in retea, comenteaza-l.
  // WiFi.mode(WIFI_AP);
  //   WiFi.softAP(SSID, PASS);
  //   Serial.print("[+] AP Created with IP Gateway ");
  //   Serial.println(WiFi.softAPIP());
            // Cod pentru WiFi in retea, incepe cu WiFi.begin si se termina la Serial.println(WiFi.localIP());
  WiFi.begin(SSID, PASS);
  Serial.println("Establishing connection to WiFi with SSID: " + String(SSID));

  while (WiFi.status() != WL_CONNECTED) {  // Cat timp nu e conectat printeaza niste puncte
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to wifi with IP address: ");
  Serial.println(WiFi.localIP());
    // Initializeaza esp-ul ca server
  server.on("/", []() {
    server.send(200, "text/html", webpage);  // Specifica ca trimitem text/html prin server
  });
  server.begin();

    //Initializeaza websocketul
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  xTaskCreatePinnedToCore(
    senzorUltrasonic,     // functia taskului
    "senzorUltrasonic",   // numele taskului
    10000,                // Stack size
    NULL,                 // Parametrul task-ului
    0,                    // Prioritatea task-ului
    &Task1,               // Tine cont de taskul creat
    0                     // Nucleul folosit
  );
}

void loop() {
  servo.write(unghi);
  server.handleClient();
  webSocket.loop();
}

void sendJSON(String type, String value) { // Functie pentru a trimite date pe pagina
  String jsonString = "";
  StaticJsonDocument<200> doc;
  JsonObject obj = doc.to<JsonObject>();
  obj["type"] = type;
  obj["value"] = value;
  serializeJson(doc, jsonString);
  webSocket.broadcastTXT(jsonString);
}

void senzorUltrasonic(void * parameter){
  for(;;){ // Creeaza o bucla infinita, am incercat while(true) dar imi tot dadea erori nu stiu de ce dar cu for merge lol
    digitalWrite(pinSensor[0], LOW); // Seteaza pinul de trigger sa fie low
    delayMicroseconds(2);
    digitalWrite(pinSensor[0], HIGH); // Trigger este HIGH pentru 10 uS
    delayMicroseconds(10);
    digitalWrite(pinSensor[0], LOW);

    duration = pulseIn(pinSensor[1], HIGH); // Durata este cat timp a durat pana pinul de echo a devenit high
    distanceCm = duration * speedSound/2;
    sendJSON("distance", String(distanceCm));
  }
}

void Directie(char directie, int viteza, int nrDriver) {
  // Primeste un caracter pentru setarea directiei motoarelor, primeste un int de la 0-255 pentru viteza si un int de la 1-2 pentru a determina care driver este folosit.
  // Cod pentru setarea viteza motorului. Daca nu se foloseste viteza, conecteaza jumper intre en1, en1 5v.
  analogWrite(pinEnable[0], viteza);
  analogWrite(pinEnable[1], viteza);
  if(nrDriver == 1){
    switch (directie) {
      case 'f':  // Daca directie == forward, seteaza motorul sa mearga in fata
        // Pentru primul motor:
        digitalWrite(piniMotor[0], LOW);
        digitalWrite(piniMotor[1], HIGH);
        // Pentru al doilea motor:
        digitalWrite(piniMotor[2], LOW);
        digitalWrite(piniMotor[3], HIGH);
        break;
      case 'b':  // Daca directia == backward, seteaza motorul sa mearga in spate
        // Pentru primul motor:
        digitalWrite(piniMotor[0], HIGH);
        digitalWrite(piniMotor[1], LOW);
        // Pentru al doilea motor:
        digitalWrite(piniMotor[2], HIGH);
        digitalWrite(piniMotor[3], LOW);
        break;
      case 's':  //Opreste miscarea
        digitalWrite(piniMotor[0], LOW);
        digitalWrite(piniMotor[1], LOW);
        digitalWrite(piniMotor[2], LOW);
        digitalWrite(piniMotor[3], LOW);
        break;
  }
  }else if(nrDriver == 2){
    switch (directie) {
    case 'f':  // Daca directie == forward, seteaza motorul sa mearga in fata
        // Pentru primul motor:
        digitalWrite(piniMotor[4], LOW);
        digitalWrite(piniMotor[5], HIGH);
        break;
      case 'b':  // Daca directia == backward, seteaza motorul sa mearga in spate
        // Pentru primul motor:
        digitalWrite(piniMotor[4], HIGH);
        digitalWrite(piniMotor[5], LOW);

        break;
      case 's':  //Opreste miscarea
        digitalWrite(piniMotor[4], LOW);
        digitalWrite(piniMotor[5], LOW);
        break;
    }
  }
}


void webSocketEvent(byte num, WStype_t type, uint8_t* payload, size_t length) { // Functie pentru a procesa ce se intampla in websocket
  switch (type) {
    case WStype_DISCONNECTED:           // Verifica daca clientul s-a deconectat
      Serial.println("Client " + String(num) + " disconnected.");
    case WStype_CONNECTED:              // Verifica daca clientul s-a conectat
      Serial.println("Client " + String(num) + "  connected.");
      sendJSON("direction", String(direction));
      break;
    case WStype_TEXT:               // Proceseaza raspunsul de la client
      StaticJsonDocument<200> doc;  // Creeaza un container JSON
      DeserializationError error = deserializeJson(doc, payload); // Verifica daca este vreo eroare
      if (error) {
        Serial.println("Shit!");
        return;
      } else {                  // Daca nu exista vreo eroare, continua
        const char* type = doc["type"]; // Preia tipul si valoarea variabilelor de la site
        const int value = doc["value"];

        if (String(type) == "direction") { // Daca tipul este directie, actualizeaza valoarea lui in site si schimba directia motorului
          direction = int(value);
          sendJSON("direction", String(value));
          switch (direction) {
            case 0:
              Directie('s', accel, 1);
              break;
            case 1:
              Directie('f', accel, 1);
              break;
            case 4:
              Directie('b', accel, 1);
              break;
            case 5:
              Directie('b', 255, 2);
              delay(500);
              Directie('s', 255, 2);
              break;
            case 6:
              Directie('f', 255, 2);
              delay(500);
              Directie('s', 255, 2);
              break;
          }
          direction = 0;
        }

        if (String(type) == "speed") {  // Daca tipul este viteza, schimba valoarea lui in site si da-o mai departe in variabila acceleratie
          accel = int(value);
          sendJSON("speed", String(value));
        }

        if (String(type) == "degrees") {  // Lafel si pentru gradele servomotorului
          unghi = int(value);
          sendJSON("degrees", String(value));
        }
      }
      Serial.println("");
      break;
  }
}
