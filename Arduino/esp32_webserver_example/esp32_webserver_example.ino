#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>

#include "index.h"
#include "page2.h"

const char* host = "esp32";

const char* ssid = "Alex-home";
const char* password = "AlexIana2005";

WebServer server(80);
WebSocketsServer ws = WebSocketsServer(81);

#define INTERVAL 1000
unsigned long previousMillis = 0;

int getRandomNumber() {
  int number1 = random(0, 255);
  return number1;
}

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
        const int l_GPIO = doc["GPIO"];
        const bool l_GPIOState = doc["GPIOState"];
        Serial.println(String(l_GPIO) + " " + String(l_GPIOState));
        pinMode(l_GPIO, OUTPUT);
        digitalWrite(l_GPIO, l_GPIOState);
      }
      Serial.println("");
      break;
  }
}

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi with ssid " + String(ssid));
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  if(!MDNS.begin(host)){
    //http://<hostname>.local
    Serial.println("Error setting up mDNS responder!");
    while(1){
      delay(1000);
    }
  }
  Serial.println("mDns responder started");

  pinMode(13, INPUT_PULLUP);
  Serial.print("Connected to WiFi with IP adrdress: ");
  Serial.println(WiFi.localIP());

  // Serve the specified HTML pages
  server.on("/", []() {
    server.send(200, "text/html", String(homepage));
  });
  server.on("/site2", []() {
    server.send(200, "text/html", String(site2));
  });
  server.begin();

  ws.begin();
  ws.onEvent(wsEvent);
}

void loop() {
  // Your code can go here or be empty; the server is handled asynchronously
  server.handleClient();
  ws.loop();

  unsigned long time = millis();
  if (time - previousMillis >= INTERVAL)  {
    String jsonString = "";
    StaticJsonDocument<200> doc;
    JsonObject object = doc.to<JsonObject>();
    object["num1"] = getRandomNumber();
    object["but"] = !digitalRead(13);
    serializeJson(doc, jsonString);
    ws.broadcastTXT(jsonString);

    previousMillis = time;
  }
}
