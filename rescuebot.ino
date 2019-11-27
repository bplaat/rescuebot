#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "config.h"

String html = "<h1>RescueBot</h1>";

ESP8266WebServer server(80);

void setup() {
    Serial.begin(9600);
    Serial.print("\nConnecting to ");
    Serial.print(wifi_ssid);
    Serial.println("...");
    WiFi.begin(wifi_ssid, wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nConnected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", []() {
        server.send(200, "text/html", html);
    });
    server.begin();
}

void loop() {
    server.handleClient();
}
