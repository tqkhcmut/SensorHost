/*
 * sensor.c
 *
 *  Created on: Sep 17, 2015
 *      Author: kieu
 */

#include "sensor.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "serial.h"

#include <unistd.h>
#include <pthread.h>
pthread_t polling_thread;
pthread_mutex_t polling_control_access;

struct polling_control
{
	unsigned char enable;
	unsigned char time_poll_ms;
};
struct polling_control polling_control;

int getTypeLength(unsigned char type)
{
	switch (type)
	{
	case TYPE_BYTE:
		return sizeof(unsigned char);
		break;
	case TYPE_INT8:
		return sizeof(signed char);
		break;
	case TYPE_INT16:
		return sizeof(signed short);
		break;
	case TYPE_INT32:
		return sizeof(signed int);
		break;
	case TYPE_INT64:
		return sizeof(signed long);
		break;
	case TYPE_UINT16:
		return sizeof(unsigned short);
		break;
	case TYPE_UINT32:
		return sizeof(unsigned int);
		break;
	case TYPE_UINT64:
		return sizeof(unsigned long);
		break;
	case TYPE_FLOAT:
		return sizeof(float);
		break;
	case TYPE_DOUBLE:
		return sizeof(double);
		break;
	default:
		return 0;
		break;
	}
}

unsigned char checksum(char * packet)
{
	unsigned char checksum, packet_len, i;
	struct Packet * mypacket = (struct Packet *)packet;
	packet_len = getTypeLength(mypacket->data_type) + 3;
	for (i = 0; i < packet_len; i++)
		checksum += packet[i];
	checksum = !checksum + 1;
	return checksum;
}

int getName(struct Sensor sensor, char * buffer, int buffer_len)
{
	if (IS_SENSOR_TEMPERATURE(sensor.sensor_type))
	{

	}
	else if (IS_SENSOR_ULTRA_SONIC(sensor.sensor_type))
	{

	}
	else
	{

	}
	return 0;
}

int sendControl(struct Sensor sensor)
{
	int packet_len = 3 + getTypeLength(sensor.data_type);
	struct Packet * packet = malloc(packet_len);

	packet->id = sensor.sensor_number + sensor.sensor_type;
	packet->cmd = CMD_CONTROL;
	packet->data_type = sensor.data_type;
	memcpy(packet->data, sensor.data, packet_len - 3);

	Serial_SendMultiBytes((unsigned char *) packet, packet_len);
	return 0;
}

int queryData(struct Sensor * sensor)
{
	int packet_len = 3 + getTypeLength(sensor.data_type);
	struct Packet * packet = malloc(packet_len);

	packet->id = sensor.sensor_number + sensor.sensor_type;
	packet->cmd = CMD_QUERY;
	packet->data_type = sensor.data_type;
	memcpy(packet->data, sensor.data, packet_len - 3);

	Serial_SendMultiBytes((unsigned char *) packet, packet_len);
	return packet_len;
}

int Sensor_init(void)
{
	Serial_Init();
	return 0;
}
int * SensorPolling(void)
{
	unsigned char poll_en = 0;
	while(1)
	{
		if (pthread_mutex_trylock(polling_control_access) == 0)
		{
			pthread_mutex_lock(polling_control_access);
			poll_en = polling_control.enable;
			pthread_mutex_unlock(polling_control_access);
		}

		if (poll_en)
		{

		}
	}
	return 0;
}
int Sensor_startPooling(void)
{
	while(pthread_mutex_trylock(polling_control_access) != 0)
		usleep(100);

	pthread_mutex_lock(polling_control_access);
	polling_control.enable = 1;
	pthread_mutex_unlock(polling_control_access);

	return 0;
}

int Sensor_stopPooling(void)
{
	while(pthread_mutex_trylock(polling_control_access) != 0)
		usleep(100);

	pthread_mutex_lock(polling_control_access);
	polling_control.enable = 0;
	pthread_mutex_unlock(polling_control_access);

	return 0;
}
