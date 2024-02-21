#include <Sync.h>
#include <RF24Network_config.h>
#include <RF24Network.h>
#include <SPI.h>
#include <RF24_config.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <LowPower.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <DHT.h>

#define DHT11_PIN A0
#define ONEWIRE_PIN A2

const int led_red = 3;
const int led_yellow = 4;
const int led_green = 5;

// Pins for radio
const int rf_ce = 8;
const int rf_csn = 7;
const int waketimer = 3000;  // 10 sek.
unsigned long prev_milis = 0;

RF24 radio(rf_ce, rf_csn);
RF24Network network(radio);


dht DHT;
OneWire  ds(ONEWIRE_PIN);
DallasTemperature sensors(&ds);

DeviceAddress DevAddr[10];

void setup()
{
	pinMode(led_green, OUTPUT);
	pinMode(led_red, OUTPUT);
	pinMode(led_yellow, OUTPUT);
	Serial.begin(115200);
	Serial.println("Starting test");

	digitalWrite(led_red, HIGH);
	digitalWrite(led_green, HIGH);


	radio.begin();
	sensors.begin();

	// locate devices on the bus
	Serial.print("Locating devices...");
	Serial.print("Found ");
	Serial.print(sensors.getDeviceCount(), DEC);
	Serial.println(" devices.");

	// report parasite power requirements
	Serial.print("Parasite power is: ");
	if (sensors.isParasitePowerMode()) Serial.println("ON");
	else Serial.println("OFF");

	for (int i = 0; i < sensors.getDeviceCount(); i++)
	{
		sensors.getAddress(DevAddr[i], i);
		Serial.print("Addr : "); Serial.flush();
		printAddress(DevAddr[i]);
		Serial.println();
	}
	prev_milis = millis();
}


void loop()
{
	int a0 = analogRead(A0);
	int d2 = digitalRead(2);

	Serial.print("DHT11, \t");
	int chk = DHT.read11(DHT11_PIN);

	// DISPLAY DATA
	Serial.print(DHT.humidity, 1);
	Serial.print(",\t");
	Serial.print(DHT.temperature, 1);

	sensors.requestTemperatures();
	for (int i = 0; i < sensors.getDeviceCount(); i++)
	{
		Serial.print(",\tTemp=");
		Serial.print(sensors.getTempC(DevAddr[i]));
		Serial.print(",\tRes=");
		Serial.print(sensors.getResolution(DevAddr[i]));
	}
	delay(9000);

	Serial.println("PowerDown Radio\n");
	digitalWrite(led_red, LOW);
	radio.powerDown();
	Serial.println("PowerDown Arduino\n");
	digitalWrite(led_green, LOW);
	Serial.flush();

	while (waketimer < (millis() - prev_milis))
		prev_milis += waketimer;  // sleep time parsed, skip to next waketime.

	SleepMilis(waketimer - (millis() - prev_milis));
	prev_milis = millis();

	digitalWrite(led_green, HIGH);
	Serial.println("Power Up Arduino\n");
	radio.powerUp();
	digitalWrite(led_red, HIGH);
}


// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		// zero pad the address if necessary
		if (deviceAddress[i] < 16) Serial.print("0");
		Serial.print(deviceAddress[i], HEX);
	}
}


void SleepMilis(uint16_t mil)
{
	if (mil >= 8000)
	{
		Serial.println("Sleeping for 8 sec"); Serial.flush();
		LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
		SleepMilis(mil - 8000);
	}
	else if (mil >= 4000)
	{
		Serial.println("Sleeping for 4 sec"); Serial.flush();
		LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
		SleepMilis(mil - 4000);
	}
	else if (mil >= 2000)
	{
		Serial.println("Sleeping for 2 sec"); Serial.flush();
		LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
		SleepMilis(mil - 2000);
	}
	else if (mil >= 1000)
	{
		Serial.println("Sleeping for 1 sec"); Serial.flush();
		LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
		SleepMilis(mil - 1000);
	}
	else if (mil >= 500)
	{
		Serial.println("Sleeping for .5 sec"); Serial.flush();
		LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
		SleepMilis(mil - 500);
	}
	else if (mil >= 120)
	{
		Serial.println("Sleeping for 0.12 sec"); Serial.flush();
		LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_OFF);
		SleepMilis(mil - 120);
	}
	else if (mil >= 30)
	{
		Serial.println("Sleeping for .03 sec"); Serial.flush();
		LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF);
		SleepMilis(mil - 30);
	}
}