
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
#define BUTTON_1 0
#define BUTTON_2 35

int repeatStart = 1000;
int repeatPeriod = 300;

// button 1
long button1_pressStartedTime = 0;
long button1_lastRepeatedTime = 0;
bool button1_isPressed = false;
// button 2
long button2_pressStartedTime = 0;
long button2_lastRepeatedTime = 0;
bool button2_isPressed = false;
TaskHandle_t core0Task;

void core0TaskCode( void * pvParameters )
{
    Serial.print("core 0 task is running on core ");
    Serial.println(xPortGetCoreID());

    for(;;)
    {
        //delay(10);
        readButtons();
    } 
}

void button1PresHandler()
{
}

void button2PresHandler()
{
}

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
  tft.drawString("Temp " + String(temperature), tft.width() / 2, tft.height() / 2 - 30);
  tft.drawString("Humid " + String(humidity), tft.width() / 2, tft.height() / 2);
  tft.drawString("Lux " + String(lux) + "/" + String(lux_index), tft.width() / 2, tft.height() / 2 + 30);
  //tft.drawString(String(testInt), tft.width() / 2, tft.height() / 2 + 30);

  //tft.fillCircle(35, tft.height()/2-30 + currentMenu*30, 3, TFT_GREEN);
  //tft.drawCircle(35, tft.height()/2-30 + currentMenu*30, 3, TFT_GREEN);
}

/////// screen


void setup()
{
    delay(1000);

    displaySetup();

    Serial.begin(115200);
    Serial.println("Start");
    report();
     
    xTaskCreatePinnedToCore(
                        core0TaskCode,   // Task function.
                        "core0Task",     // name of task.
                        10000,       // Stack size of task 
                        NULL,        // parameter of the task
                        0,           // priority of the task 
                        &core0Task,      // Task handle to keep track of created task 
                        0);          // pin task to core 0 

    sht31.begin(0x44);

    LightSensor.begin();
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

    Serial.println(String(temperature) + " - " + String(humidity));
    report();

    delay(1000/15.f);
}


///////////

void readButtons()
{
    long time = millis();

    int button1Val = digitalRead(BUTTON_1);
    int button2Val = digitalRead(BUTTON_2);

    if (button1Val != HIGH) 
    {
        // button pressed
        if(button1_isPressed)
        {
        // been pressed before
        if(time - button1_pressStartedTime > repeatStart && time - button1_lastRepeatedTime > repeatPeriod)
        {
            button1_lastRepeatedTime = time;
            button1PresHandler();
        }
        }
        else
        {
        // pressed for the first time
        button1_isPressed = true;
        button1_pressStartedTime = time;
        button1_lastRepeatedTime = 0;
        button1PresHandler();
        }
    }
    else
    {
        // button not pressed
        button1_isPressed = false;
        button1_pressStartedTime = 0;
        button1_lastRepeatedTime = 0;
    }
    
    if (button2Val != HIGH) 
    {
        // button pressed
        if(button2_isPressed)
        {
        // been pressed before
        if(time - button2_pressStartedTime > repeatStart && time - button2_lastRepeatedTime > repeatPeriod)
        {
            button2_lastRepeatedTime = time;
            button2PresHandler();
        }
        }
        else
        {
        // pressed for the first time
        button2_isPressed = true;
        button2_pressStartedTime = time;
        button2_lastRepeatedTime = 0;
        button2PresHandler();
        }
    }
    else
    {
        // button not pressed
        button2_isPressed = false;
        button2_pressStartedTime = 0;
        button2_lastRepeatedTime = 0;
    }
}

