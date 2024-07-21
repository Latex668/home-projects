#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

// SSID and Password for wifi
const char* SSID = "Alex-home";
const char* PASS = "AlexIana2005";

// Webpage for the site
String webpage ="";

WebServer server(80); // Start server at port 80
WebsocketsServer webSocket = WebSocketsServer(81); // Start ws server at port 81

void setup(){
    Serial.begin(115200);
    WiFi.begin(SSID, PASS);
    Serial.println("Establishing connection to wifi with SSID: " + SSID);
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

}

void loop(){
    server.handleClient();
    webSocket.loop();
}

void sendJSON(String type, String value) { // Functie pentru a trimite date pe pagina
    String jsonString = "";
    StaticJsonDocument<400> doc;
    JsonObject obj = doc.to<JsonObject>();
    obj["type"] = type;
    obj["value"] = value;
    serializeJson(doc, jsonString);
    webSocket.broadcastTXT(jsonString);
}