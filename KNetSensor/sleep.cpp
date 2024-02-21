#include "sleep.h" 


void SleepMilis(uint16_t mil)
{
	if (mil >= 8000)
	{
		LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
		SleepMilis(mil - 8000);
	}
	else if (mil >= 4000)
	{
		LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
		SleepMilis(mil - 4000);
	}
	else if (mil >= 2000)
	{
		LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
		SleepMilis(mil - 2000);
	}
	else if (mil >= 1000)
	{
		LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
		SleepMilis(mil - 1000);
	}
	else if (mil >= 500)
	{
		LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
		SleepMilis(mil - 500);
	}
	else if (mil >= 120)
	{
		LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_OFF);
		SleepMilis(mil - 120);
	}
	else if (mil >= 30)
	{
		LowPower.powerDown(SLEEP_30MS, ADC_OFF, BOD_OFF);
		SleepMilis(mil - 30);
	}
}

// vim:ai:cin:sts=2 sw=2 ft=cpp
