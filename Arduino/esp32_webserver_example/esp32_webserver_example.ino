#include <WiFi.h>
#include <WebServer.h>

// SSID and PASS for wi-fi
const char* ssid = "Alex-home";
const char* pass = "AlexIana2005";

WebServer server(80); // server at port 80

String webpage = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  WiFi.begin(ssid, pass);
  Serial.println("Establishing connection to WiFi with SSID " + String(ssid));

  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }
  Serial.print("Connected to network with the IP Adress: ");
  Serial.println(WiFi.localIP());

  server.on("/", [](){
    server.send(200, "text\html", webpage);
  });
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}
