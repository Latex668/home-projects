#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

// SSID and Pass for wifi
const char* SSID = "Alex-home";
const char* PASS = "AlexIana2005";

int led[] = {5, 18, 19};

int brightness = 127;
int ledNumber = 0;

WebServer server(80);
WebSocketsServer ws = WebSocketsServer(81);


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
  webSocket.onEvent(wsEvent);
  analogWrite(led[ledNumber], brightness);
}

void loop() {
  server.handleClient();
  webSocket.loop();
}

void wsEvent(byte num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:  // If client disconnected, print it
      Serial.println("Client " + String(num) + " disconnected.");
      break;
    case WStype_CONNECTED:  // check if websocket client is connected
      Serial.println("Client " + String(num) + "  connected.");
      break;
    case WStype_TEXT:  // Check response from client
      StaticJsonDocument<200> doc;  // create a JSON container
      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
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
  ws.broadcastTXT(jsonString);
}