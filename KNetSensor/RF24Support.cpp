// 
// 
// 

#include "RF24Support.h"


void RequestConfig(RF24Network network)
{
	RF24NetworkHeader header(0, 'c');
	C_message c_message;
	int ok;

	//	header.from_node = this_node.address;
	c_message.idUnit = this_node.NodeID;
	c_message.address = this_node.address;

	ok = network.multicast(header, &c_message, sizeof(c_message), 0); //  Send directely to base.

	printf_P(PSTR("NodeID %d is Requesting config from=%03o - "), c_message.idUnit, header.from_node);
	if (!ok)
		printf_P(PSTR("FAILED\n"));
	else
		printf_P(PSTR("OK\n"));

	if (!this_node.Relay) // has to wait for reply, as non relay nodes gooes to sleep.
	{
		printf_P(PSTR("Waiting for reply\n"));
		unsigned long l = millis();

		while (millis() < l + 5000)  // Timeout after 5 sek
		{
			network.update();
			printf(".");
			while (network.available())
			{
				printf("#");
				RF24NetworkHeader header;
				network.peek(header);

				if (header.type == 'C')
				{
					this_node.Relay = true;
					return;
				}
			}
			delay(250);
		}
	}
}

