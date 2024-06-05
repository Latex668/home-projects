#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

// SSID and Pass for wifi
const char* SSID = "Alex-home";
const char* PASS = "AlexIana2005";

String webpage = "<!DOCTYPE html><html> <head> <title>fucking bitch site</title> <style> body{ background-color: #212121; color:gold; } </style> </head> <body> <h1>Generating random number</h1> <p>The numbers are: <span id='rand1'>-</span></p> <p><span id='rand2'>-</span></p> <button id='BUT' onclick='BUT_SEND()'>Send info</button> </body> <script> var Socket; function init(){ Socket = new WebSocket('ws://' + window.location.hostname + ':81/'); Socket.onmessage = function(event){ processCommand(event); }; } function BUT_SEND(){ var msg = { brand: 'Harley Benton', type: 'TE-20HH', year: 'idk', color: 'black' }; Socket.send(JSON.stringify(msg)); } function processCommand(event){ var obj = JSON.parse(event.data); document.getElementById('rand1').innerHTML = obj.rand1; document.getElementById('rand2').innerHTML = obj.rand2; console.log(obj.rand1); console.log(obj.rand2); } window.onload = function(event){ init(); } </script></html>";

int interval = 1000;
unsigned long previousMillis = 0;

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void setup() {
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
    server.send(200, "text\html", webpage);
  });
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  server.handleClient();
  webSocket.loop();

  unsigned long now = millis();
  if ((unsigned long)(now - previousMillis) > interval) {
    String JsonString = "";
    StaticJsonDocument<200> doc;                      // create a JSON container
    JsonObject obj = doc.to<JsonObject>();
    obj["rand1"] = random(255);
    obj["rand2"] = random(255);
    serializeJson(doc, JsonString);
    Serial.println(JsonString);

    webSocket.broadcastTXT(JsonString);
    previousMillis = now;
  }
}

void webSocketEvent(byte num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:  // If client disconnected, print it
      Serial.println("Client disconnected.");
    case WStype_CONNECTED:  // check if websocket client is connected
      Serial.println("Client connected.");
      break;
    case WStype_TEXT:  // Check response from client
      StaticJsonDocument<200> doc;                    // create a JSON container
      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
        Serial.println("Shit!");
        return;
      } else {
        const char* brand = doc["brand"];
        const char* type = doc["type"];
        const int year = doc["year"];
        const char* color = doc["color"];
        Serial.println("Received guitar info");
        Serial.println("Brand: " + String(brand));
        Serial.println("Type: " + String(type));
        Serial.println("Year: " + String(year));
        Serial.println("Color: " + String(color));
      }
      Serial.println("");
      break;
  }
}
