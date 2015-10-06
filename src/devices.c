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
#include "ishare.h"

#ifdef __linux
#include <unistd.h>
#include "wiringPi.h"
#endif


#include <pthread.h>

#define DEV_HOST_NUMBER 4 // 4 USB interfaces

pthread_t polling_thread[DEV_HOST_NUMBER];
struct Device dev_host[DEV_HOST_NUMBER];

// protect device host access
pthread_mutex_t device_control_access = PTHREAD_MUTEX_INITIALIZER;
// protect serial access
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
	int packet_len;
	struct Packet * packet = NULL;
	if (IS_MY_THESIS(dev->type))
	{
		packet_len = 3 + getTypeLength(dev->data_type);
		packet = malloc(packet_len);
	}
	else
	{
		packet_len = 3 + sizeof (struct ThesisData);
		packet = malloc(packet_len);
	}

	packet->id = dev->number + dev->type;
	packet->cmd = CMD_QUERY;
	packet->data_type = dev->data_type;
	memcpy(packet->data, dev->data, packet_len - 3);

	Serial_SendMultiBytes((unsigned char *) packet, packet_len);
	// sleep 1ms for timeout
	usleep(1000);
	packet_len = Serial_Available();
	if (packet_len)
	{
		Serial_GetData((char *) packet, packet_len);

		dev->data_type = packet->data_type;
		dev->type = packet->id & 0xf0;
		dev->number = packet->id & 0x0f;
		if (IS_MY_THESIS(packet->id))
		{
			dev->polling_control.time_poll_ms = 500;
			memcpy(dev->data, packet->data, sizeof (struct ThesisData));
		}
		else
		{
			memcpy(dev->data, packet->data, getTypeLength(packet->data_type));
		}
		if (packet != NULL)
			free(packet);
		return packet_len;
	}
	else
	{
		if (packet != NULL)
			free(packet);
		return 0;
	}
}

void * DevicePolling(void * host_number) // thread
{
	unsigned char poll_en = 0;
	unsigned int time_poll = 0;
	unsigned char destroy = 0;
	int host = (int) host_number;

	if (host >= DEV_HOST_NUMBER)
	{
		printf("Host number not valid.\r\nIt should be lester than %d.\r\n", DEV_HOST_NUMBER);
		printf("Thread exiting.\r\n");
		pthread_exit(NULL);
	}

	printf("Thread: %d start with host: %d.\n",
			(int)polling_thread[host], host);

	while(1)
	{
		if (pthread_mutex_trylock(&device_control_access) == 0)
		{
			poll_en = dev_host[host].polling_control.enable;
			time_poll = dev_host[host].polling_control.time_poll_ms * 1000;
			destroy = dev_host[host].polling_control.destroy;
			pthread_mutex_unlock(&device_control_access);
		}
		else
		{
			printf("Thread: %d. host: %d. Fail to access device control.\n",
					(int)polling_thread[host], host);
			usleep(1000);
		}

		if (destroy)
		{
			printf("Thread: %d. host: %d. Destroying.\n",
					(int)polling_thread[host], host);
			pthread_exit(NULL);
		}

		if (poll_en)
		{
			if (dev_host[host].type != 0xff)
			{
				if (pthread_mutex_trylock(&serial_access) == 0)
				{
					if (queryData(&dev_host[host]))
					{
						printf("Thread: %d. host: %d. Got data from device.\n",
								(int)polling_thread[host], host);
					}
					else
					{
						printf("Thread: %d. host: %d. No device here.\n",
								(int)polling_thread[host], host);

					}
					pthread_mutex_unlock(&serial_access);
				}
				else
				{
					printf("Thread: %d. host: %d. Fail to access serial port.\n",
							(int)polling_thread[host], host);
				}

				switch (dev_host[host].type & 0xf0)
				{
				case DEV_SENSOR_TEMPERATURE:
					printf("Thread: %d. host: %d. Temperature: %0.3f.\n",
												(int)polling_thread[host], host, *(float *)&(dev_host[host].data[0]));

					// adjust time polling

					// save to shared memory

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
					break;
				case DEV_MY_THESIS:
					break;
				default:
					break;
				}
			}
			else
			{
				printf("Thread: %d. host: %d. Unknown device, identifying.\n",
						(int)polling_thread[host], host);

				// query broadcast id to identify what it is
				// adjust time polling to 500 ms
				time_poll = 500000;

				if (pthread_mutex_trylock(&serial_access) == 0)
				{
					if (queryData(&dev_host[host]))
					{
						printf("Thread: %d. host: %d. Got data from client.\n",
								(int)polling_thread[host], host);
					}
					else
					{
						printf("Thread: %d. host: %d. No client here.\n",
								(int)polling_thread[host], host);

					}
					pthread_mutex_unlock(&serial_access);
				}
				else
				{
					printf("Thread: %d. host: %d. Fail to access serial port.\n",
							(int)polling_thread[host], host);
				}
			}
			usleep(time_poll);
		}// query device
	}
}
int Device_init(void)
{
	int i = 0;
	printf("Initial Sensor Host.\r\n");

	pthread_mutex_init(&device_control_access, NULL);
//	pthread_mutex_unlock(&device_control_access);
	pthread_mutex_init(&serial_access, NULL);
//	pthread_mutex_unlock(&serial_access);

	if (wiringPiSetup() != 0)
	{
		printf("Initial wiringPi fail.\n");
		die(-1);
	}
	printf("Initial wiringPi successful.\n");

	Serial_Init();
	for (i = 0; i < DEV_HOST_NUMBER; i++)
	{
		printf("Initial Sensor Host %d parameters.\r\n", i);
		dev_host[i].data_type = TYPE_FLOAT;
		dev_host[i].number = 0xff;	// unknown
		dev_host[i].type = 0xff;		// unknown
		dev_host[i].polling_control.enable = 0;
		dev_host[i].polling_control.time_poll_ms = 50;
		dev_host[i].polling_control.destroy = 0;

		printf("Create thread poll for Sensor Host %d.\r\n", i);
		pthread_create(&polling_thread[i], NULL, &DevicePolling, (void *)i);
	}
	return 0;
}
int Device_startPooling(int host_number)
{
	if (host_number < 1 || host_number > 4)
	{
		printf("Invalid Host Number. (1 - 4).\n");
		return -1;
	}
	while(pthread_mutex_trylock(&device_control_access) != 0)
		usleep(100);

//	pthread_mutex_lock(&device_control_access);
	dev_host[host_number-1].polling_control.enable = 1;
	pthread_mutex_unlock(&device_control_access);

	return 0;
}

int Device_stopPooling(int host_number)
{
	while(pthread_mutex_trylock(&device_control_access) != 0)
		usleep(100);

//	pthread_mutex_lock(&device_control_access);
	dev_host[host_number].polling_control.enable = 0;
	pthread_mutex_unlock(&device_control_access);

	return 0;
}

int Device_destroyAll(void)
{
	int i;
	while(pthread_mutex_trylock(&device_control_access) != 0)
		usleep(100);

//	pthread_mutex_lock(&device_control_access);
	for (i = 0; i < DEV_HOST_NUMBER; i++)
	{
		dev_host[i].polling_control.destroy = 1;
	}
	pthread_mutex_unlock(&device_control_access);

	return 0;
}
