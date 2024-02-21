// Measurements.h

#ifndef _MEASUREMENTS_h
#define _MEASUREMENTS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <DallasTemperature.h>
#include <OneWire.h>
#include "nodeconfig.h"
#endif

#define MAXONEWIRES 9

class KNetOneWire
{
	OneWire *ds;
	DallasTemperature *sensors;
	DeviceAddress *DevAddr;

public:
	KNetOneWire(eeprom_info_t this_node);
	int GetReading(int16_t readings[], int num);
	~KNetOneWire();
};

const int num_measurements = 64;


uint32_t measure_voltage();
int16_t measure_Internal_temp(int Temp_Pin);

