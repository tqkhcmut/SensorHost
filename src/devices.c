/*
 * sensor.c
 *
 *  Created on: Sep 17, 2015
 *      Author: kieu
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "devices.h"
#include "serial.h"

#define DEV_HOST_NUMBER 4 // 4 USB interfaces
struct Device dev_host[DEV_HOST_NUMBER];

#ifdef __linux
#include <unistd.h>
#endif


#include <pthread.h>
pthread_t polling_thread;
pthread_mutex_t polling_control_access = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t serial_access = PTHREAD_MUTEX_INITIALIZER;


int getTypeLength(unsigned char type)
{
	switch (type)
	{
	case TYPE_BYTE:
		return 1; //sizeof(unsigned char);
		break;
	case TYPE_INT8:
		return 1; //sizeof(signed char);
		break;
	case TYPE_INT16:
		return 2; //sizeof(signed short);
		break;
	case TYPE_INT32:
		return 4; //sizeof(signed int);
		break;
	case TYPE_INT64:
		return 8; //sizeof(signed long);
		break;
	case TYPE_UINT16:
		return 2; //sizeof(unsigned short);
		break;
	case TYPE_UINT32:
		return 4; //sizeof(unsigned int);
		break;
	case TYPE_UINT64:
		return 8; //sizeof(unsigned long);
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

int getName(struct Device dev, char * buffer, int buffer_len)
{
	if (IS_SENSOR_TEMPERATURE(dev.type))
	{

	}
	else if (IS_SENSOR_ULTRA_SONIC(dev.type))
	{

	}
	else
	{

	}
	return 0;
}

int sendControl(struct Device dev)
{
	int packet_len = 3 + getTypeLength(dev.data_type);
	struct Packet * packet = malloc(packet_len);

	packet->id = dev.number + dev.type;
	packet->cmd = CMD_CONTROL;
	packet->data_type = dev.data_type;
	memcpy(packet->data, dev.data, packet_len - 3);

	Serial_SendMultiBytes((unsigned char *) packet, packet_len);
	return 0;
}

int queryData(struct Device * dev)
{
	int packet_len = 3 + getTypeLength(dev->data_type);
	struct Packet * packet = malloc(packet_len);

	packet->id = dev->number + dev->type;
	packet->cmd = CMD_QUERY;
	packet->data_type = dev->data_type;
	memcpy(packet->data, dev->data, packet_len - 3);

	Serial_SendMultiBytes((unsigned char *) packet, packet_len);
	return packet_len;
}

void * DevicePolling(void * host_number) // thread
{
	unsigned char poll_en = 0;
	unsigned char time_poll = 0;
	int host = (int) host_number;

	if (host >= DEV_HOST_NUMBER)
	{
		printf("Host number not valid.\r\nIt should be lester than %d.\r\n", DEV_HOST_NUMBER);
		printf("Thread exiting.\r\n");
		pthread_exit(NULL);
	}

	while(1)
	{
		if (pthread_mutex_trylock(&polling_control_access) == 0)
		{
			pthread_mutex_lock(&polling_control_access);
			poll_en = dev_host[host].polling_control.enable;
			time_poll = dev_host[host].polling_control.time_poll_ms * 1000;
			pthread_mutex_unlock(&polling_control_access);
		}

		if (poll_en)
		{
			if (dev_host[host].type != 0xff)
			{
				switch (dev_host[host].type & 0xf0)
				{
				case DEV_SENSOR_TEMPERATURE:
					break;
				case DEV_SENSOR_ULTRA_SONIC:
					break;
				case DEV_SENSOR_LIGTH:
					break;
				case DEV_RF:
					break;
				case DEV_BLUETOOTH:
					break;
				case DEV_BUZZER:
					break;
				case DEV_SENSOR_GAS:
					break;
				case DEV_SIM900:
					break;;
				case DEV_MY_THESIS:
					break;
				default:
					break;
				}
			}
			else
			{
				// query device
			}
			usleep(time_poll);
		}
	}
}
int Device_init(void)
{
	int i = 0;
	printf("Initial Sensor Host.\r\n");

	pthread_mutex_init(&polling_control_access, NULL);
	pthread_mutex_unlock(&polling_control_access);
	pthread_mutex_init(&serial_access, NULL);
	pthread_mutex_unlock(&serial_access);
	Serial_Init();
	for (i = 0; i < DEV_HOST_NUMBER; i++)
	{
		printf("Initial Sensor Host %d parameters.\r\n", i);
		dev_host[i].data_type = TYPE_FLOAT;
		dev_host[i].number = 0xff;	// unknown
		dev_host[i].type = 0xff;		// unknown
		dev_host[i].polling_control.enable = 0;
		dev_host[i].polling_control.time_poll_ms = 50;

		printf("Create thread poll for Sensor Host %d.\r\n", i);
		pthread_create(&polling_thread, NULL, &DevicePolling, (void *)i);
	}
	return 0;
}
int Device_startPooling(int host_number)
{
	while(pthread_mutex_trylock(&polling_control_access) != 0)
		usleep(100);

	pthread_mutex_lock(&polling_control_access);
	dev_host[host_number].polling_control.enable = 1;
	pthread_mutex_unlock(&polling_control_access);

	return 0;
}

int Device_stopPooling(int host_number)
{
	while(pthread_mutex_trylock(&polling_control_access) != 0)
		usleep(100);

	pthread_mutex_lock(&polling_control_access);
	dev_host[host_number].polling_control.enable = 0;
	pthread_mutex_unlock(&polling_control_access);

	return 0;
}
