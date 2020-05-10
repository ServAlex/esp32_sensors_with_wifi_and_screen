
#ifndef sensors_part_h
#define sensors_part_h

#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <BH1750FVI.h>
#include "SparkFunCCS811.h" //Click here to get the library: http://librarymanager/All#SparkFun_CCS811

//#define CCS811_ADDR 0x5B //Default I2C Address
#define CCS811_ADDR 0x5A //Alternate I2C Address
CCS811 mySensor(CCS811_ADDR);

Adafruit_SHT31 sht31 = Adafruit_SHT31();

// Create the Lightsensor instance
//BH1750FVI LightSensor(BH1750FVI::k_DevModeContHighRes);
BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);

float temperature = 0;
float humidity = 0;

uint16_t lux = 0;
int lux_index = 0;

int intensity_min = 8;
int intensity_max = 60;

int co2 = 0;
int tvoc = 0;

void sensorsSetup()
{
	Wire.begin(); //Inialize I2C Hardware

	mySensor.begin();
    sht31.begin(0x44);
    LightSensor.begin();
}

void pollSensors()
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

	if (mySensor.dataAvailable())
	{
		mySensor.readAlgorithmResults();
		co2 = mySensor.getCO2();
		tvoc = mySensor.getTVOC();
	}

    //Serial.println(String(temperature) + " - " + String(humidity));
}

#endif