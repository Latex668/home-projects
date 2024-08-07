#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

// SSID and Pass for wifi
const char* SSID = "Alex-home";
const char* PASS = "AlexIana2005";

String webpage = "<!DOCTYPE html><html> <head> <title>fucking bitch site</title> <style> body{ background-color: #212121; color:gold; } </style> </head> <body> <h1>Control the LEDs</h1> <form> <input type='radio' id='LED0' name='radio' onclick='changeLED(0)'> <label for='LED0'>LED0</label> <input type='radio' id='LED1' name='radio' onclick='changeLED(1)'> <label for='LED1'>LED1</label> <input type='radio' id='LED2' name='radio' onclick='changeLED(2)'> <label for='LED2'>LED2</label> </form> <input type='range' id='slider' min='0' max='255' value='127' onclick='brightness(slider.value)'><br> <label for='slider'>Value: <span id='sliderval'>-</span></label> </body> <script> var Socket; output = document.getElementById('sliderval'); slider = document.getElementById('slider'); function init(){ Socket = new WebSocket('ws://' + window.location.hostname + ':81/'); Socket.onmessage = function(event){ processCommand(event); }; } function changeLED(ledNumber){ let msg = { type: 'ledChanged', value: ledNumber }; Socket.send(JSON.stringify(msg)); } function brightness(intensity){ let msg = { type: 'brightness', value: intensity }; Socket.send(JSON.stringify(msg)); } function processCommand(event){ let obj = JSON.parse(event.data); let type = obj.type; if(type.localeCompare('brightness') == 0){ var ledIntensity = parseInt(obj.value); slider.value = ledIntensity; output.innerHTML = ledIntensity; console.log(ledIntensity); }else if(type.localeCompare('ledChanged') == 0){ var l_ledChanged = parseInt(obj.value); console.log(l_ledChanged); if(l_ledChanged == 0){ document.getElementById('LED0').checked = true; }else if(l_ledChanged == 1){ document.getElementById('LED1').checked = true; }else if(l_ledChanged == 2){ document.getElementById('LED2').checked = true; } } } window.onload = function(event){ init(); } </script></html>";

int led[] = {5, 18, 19};

int brightness = 127;
int ledNumber = 0;

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);


void setup() {
  for(int i; i<3; i++){
    pinMode(led[i], OUTPUT);
  }
  Serial.begin(115200);
  WiFi.begin(SSID, PASS);
  Serial.println("Establishing connection to WiFi with SSID: " + String(SSID));

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to wifi with IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", []() {
    server.send(200, "text/html", webpage);
  });
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  analogWrite(led[ledNumber], brightness);
}

void loop() {
  server.handleClient();
  webSocket.loop();
}

void webSocketEvent(byte num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:  // If client disconnected, print it
      Serial.println("Client " + String(num) + " disconnected.");
    case WStype_CONNECTED:  // check if websocket client is connected
      Serial.println("Client " + String(num) + "  connected.");
      sendJSON("brightness", String(brightness));
      sendJSON("ledChanged", String(ledNumber));
      break;
    case WStype_TEXT:  // Check response from client
      StaticJsonDocument<200> doc;  // create a JSON container
      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
        Serial.println("Shit!");
        return;
      } else {
        const char* type = doc["type"];
        const int value = doc["value"];
        Serial.println(String(type));
        Serial.println(String(value));
        
        if(String(type) == "brightness"){
          brightness = int(value);
          sendJSON("brightness", String(value));
          analogWrite(led[ledNumber], brightness);
        }
        if(String(type) == "ledChanged"){
          ledNumber = int(value);
          sendJSON("ledChanged", String(value));
          for(int i=0; i<3; i++){
            if(i == ledNumber){
              analogWrite(led[i], brightness);
            }else{
              analogWrite(led[i], 0);
            }
          }
        }
      }
      Serial.println("");
      break;
  }
}

void sendJSON(String type, String value){
  String jsonString = "";
  StaticJsonDocument<200> doc;
  JsonObject obj = doc.to<JsonObject>();
  obj["type"] = type;
  obj["value"] = value;
  serializeJson(doc, jsonString);
  webSocket.broadcastTXT(jsonString);
}