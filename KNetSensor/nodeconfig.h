/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */
#include "C_Message_arduino.h"


#ifndef __NODECONFIG_H__
#define __NODECONFIG_H__

#define CONFIG_OK 11
#define CONFIG_ERROR 12

// Additional info
struct eeprom_info_t
{
	uint8_t node_status = CONFIG_ERROR;
	uint16_t address; // RF24Network address in octal (0000-0777)
	uint8_t NodeID; // A Uniq address 1-255
	uint8_t Relay;  // Relay (true/false), relay do not sleep
	uint8_t InternalTemp;
	uint8_t OneWirePin;
	uint8_t OneWireNum;
	uint8_t DHTPin;
	uint8_t BMPPin;
	uint8_t SHTPin;
	uint8_t OPTPin1;
	uint8_t OPTPin2;
	uint8_t OPTPin3;
	uint8_t BarometerPin;
	uint8_t SoilHumidityPin;
	uint8_t light;
	uint8_t PIR;
	uint8_t lamp;
	uint8_t NumReadings;

	void UpdateNodeConfig(C_message *c_message)
	{
		NodeID = c_message->idUnit;
		address = (uint16_t)(c_message->address);
		Relay = c_message->Relay;
		InternalTemp = c_message->InternalTemp;
		OneWirePin = c_message->OneWirePin;
		OneWireNum = c_message->OneWireNum;
		DHTPin = c_message->DHTPin;
		BMPPin = c_message->BMPPin;
		SHTPin = c_message->SHTPin;
		OPTPin1 = c_message->OPTPin1;
		OPTPin2 = c_message->OPTPin2;
		OPTPin3 = c_message->OPTPin3;
		BarometerPin = c_message->BarometerPin;
		SoilHumidityPin = c_message->SoilHumidityPin;
		NumReadings = c_message->NumReadings;
		light = c_message->light;
		PIR = c_message->PIR;
		lamp = c_message->lamp;
	}

	void printconfig()
	{
		printf_P(PSTR("node : Node=%d, Address=0%o, relay=%d, lamp=%d, PIR=%d, light=%d, #=%d\n"),
			(int)NodeID, (int)address, (int)Relay, (int)lamp, (int)PIR, (int)light, (int)NumReadings);
	}



	eeprom_info_t()
	{
		clear();
	}

	bool isValid() const
	{
		return ((node_status == CONFIG_OK) && (address != 0xffff));
	}

	void clear()
	{
		node_status = CONFIG_ERROR;
		address = 1; // 0xffff;
		NodeID = 0;
	}
};

const eeprom_info_t& nodeconfig_read(void);
void nodeconfig_write(const eeprom_info_t&);
void nodeconfig_listen(void);

#endif // __NODECONFIG_H__

// vim:ai:cin:sts=2 sw=2 ft=cpp

