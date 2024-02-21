#include "light.h"

light::light(eeprom_info_t node)
{
	this_node = node;
	pinMode(this_node.light, INPUT);
	pinMode(this_node.PIR, INPUT);
	pinMode(this_node.lamp, OUTPUT);

}


light::~light()
{
}

void light::UpdateParameter(L_message *m)
{
	LowIntensity = m->LowIntensity;
	HighIntensity = m->HighIntensity;
	OnDarkness = m->OnDarkness;
	OffDarkness = m->OffDarkness;
	TimeOutTime = m->TimeOutTime;
}

void light::CheckSensor()
{
	Lsensor = analogRead(this_node.light); 
	movement = digitalRead(this_node.PIR);

	printf("Light Sensor = %d - Pir readout = %d\n", Lsensor, movement);

	if (Lsensor < OnDarkness)
		LightLow = true;
	else if (Lsensor > OffDarkness)
		LightLow = false;

	if (LightHigh && (millis() - TimeOut > TimeOutTime))
	{  
		// Clear LightHigh
		LightHigh = false;
	}


	if (LightLow && movement)
	{
		LightHigh = true;
		TimeOut = millis();

	}

	if (LightHigh)
	{
		if (LightIntensity < HighIntensity) LightIntensity += 5;
		if (LightIntensity > HighIntensity) LightIntensity--;
	}
	else if (LightLow)
	{
		if (LightIntensity < LowIntensity) LightIntensity += 1;
		if (LightIntensity > LowIntensity) LightIntensity--;
	}
	else
	{
		if (LightIntensity > 0) LightIntensity--;
	}

	analogWrite(this_node.lamp, LightIntensity);
	
	delay(50);
}