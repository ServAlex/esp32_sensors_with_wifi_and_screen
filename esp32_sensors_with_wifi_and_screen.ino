
#include "display_part.h"
#include "sensors_part.h"
#include "buttons_part.h"
#include "wifi_part.h"

void report()
{
  clearScreen();
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  //drawStringWithOffset("Temp " + String(temperature) + " | "+ String(temperature2), 0, -30-30);
  //drawStringWithOffset("Humid " + String(humidity) + " | " + String(humidity2), 0, 0-30);
  int d = 25;
  drawStringWithOffset("Temp " + String(temperature), 0, -2*d);
  drawStringWithOffset("Humid " + String(humidity), 0, -d);
  drawStringWithOffset("Lux " + String(lux) + ", " + String(lux_index), 0, 0);
  drawStringWithOffset("co2 " + String(co2), 0, d);
  drawStringWithOffset("tvoc " + String(tvoc), 0, 2*d);

  //tft.fillCircle(35, tft.height()/2-30 + currentMenu*30, 3, TFT_GREEN);
  //tft.drawCircle(35, tft.height()/2-30 + currentMenu*30, 3, TFT_GREEN);
}

void setup()
{
    delay(1000);

    Serial.begin(115200);
    Serial.println("Start");

    displaySetup();
    buttonsSetup();
    sensorsSetup();
    wifiSetup();

    clearScreen();
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    drawStringWithOffset("ip " + String(ip), 0, 0);
    delay(5000);

    report();
}

void button1PresHandler() { }
void button2PresHandler() { /*Serial.println("button 2 in main");*/ }
void bothButtonsPressHandler() { }

long lastTrigered = 0;

void loop()
{
    long time = millis();

    pollSensors();
    wifiLoop();

    if(time - lastTrigered > 1000/24.f)
    {
        report();
        lastTrigered = time;
    }
    //delay(1000/24.f);
}
