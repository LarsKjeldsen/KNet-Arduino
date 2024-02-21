/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#ifndef __S_MESSAGE_H__
#define __S_MESSAGE_H__

#include <stdint.h>

/**
 * Sensor message (type 'S') 
 */

struct S_message
{
	// Max payload = 22 Bytes
	//
	uint16_t voltage_reading; // Voltage in 1/100 volts
	int16_t readings[9];     // Sensor reading in 1/100C

	uint8_t NodeID;
	uint8_t lost_packets;
	static char buffer[];
	S_message(void) : voltage_reading(0) { for (uint8_t i = 0; i < (sizeof(readings) / sizeof(readings[0])); i++) readings[i] = 0; }
  char* toString(void);
};

#endif // __S_MESSAGE_H__
// vim:cin:ai:sts=2 sw=2 ft=cpp
