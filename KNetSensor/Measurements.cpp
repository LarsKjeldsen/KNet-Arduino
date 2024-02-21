// 
// 
// 

#include "Measurements.h"



uint32_t measure_voltage()
{
	// For 168/328 boards
	const long InternalReferenceVoltage = 1056L;  // Adjust this value to your boards specific internal BG voltage x1000
												  // REFS1 REFS0          --> 0 1, AVcc internal ref. -Selects AVcc external reference
												  // MUX3 MUX2 MUX1 MUX0  --> 1110 1.1V (VBG)         -Selects channel 14, bandgap voltage, to measure
	ADMUX = (0 << REFS1) | (1 << REFS0) | (0 << ADLAR) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0);


	delay(50);  // Let mux settle a little to get a more stable A/D conversion
				// Start a conversion  
	ADCSRA |= _BV(ADSC);
	// Wait for it to complete
	while (((ADCSRA & (1 << ADSC)) != 0));
	// Scale the value
	uint32_t results = (((InternalReferenceVoltage * 1023L) / ADC) + 5L) / 10L; // calculates for straight line value 
	analogReference(INTERNAL);  // Put back to internal ref.
	ADMUX = (INTERNAL << 6) | 8;
	return results;
}



int16_t measure_Internal_temp(int Temp_Pin)
{
	uint32_t reading = analogRead(Temp_Pin);
	delay(100);   // let mux settle
	reading = 0; // reset mesurement
	int i = num_measurements;

	while (i--)
		reading += analogRead(Temp_Pin);
	reading = reading / num_measurements;

	float tSensor = ((reading*(1.1 / 1024.0)) - 0.5) * 100;
	float error = 244e-6*(125 - tSensor)*(tSensor - -40.0) + 2E-12*(tSensor - -40.0) - 2.0;

	return (uint16_t)((tSensor - error) * 100);
}


KNetOneWire::KNetOneWire(eeprom_info_t this_node)
{
	ds = new OneWire(this_node.OneWirePin);
	sensors = new DallasTemperature(ds);
	DevAddr = new DeviceAddress[MAXONEWIRES];

	int error;
	unsigned long Timeout = millis() + 20000UL;

	do {
		sensors->begin();
		delay(100);
		error = 0;
		int i;
		for (i = 0; i < this_node.OneWireNum; i++)
		{
			if (!sensors->getAddress(DevAddr[i], i))
				error++;
		}
		printf_P(PSTR("OneWire error=%d i=%d\n"), error, i);
	} while (error != 0 && Timeout > millis());
}

int KNetOneWire::GetReading(int16_t readings[], int num)
{
	sensors->setWaitForConversion(true);
	sensors->setResolution(12);
	int error = 0;
	uint8_t ReadingCount = 0;
	uint8_t retry = 0;

	do
	{
		error = 0;
		sensors->requestTemperatures();

		for (int k = 0; k < num; k++)
		{
			float temp = sensors->getTempC(DevAddr[k]);

			if (temp != DEVICE_DISCONNECTED_C && temp != 85.0)  // 85 is a error or default value (very strange setup from Maxim...)
				readings[ReadingCount++] = temp * 100;
			else
			{
				error++;
				readings[ReadingCount++] = 0;
			}
		}
		if (error != 0)
		{
			printf_P(PSTR("Failed in OneWire conversion #errors = %d\r\n"), error);
			ReadingCount = 0;
		}

		delay(250);

	} while (error != 0 && retry++ < 10);

	return ReadingCount;
}

KNetOneWire::~KNetOneWire()
{
	delete ds;
	delete sensors;
	delete DevAddr;
}