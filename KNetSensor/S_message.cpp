#include <stdio.h>
#include <stdlib.h>
#include "S_message.h"


char S_message::buffer[80];
char* S_message::toString(void)
{
	snprintf(buffer, sizeof(buffer), "%2u.%02uV %ulost %3i.%02u %2i.%02u %2i.%02u %2i.%02u %2i.%02u %2i.%02u %2i.%02u %2i.%02u %2i.%02u",
		voltage_reading / 100, voltage_reading % 100,
		lost_packets,
		readings[0] / 100, abs(readings[0] % 100),
		readings[1] / 100, abs(readings[1] % 100),
		readings[2] / 100, abs(readings[2] % 100),
		readings[3] / 100, abs(readings[3] % 100),
		readings[4] / 100, abs(readings[4] % 100),
		readings[5] / 100, abs(readings[5] % 100),
		readings[6] / 100, abs(readings[6] % 100),
		readings[7] / 100, abs(readings[7] % 100),
		readings[8] / 100, abs(readings[8] % 100)
		);
	return buffer;
}
/****************************************************************************/
// vim:cin:ai:sts=2 sw=2 ft=cpp
