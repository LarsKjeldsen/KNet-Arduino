#include <Arduino.h>

#pragma once

class L_message
{
public:
	uint8_t LowIntensity;
	uint8_t HighIntensity;
	uint16_t OnDarkness;
	uint16_t OffDarkness;
	uint8_t TimeOutTime; // TimeOut in Sec
public:

	L_message();
	~L_message();
};

