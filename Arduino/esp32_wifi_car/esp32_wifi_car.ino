#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include "index.h"

const char* host = "car";
const char* ssid = "ESP32 CAR";
const char* pass = "";

const int motorPins[4] = {13,12,14,27};

WebServer server(80);
WebSocketsServer ws = WebSocketsServer(81);

void wsEvent(byte num, WStype_t type, uint8_t* payload, size_t len) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("Client " + String(num) + "disconnected.");
      break;
    case WStype_CONNECTED:
      Serial.println("Client " + String(num) + " connected.");
      break;
    case WStype_TEXT:
      StaticJsonDocument<200> doc;
      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      } else {
        const int l_num = doc["num"];
        switch(l_num){
          case 0: // UP
          digitalWrite(motorPins[0],1);
          digitalWrite(motorPins[1],0);

          digitalWrite(motorPins[2],1);
          digitalWrite(motorPins[3],0);
          break;
        case 1: // RIGHT
        for(int i=0;i<2;i++){
          digitalWrite(motorPins[i],0);
        }
        digitalWrite(motorPins[2],1);
        digitalWrite(motorPins[3],0);
        break;
        case 2: // DOWN
        digitalWrite(motorPins[0],0);
        digitalWrite(motorPins[1],1);

        digitalWrite(motorPins[2],0);
        digitalWrite(motorPins[3],1);
        break;
        case 3: // LEFT
        digitalWrite(motorPins[0],1);
        digitalWrite(motorPins[1],0);
        for(int i=2; i<4;i++){
          digitalWrite(motorPins[i],0);
        }
        break;
        case 5:
        for(int i=0;i<4;i++){
          digitalWrite(motorPins[i],0);
          break;
        }
        }
      }

      Serial.println("");
  }
}

void sendJSON(String val){
    String jsonString = "";
    StaticJsonDocument<200> doc;
    JsonObject object = doc.to<JsonObject>();
    object["val"] = val;
    serializeJson(doc, jsonString);
    ws.broadcastTXT(jsonString);
}

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
  WiFi.softAP(ssid,pass);
  if(!MDNS.begin(host)){
    //http://<hostname>.local
    Serial.println("Error setting up mDNS responder!");
    while(1){
      delay(1000);
    }
  }
  Serial.println("mDns responder started");
  for(int i=0; i<4;i++){
    pinMode(motorPins[i], OUTPUT);
  }
  Serial.print("Connected to WiFi with IP adrdress: ");
  Serial.println(WiFi.localIP());

  // Serve the specified HTML pages
  server.on("/", []() {
    server.send(200, "text/html", String(homepage));
  });

  server.begin();

  ws.begin();
  ws.onEvent(wsEvent);
}

void loop() {
  // put your main code here, to run repeatedly:
server.handleClient();
ws.loop();
delay(30);
}
