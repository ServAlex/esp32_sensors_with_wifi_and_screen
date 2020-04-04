
/////// display includes and defines
#include "display_part.h"
///////

#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <BH1750FVI.h>

Adafruit_SHT31 sht31 = Adafruit_SHT31();

// Create the Lightsensor instance
//BH1750FVI LightSensor(BH1750FVI::k_DevModeContHighRes);
BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);

/////// WiFi
// create this file with 2 const char* variables: ssid and password
#include "networkCredentials.h"
/////// buttons
#include "buttons_part.h"
////////

float temperature = 0;
float humidity = 0;

uint16_t lux = 0;
int lux_index = 0;

int intensity_min = 8;
int intensity_max = 60;
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
    report();

    buttonsSetup();

    sht31.begin(0x44);
    LightSensor.begin();
}

void button1PresHandler()
{
    //Serial.println("button 1 in main");
}
void button2PresHandler()
{
    //Serial.println("button 2 in main");
}
void bothButtonsPressHandler()
{
    //Serial.println("both buttons in main");
}

void loop()
{
    temperature = sht31.readTemperature();
	humidity = sht31.readHumidity();

    if(isnan(temperature))
		temperature = 0;

	if(isnan(humidity))
		humidity = 0;

    lux = LightSensor.GetLightIntensity();
	int normalizer = 5;
	float unscaled_lux = log(lux+normalizer)/log(1.2);
	lux_index = (int)unscaled_lux;
	//lux_index = (int)((unscaled_lux-intensity_min)/(intensity_max - intensity_min)*(SCREEN_HEIGHT - 11));

    //Serial.println(String(temperature) + " - " + String(humidity));
    report();

    delay(1000/15.f);
}
