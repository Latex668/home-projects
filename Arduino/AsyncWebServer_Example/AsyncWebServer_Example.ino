#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <LittleFS.h>

#define interval 1000
unsigned long previousMillis = 0;

// Replace with your network credentials
const char* host = "esp32";
const char* ssid = "Alex-home";
const char* password = "AlexIana2005";


// Create AsyncWebServer object on port 80
AsyncWebServer server (80);
WebSocketsServer ws = WebSocketsServer(81);

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

void sendJson(){
  String jsonString = "";
  StaticJsonDocument<200> doc;
  JsonObject object = doc.to<JsonObject>();
  object["num1"] = getRandomNumber();
  serializeJson(doc, jsonString);
  ws.broadcastTXT(jsonString);
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Initialize littleFS
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }
  Serial.print("Local IP is: ");
  Serial.println(WiFi.localIP());

  if(!MDNS.begin(host)){
    Serial.println("Error setting up mDNS responder!");
    while(1){
      delay(1000);
    }
  }
  Serial.println("mDns responder started");

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html");
  });
  server.on("/site2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/site2.html", "text/html");
  });
  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.js", "text/javascript");
  });

  // Start server
  server.begin();
  ws.begin();
  ws.onEvent(wsEvent);
}
 
void loop(){
  ws.loop();
  unsigned long time = millis();
  if(time-previousMillis >= interval){
    sendJson();
    previousMillis = time;
  }
}