/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-web-server-multiple-pages
 */

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "index.h"

#define LED_PIN 18  // ESP32 pin GPIO18 connected to LED

const char *ssid = "Alex-home";     // CHANGE IT
const char *password = "AlexIana2005";  // CHANGE IT

AsyncWebServer server(80);

int LED_state = LOW;

int getRandomNumber(){
  int number1 = random(0,255);
  return number1;
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi with ssid " + String(ssid));
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("Connected to WiFi with IP adrdress: ");
  Serial.println(WiFi.localIP());

  // Serve the specified HTML pages
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Web Server: home page");
    String html = HTML_CONTENT_HOME;  // Use the HTML content from the index.h file
    request->send(200, "text/html", html);
  });

  server.on("/Number", HTTP_GET, [](AsyncWebServerRequest* request){
    String number = String(getRandomNumber());
    request->send(200,"text/plain", number);
  });

  server.begin();
}

void loop() {
  // Your code can go here or be empty; the server is handled asynchronously
}
