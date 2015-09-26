/*
 * queue.c
 *
 *  Created on: Sep 26, 2015
 *      Author: kieutq
 */

#include "queue.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct queue_private
{
	int ID; // unique identify
	char * data;
};

int Queue_Init(Queue_t * queue, int queue_size, int data_size)
{

	return 0;
}
int Queue_Send(Queue_t * queue, void * data, int data_size)
{

	return 0;
}
int Queue_Receive(Queue_t * queue, void * data, int data_size)
{

	return 0;
}
int Queue_Length(Queue_t * queue)
{

	return 0;
}
