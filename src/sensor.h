/*
 * sensor.h
 *
 *  Created on: Sep 17, 2015
 *      Author: kieu
 */

#ifndef SENSOR_H_
#define SENSOR_H_

/* data type */
#define TYPE_BYTE		0x01
#define TYPE_INT8		0x02
#define TYPE_INT16		0x03
#define TYPE_INT32		0x04
#define TYPE_INT64		0x05
#define TYPE_UINT16		0x06
#define TYPE_UINT32		0x07
#define TYPE_UINT64		0x08
#define TYPE_FLOAT		0x09
#define TYPE_DOUBLE		0x0A

#define IS_TYPE_BYTE(x) 	((x & 0x0f) == TYPE_BYTE)
#define IS_TYPE_INT8(x) 	((x & 0x0f) == TYPE_INT8)
#define IS_TYPE_INT16(x) 	((x & 0x0f) == TYPE_INT16)
#define IS_TYPE_INT32(x) 	((x & 0x0f) == TYPE_INT32)
#define IS_TYPE_INT64(x) 	((x & 0x0f) == TYPE_INT64)
#define IS_TYPE_UINT16(x) 	((x & 0x0f) == TYPE_UINT16)
#define IS_TYPE_UINT32(x) 	((x & 0x0f) == TYPE_UINT32)
#define IS_TYPE_UINT64(x) 	((x & 0x0f) == TYPE_UINT64)
#define IS_TYPE_FLOAT(x) 	((x & 0x0f) == TYPE_FLOAT)
#define IS_TYPE_DOUBLE(x) 	((x & 0x0f) == TYPE_DOUBLE)
/* data type */

/* byte order */
#define BIG_ENDIAN		0x10
#define LITTLE_ENDIAN	0x20

#define IS_BIG_ENDIAN(x) ((x & 0xf0) == BIG_ENDIAN)
#define IS_LITTLE_ENDIAN(x) ((x & 0xf0) == LITTLE_ENDIAN)
/* byte order */

/* sensor type */
#define SENSOR_TEMPERATURE		0x10
#define SENSOR_ULTRA_SONIC		0x20

#define IS_SENSOR_TEMPERATURE(x) ((x & 0xf0) == SENSOR_TEMPERATURE)
#define IS_SENSOR_ULTRA_SONIC(x) ((x & 0xf0) == SENSOR_ULTRA_SONIC)
/* sensor type */

/* command */
#define CMD_CONTROL		0x01
#define CMD_QUERY		0x02
/* command */

struct Sensor
{
	unsigned char sensor_number;
	unsigned char sensor_type;

	unsigned char data_type;
	unsigned char data[];
};

struct Packet
{
	unsigned char id;	// include type and order in BCD number
	unsigned char cmd;
	unsigned char data_type;
	unsigned char data[]; // checksum number on the data byte number data_type
};

extern int Sensor_init(void);
extern int Sensor_startPooling(void);
extern int Sensor_stopPooling(void);

extern int getTypeLength(unsigned char type);
extern unsigned char checksum(char * packet);
extern int getName(struct Sensor sensor, char * buffer, int buffer_len);
extern int sendControl(struct Sensor sensor);
extern int queryData(struct Sensor * sensor);

#endif /* SENSOR_H_ */
