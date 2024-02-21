

#ifndef __C_MESSAGE_H__
#define __C_MESSAGE_H__

#include <Arduino.h>

/**
* Sensor message (type 'C')
*/

struct C_message
{
	// C_Message use for sending and requesting configuration
	// Max payload = 22 Bytes
	//
	uint8_t idUnit; //ID (0-255)
	uint16_t address; //RF24Network address 0000-0777.
	uint8_t Relay;
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

	void print()
	{
		printf("C_Message : Node=%d, Address=0%o, relay=%d, lamp=%d, PIR=%d, light=%d, #=%d\n",
			(int)idUnit, (int)address, (int)Relay, (int)lamp, (int)PIR, (int)light, (int)NumReadings);
	}
} __attribute__((packed));

#endif // __S_MESSAGE_H__
// vim:cin:ai:sts=2 sw=2 ft=cpp




