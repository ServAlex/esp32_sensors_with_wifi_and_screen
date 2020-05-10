#ifndef wifi_part_h
#define wifi_part_h

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
// create this file with 2 const char* variables: ssid and password
#include "networkCredentials.h"

String ip = "0.0.0.0";
WebServer server(80);

void handleNotFound()
{
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

void wifiSetup(void)
{
  WiFi.mode(WIFI_STA);

  int current = -1;
  int atemptsRemaining = ssids_count*6;

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED && atemptsRemaining-->0) {
    current = (current+1)%ssids_count;
    WiFi.begin(ssids[current], passwords[current]);
    Serial.print("\nAttempting ");
    Serial.println(ssids[current]);
    delay(2000);
  }

  Serial.print("\nConnected to ");
  Serial.println(ssids[current]);
  Serial.print("IP address: ");
  ip = WiFi.localIP().toString();
  Serial.println(ip);

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", [](){
    //String message = "endpoints: [temp, humid, lux]\n";
    String message = "<html><body>";
    message += "<a href=\"http://192.168.0.248/all\">All</a><br>";
    message += "<a href=\"http://192.168.0.248/temp\">Temperature</a><br>";
    message += "<a href=\"http://192.168.0.248/humid\">Humidity</a><br>";
    message += "<a href=\"http://192.168.0.248/lux\">Lux</a><br>";
    message += "<a href=\"http://192.168.0.248/co2\">CO2</a><br>";
    message += "<a href=\"http://192.168.0.248/tvoc\">Volotiles</a>";
    message += "\
   <h3>Normal CO<sub>2</sub> Levels</h3>\
   <p>The effects of CO<sub>2</sub> on adults at good health can be summarized to:</p>\
   <ul>\
      <li>normal outdoor level: <i>350 - 450 ppm</i></li>\
      <li>acceptable levels: <i>&lt; 600 ppm</i></li>\
      <li>complaints of stiffness and odors: <i>600 - 1000 ppm</i></li>\
      <li>ASHRAE and OSHA standards: <i>1000 ppm</i></li>\
      <li>general drowsiness: <i>1000 - 2500 ppm</i></li>\
      <li>adverse health effects may be expected: <i>2500 - 5000 ppm</i></li>\
      <li>maximum allowed concentration within a 8 hour working period: <i>5000 - 10000 ppm&nbsp;&nbsp;</i></li>\
      <li>maximum allowed concentration within a<i> 15 minute </i>working period:<i> <i>30000 ppm <br></i></i></li>\
   </ul>\
   <p>The levels above are quite normal and maximum levels may occasionally happen from time to time. In general - ventilation rates should keep carbon dioxide concentrations below <em>1000 ppm</em> to create indoor air quality conditions acceptable to most individuals.</p>\
   <h3>Extreme and Dangerous CO<sub>2</sub> Levels</h3>\
   <ul>\
      <li>slightly intoxicating, breathing and pulse rate increase, nausea: <em>30000 - 40000 ppm</em></li>\
      <li>above plus headaches and sight impairment: <i>50000 ppm</i></li>\
      <li>unconscious, further exposure death: <i>100000 ppm</i></li>\
   </ul>";
    message += "</body></html>";
    server.send(200, "text/html", message);
  });

  server.on("/all", [](){
    server.send(200, "text/plain", "Temperature: \t"+String(temperature)+
                                   "\nHumidity: \t"+String(humidity)+
                                   "\nLux: \t\t"+String(lux)+
                                   "\nCO2: \t"+String(co2)+
                                   "\ntvoc: \t"+String(tvoc)
                                   );
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

  server.on("/co2", [](){
    server.send(200, "text/plain", String(co2));
  });
  server.on("/tvoc", [](){
    server.send(200, "text/plain", String(tvoc));
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void wifiLoop(void) 
{
  if(WiFi.status() == WL_CONNECTED)
    server.handleClient();
}

#endif