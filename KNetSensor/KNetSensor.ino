#include <BME280I2C.h>
#include <BME280.h>
#include <EthernetUdp.h>
#include <EthernetServer.h>
#include <EthernetClient.h>
#include <Ethernet.h>
#include <Dns.h>
#include <Dhcp.h>
#include <Wire.h>
#include "RF24Support.h"
#include "Measurements.h"
#include <RF24Mesh_config.h>
#include <RF24Mesh.h>
#include <RF24.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24_config.h>
#include <RF24Network_config.h>
#include <RF24Network.h>
#include <Sync.h>
#include <LowPower.h>
#include <DallasTemperature.h>
#include <dht.h>
#include <OneWire.h>
#include <avr/pgmspace.h>
#include "nodeconfig.h"
#include "S_message.h"
#include "L_message.h"
#include "C_Message_arduino.h"
#include "printf.h"
#include "sleep.h"
#include "timer.h"
#include "light.h"


#define TEMP_PIN 2
#define WAKETIMER 60000ul  // Test = 5000ul - PROD = 60000ul
#define FAILSAVE_LOST_PACKAGES 30

// Pins for radio
const int rf_ce = 8;
const int rf_csn = 7;
// Pins for status LED, or '0' for no LED connected
const int led_red = 3;
const int led_yellow = 4;
const int led_green = 5;
const int button_a = 6;

bool		
NeedNewConfig = true;
unsigned long waketimer = WAKETIMER;
unsigned long prev_millis = 0;
timer_t		send_timer(1000); // first reading after 1 sec.
uint16_t	lost_packets = 0;
uint8_t		ReadingCount = 0;

KNetOneWire	*onewire;
light *Light;
dht *DHT;
BME280I2C *BME;

RF24 radio(rf_ce, rf_csn);
RF24Network network(radio);

// Our node configuration 
eeprom_info_t this_node;

void setup(void)
{
	Serial.begin(115200);
	printf_begin();
	//
	// Set up board hardware
	//
	pinMode(led_green, OUTPUT);
	pinMode(led_red, OUTPUT);
	pinMode(led_yellow, OUTPUT);
	pinMode(button_a, INPUT_PULLUP);
	//
	// Pull node config out of eeprom, and flag a new config from master
	// Which node are we?
	digitalWrite(led_red, HIGH);

	if (digitalRead(button_a) == LOW)  // Key pressed reset nodeconfig
	{
		printf_P(PSTR("Reset pressed, node cleared\n"));
		this_node.clear();
		while (1)
		{
			nodeconfig_listen();
		}
	}

	this_node = nodeconfig_read();
	this_node.Relay = true;  // Do not go to sleep until we have received new config.



	digitalWrite(led_red, LOW);
	digitalWrite(led_yellow, HIGH);

	SPI.begin();

	printf_P(PSTR("Start radio\n"));

	radio.begin();
	radio.setChannel(92);
	radio.setPALevel(RF24_PA_MAX);
	radio.printDetails();
	radio.setAutoAck(true);

	network.begin(this_node.address);
	network.multicastLevel(1);

	this_node.printconfig();

	RequestConfig(network);

	if (this_node.OneWirePin)
	{
		onewire = new KNetOneWire(this_node);
	}

	if (this_node.DHTPin)
	{
		DHT = new dht();
	}

	if (this_node.BMPPin)
	{
		BME = new BME280I2C;
		BME->begin();
	}

	if (this_node.lamp != 0)
	{
		Light = new light(this_node);
	}

	// Sensors use the stable internal 1.1V voltage
	analogReference(INTERNAL);
	ADMUX = (INTERNAL << 6) | 8;

	digitalWrite(led_green, LOW);
	digitalWrite(led_red, LOW);
	digitalWrite(led_yellow, LOW);
}

void loop(void)
{
	//	network.update();
	network.update();

	while (network.available())
	{
		RF24NetworkHeader header;

		network.update();
		network.peek(header);

		// List settings message received
		if (header.type == 'l')
		{
			L_message l_message;
			// clear message from queue
			network.read(header, &l_message, sizeof(l_message));
			if (this_node.light)
			{
				RF24NetworkHeader header(0, 'L');
				Light->UpdateParameter(&l_message);
				header.to_node = 0;
				header.from_node = this_node.address;
				header.type = 'L';
				network.write(header, NULL, 0);  // Send back ack.
			}
		}
		else
		if (header.type == 'C')
		{
			C_message c_message;

			int r = network.read(header, &c_message, sizeof(c_message));

			printf_P(PSTR("New config received "));
			if (c_message.idUnit == this_node.NodeID)
			{
				this_node.UpdateNodeConfig(&c_message);
				nodeconfig_write(this_node);

				network.begin(this_node.address);

				NeedNewConfig = false;
				waketimer = WAKETIMER; // Retry in 60 sek.
				this_node.printconfig();
			}
			else
				printf_P(PSTR("Config message not for us (ID=%d)\n"), c_message.idUnit);

			//if (this_node.Relay)
			//	network.multicastRelay = true;
			//else
			//	network.multicastRelay = false;
		}
		else
		{
			S_message s_message;

			int r = network.read(header, &s_message, sizeof(s_message));

			printf_P(PSTR("Unknows message %c (%h)\n"), header.type, header.type);
		}
	}
	// If we are the kind of node that sends readings, 
	// AND it's time to send a reading 

	if ((this_node.Relay && send_timer) || !this_node.Relay)  // if not relay, then we are sleeping.
	{
		S_message s_message;
		ReadingCount = 0;

		// Read the DHT11 temparture and humidity sensor
		if (this_node.DHTPin)
		{
			int chk = DHT->read11(this_node.DHTPin);
			s_message.readings[ReadingCount++] = (int16_t)(DHT->temperature * 100.0);   // reading in 1/100 messaurements
			s_message.readings[ReadingCount++] = (int16_t)(DHT->humidity * 100.0);  // Read humidity
		}
		if (this_node.BMPPin)
		{
			float pres, humi, temp;

			BME->read(pres, temp, humi);

//				if (temp != 21,74 && pres != 696,4  && humi != 57,8
			if (isnan(temp) || isnan(pres) || isnan(humi))
			{
				BME->begin();  // Reset BME;
				BME->read(pres, temp, humi);  //reread data
			}
			
			//Serial.print("Temp=");    Serial.print(temp);
			//Serial.print(" - Pres="); Serial.print(pres);
			//Serial.print(" - Humi="); Serial.println(humi);

			s_message.readings[ReadingCount++] = (int)(temp * 100.0);
			s_message.readings[ReadingCount++] = (int)(pres / 10.0);
			s_message.readings[ReadingCount++] = (int)(humi * 100.0);

		}
		if (this_node.OneWirePin)
		{
			int ret = onewire->GetReading(&(s_message.readings[ReadingCount]), this_node.OneWireNum);
			ReadingCount += ret;
		}

		if (this_node.InternalTemp)
		{
			s_message.readings[ReadingCount++] = measure_Internal_temp(TEMP_PIN);
		}


		if (this_node.light)
		{
			s_message.readings[ReadingCount++] = Light->Lsensor;
			s_message.readings[ReadingCount++] = Light->movement * 100;
			s_message.readings[ReadingCount++] = Light->LightIntensity * 10;
		}

		s_message.lost_packets = min(lost_packets, 0xff);

		s_message.voltage_reading = measure_voltage();

		// By default send to the base
		s_message.NodeID = this_node.NodeID;

		printf_P(PSTR("Send to base %s - "), s_message.toString());

		//	WRITE data to Base
		RF24NetworkHeader header(0, 'S');
		bool ok = network.write(header, &s_message, sizeof(s_message));

		if (ok) {
			printf_P(PSTR("OK\n"));
			lost_packets = 0;
		}
		else
		{
			printf_P(PSTR("Error try multicast - "));
			ok = network.multicast(header, &s_message, sizeof(s_message), 0); //  Send directely to base.
			if (ok) {
				printf_P(PSTR("OK\n"));
				lost_packets = 0;
			}
			else
				printf_P(PSTR("Error\n"));
		}
		/*
		** if a leaf node and has config then go to sleep
		*/

		if (!this_node.Relay)  // Leaf nodes goes to sleep
		{
			printf("God night\n");
			radio.powerDown();
			Serial.flush();

			while (waketimer < (millis() - prev_millis))
				prev_millis += waketimer;  // sleep time parsed, skip to next waketime.

			SleepMilis(waketimer - (millis() - prev_millis));
			prev_millis = millis();
			radio.powerUp();

			printf("God morgning\n");
		}
		else
		{
			send_timer.interval = waketimer;
		}
	}

	if (this_node.light)
	{
		Light->CheckSensor();
	}
}
// vim:ai:cin:sts=2 sw=2 ft=cpp
