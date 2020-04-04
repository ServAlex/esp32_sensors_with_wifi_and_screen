
/////// display includes and defines
#include <TFT_eSPI.h>
#include <SPI.h>
#include "WiFi.h"
#include <Wire.h>
#include "esp_adc_cal.h"

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif

#define TFT_MOSI            19
#define TFT_SCLK            18
#define TFT_CS              5
#define TFT_DC              16
#define TFT_RST             23

#define TFT_BL          4  // Display backlight control pin
#define ADC_EN          14
#define ADC_PIN         34

#define BUTTON_1 0
#define BUTTON_2 35

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

char buff[512];
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

void clearScreen()
{
  tft.fillScreen(TFT_BLACK);
}

void writeCenter(String str)
{
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString(str, tft.width() / 2, tft.height() / 2);
}
/////// screen


void setup()
{
    delay(1000);

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    //  tft.setTextColor(TFT_WHITE);
    tft.setCursor(0, 0);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(2);

    if (TFT_BL > 0) { // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
        pinMode(TFT_BL, OUTPUT); // Set backlight pin to output mode
        digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    }

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

