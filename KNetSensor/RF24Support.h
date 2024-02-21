// RF24Support.h

#ifndef _RF24SUPPORT_h
#define _RF24SUPPORT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <RF24.h>
#include <RF24Network.h>
#include "S_message.h"
#include "L_message.h"
#include "C_Message_arduino.h"
#include "nodeconfig.h"
#endif

extern eeprom_info_t this_node;

void RequestConfig(RF24Network network);