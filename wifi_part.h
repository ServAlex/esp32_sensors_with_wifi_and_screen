#ifndef wifi_part_h
#define wifi_part_h

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
// create this file with 2 const char* variables: ssid and password
#include "networkCredentials.h"

WebServer server(80);

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: " + server.uri();
  message += "\nMethod: " + (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: " + server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void wifiSetup(void) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", [](){
    //String message = "endpoints: [temp, humid, lux]\n";
    String message = "<html><body>";
    message += "<a href=\"http://192.168.0.248/all\">All</a><br>";
    message += "<a href=\"http://192.168.0.248/temp\">Temperature</a><br>";
    message += "<a href=\"http://192.168.0.248/humid\">Humidity</a><br>";
    message += "<a href=\"http://192.168.0.248/lux\">Lux</a>";
    message += "</body></html>";
    server.send(200, "text/html", message);
  });

  server.on("/all", [](){
    server.send(200, "text/plain", "Temperature: \t"+String(temperature)+"\nHumidity: \t"+String(humidity)+"\nLux: \t\t"+String(lux));
  });
  server.on("/temp", [](){
    server.send(200, "text/plain", String(temperature));
  });
  server.on("/humid", [](){
    server.send(200, "text/plain", String(humidity));
  });
  server.on("/lux", [](){
    server.send(200, "text/plain", String(lux));
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void wifiLoop(void) {
  server.handleClient();
}


#endif