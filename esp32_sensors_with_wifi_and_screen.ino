
#include "display_part.h"
#include "sensors_part.h"
#include "buttons_part.h"

/////// WiFi
// create this file with 2 const char* variables: ssid and password
#include "networkCredentials.h"

/////// screen
void report()
{
  clearScreen();
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  drawStringWithOffset("Temp " + String(temperature), 0, -30);
  drawStringWithOffset("Humid " + String(humidity), 0, 0);
  drawStringWithOffset("Lux " + String(lux) + ", " + String(lux_index), 0, 30);

  //tft.fillCircle(35, tft.height()/2-30 + currentMenu*30, 3, TFT_GREEN);
  //tft.drawCircle(35, tft.height()/2-30 + currentMenu*30, 3, TFT_GREEN);
}
/////// screen

void setup()
{
    delay(1000);

    Serial.begin(115200);
    Serial.println("Start");

    displaySetup();
    buttonsSetup();
    sensorsSetup();

    report();
}

void button1PresHandler() { }
void button2PresHandler() { /*Serial.println("button 2 in main");*/ }
void bothButtonsPressHandler() { }

void loop()
{
    pollSensors();
    report();
    delay(1000/24.f);
}
