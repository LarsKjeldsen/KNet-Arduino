#include "nodeconfig.h"
#include <Arduino.h>
#include "L_message.h"


#pragma once
class light
{
private:
	eeprom_info_t this_node;

	int LowIntensity = 10;
	int HighIntensity = 255;
	int OnDarkness = 800;
	int OffDarkness = 900;
	int TimeOutTime = 10000;
	int LightLow = false;
	int LightHigh = false;
	unsigned long TimeOut;

public:
	int16_t Lsensor;
	int16_t movement = false;
	int16_t LightIntensity = 0;

	void UpdateParameter(class L_message *);
	light(eeprom_info_t this_node);
	~light();

	void CheckSensor();
};

