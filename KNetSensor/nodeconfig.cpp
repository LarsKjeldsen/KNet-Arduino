/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "RF24Network_config.h"
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include "nodeconfig.h"
#include "C_Message_arduino.h"
#include <Print.h>

 // Where in EEPROM is the address stored?
uint8_t* address_at_eeprom_location = (uint8_t*)10;

eeprom_info_t eeprom_info;

const eeprom_info_t& nodeconfig_read(void)
{
	eeprom_read_block(&eeprom_info, address_at_eeprom_location, sizeof(eeprom_info));
	printf_P(PSTR("EEPROM Address = %d/0%o\n"), eeprom_info.NodeID, eeprom_info.address);

	// Look for the token in EEPROM to indicate the following value is
	// a validly set node address 
	if (eeprom_info.isValid())
	{
		printf_P(PSTR("NodeID=%d\n"), eeprom_info.NodeID);
		eeprom_info.node_status = CONFIG_OK;
	}
	else
	{
		eeprom_info.clear();

		printf_P(PSTR("*** No valid address found.  Send node address via serial of the form 011N\n\r"));

		while (1)
		{
			nodeconfig_listen();
		}
	}
	return eeprom_info;
}


void nodeconfig_write(const eeprom_info_t &eeprom_info)
{
	eeprom_update_block(&eeprom_info, address_at_eeprom_location, sizeof(eeprom_info));
}


char serialdata[10];
char* nextserialat = serialdata;
const char* maxserial = serialdata + sizeof(serialdata) - 1;

void nodeconfig_listen(void)
{
	//
	// Listen for serial input, which is how we set the address
	//
	if (Serial.available())
	{
		// If the character on serial input is in a valid range...
		char c = tolower(Serial.read());
		printf_P(PSTR("%i: %c - %c - %c - %c\n"), maxserial - nextserialat, serialdata[0], serialdata[1], serialdata[2], serialdata[3]);

		if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))
		{
			*nextserialat++ = c;
			if (nextserialat == maxserial)
			{
				*nextserialat = 0;
				printf_P(PSTR("\r\n*** Unknown serial command: %s\r\n"), serialdata);
				nextserialat = serialdata;
			}
		}
		else if (c == 'n')
		{
			// Convert to octal
			char *pc = serialdata;
			uint16_t address = 0;
			while (pc < nextserialat)
			{
				address <<= 4;  // use 3 for octal addresses
				address |= (*pc++ - '0');
			}

			if (address > 0)
			{
				// It is our address
				memset(&eeprom_info, 0, sizeof(eeprom_info));
				eeprom_info.address = 1; // Default address
				eeprom_info.NodeID = (uint8_t)address;
				eeprom_info.node_status = CONFIG_OK;
				eeprom_update_block(&eeprom_info, address_at_eeprom_location, sizeof(eeprom_info));


				// And we are done right now (no easy way to soft reset)
				printf_P(PSTR("\n\rManually set to address %03o\n\rPowercycle to continue!"), address);
				while (1);
			}
			else
			{
				printf_P(PSTR("\nERROR: Cannot run on node 00\n"));

			}
		}
	}
}

inline void eeprom_update_block(const void *__src, void * __dst, size_t __n)
{
	bool flag = false;
	const uint8_t* __src_u = reinterpret_cast<const uint8_t*>(__src);
	uint8_t* __dst_u = reinterpret_cast<uint8_t*>(__dst);
	while (__n--)
	{
		if (eeprom_read_byte(__dst_u) != *__src_u) {
			eeprom_write_byte(__dst_u, *__src_u);
			flag = true;
		}
		++__src_u;
		++__dst_u;
	}
}



// vim:ai:cin:sts=2 sw=2 ft=cpp
