/*
 * raspi_ext.c
 *
 *  Created on: Oct 7, 2015
 *      Author: kieutq
 */

#include "raspi_ext.h"
#include "wiringPi.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// global control parameter for threading
unsigned char led_mode[6], led_state[6];
int led_time_ms[6];

pthread_mutex_t raspi_ext_control_access = PTHREAD_MUTEX_INITIALIZER;

#define LED_RUN_PIN		24
#define LED_POWER_PIN	25
#define LED_HOST1_PIN	7
#define LED_HOST2_PIN	0
#define LED_HOST3_PIN	2
#define LED_HOST4_PIN	3
#define POWER_PIN		4 // to cutoff power
#define POWER_OFF_PIN	1

void LED_Thread(void * paramesters)
{
	for (;;)
	{
		if (led_time_ms[0] > 0)
		{
			switch (led_mode[0])
			{
			case LED_MODE_ON:
				pinWrite(LED_RUN_PIN, HIGH);
				led_state[0] = 1;
				break;
			case LED_MODE_OFF:
				pinWrite(LED_RUN_PIN, LOW);
				led_state[0] = 0;
				break;
			case LED_MODE_TOGGLE:
				break;
			default:
				break;
			}
			led_time_ms[0]--;
		}
		usleep(1000);
	}
}

int RaspiExt_Init(void)
{
	if (wiringPiSetup() != 0)
	{
		printf("Initial wiringPi fail.\n");
		return -1;
	}
	printf("Initial wiringPi successful.\n");

	pinMode(LED_RUN_PIN, OUTPUT);
	pinMode(LED_POWER_PIN, OUTPUT);
	pinMode(LED_HOST1_PIN, OUTPUT);
	pinMode(LED_HOST2_PIN, OUTPUT);
	pinMode(LED_HOST3_PIN, OUTPUT);
	pinMode(LED_HOST4_PIN, OUTPUT);
	pinMode(POWER_PIN, OUTPUT);
	pinMode(POWER_OFF_PIN, INPUT);

	return 0;
}

int RaspiExt_PowerOff(void)
{

	return 0;
}

int RaspiExt_LED_Run_Config(unsigned char mode, int time_ms)
{

	return 0;
}
int RaspiExt_LED_Power_Config(unsigned char mode, int time_ms)
{

	return 0;
}
int RaspiExt_LED_Hostx_Config(unsigned char mode, int time_ms, int host)
{

	return 0;
}
int RaspiExt_DS1307_GetTime(Time_t * t)
{

	return 0;
}
int RaspiExt_DS1307_SetTime(Time_t t)
{

	return 0;
}
int RaspiExt_DS1307_GetDate(Date_t * d)
{

	return 0;
}
int RaspiExt_DS1307_SetDate(Date_t d)
{

	return 0;
}
