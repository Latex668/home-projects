#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJSON.h>
#include <WebSocketsServer.h>

const char* host = "esp32";
const char* SSID = "Alex-home";
const char* PASS = "AlexIana2005";

AyncWebServer server(80);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(SSID, PASS);
  Serial.println("Connecting to wifi")
  while(WiFi.status() != WL_Connected){
    delay(1000);
    Serial.println(.);
  }
  Serial.println("Connected to wifi with ssid and pass " + String(SSID) + String(PASS));
  Serial.println("IP Adress is: " + WiFi.localIP());

  if(!MDNS.begin(host)){
    Serial.println("Error setting up MDNS responder.");
    while(1){
      delay(1000);
    }
  }
  Serial.println("MDNS responder started.");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request){
    request->send();
  });
}

void loop() {
  // put your main code here, to run repeatedly:

}
