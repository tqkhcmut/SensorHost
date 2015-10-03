/*
 * sensor.h
 *
 *  Created on: Sep 17, 2015
 *      Author: kieu
 */

#ifndef DEVICES_H_
#define DEVICES_H_

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

/* devices type */
#define DEV_SENSOR_TEMPERATURE		0x10
#define DEV_SENSOR_ULTRA_SONIC		0x20
#define DEV_SENSOR_GAS				0x30
#define DEV_SENSOR_LIGTH			0x40
#define DEV_BUZZER					0x50
#define DEV_SIM900					0x60
#define DEV_RF						0x70
#define DEV_BLUETOOTH				0x80
// special device for my thesis, it have all temperature, lighting, and gas sensors
#define DEV_MY_THESIS					0x90

#define IS_SENSOR_TEMPERATURE(x) 	((x & 0xf0) == DEV_SENSOR_TEMPERATURE)
#define IS_SENSOR_ULTRA_SONIC(x) 	((x & 0xf0) == DEV_SENSOR_ULTRA_SONIC)
#define IS_SENSOR_GAS(x) 			((x & 0xf0) == DEV_SENSOR_GAS)
#define IS_SENSOR_LIGTH(x) 			((x & 0xf0) == DEV_SENSOR_LIGTH)
#define IS_BUZZER(x) 				((x & 0xf0) == DEV_BUZZER)
#define IS_SIM900(x) 				((x & 0xf0) == DEV_SIM900)
#define IS_RF(x) 					((x & 0xf0) == DEV_RF)
#define IS_BLUETOOTH(x) 			((x & 0xf0) == DEV_BLUETOOTH)
#define IS_MY_THESIS(x) 			((x & 0xf0) == DEV_MY_THESIS)
/* devices type */

/* command */
#define CMD_CONTROL		0x01
#define CMD_QUERY		0x02
/* command */

struct polling_control
{
	unsigned char destroy;
	unsigned char enable;
	unsigned char time_poll_ms;
};

struct Device
{
	unsigned char number;
	unsigned char type;

	struct polling_control polling_control;

	unsigned char data_type;
	unsigned char data[];
};

struct HostInfo
{
	unsigned char buzzer_available;
	unsigned char sim900_available;
	unsigned char rf_available;
};

struct Packet
{
	unsigned char id;	// include type and order in BCD number
	unsigned char cmd;
	unsigned char data_type;
	unsigned char data[]; // checksum number on the data byte number data_type
};

struct ThesisData
{
	float temperature;
	float lighting;
	float gas;
};

extern int Device_init(void);
extern int Device_startPooling(int host_number);
extern int Device_stopPooling(int host_number);
extern int Device_destroyAll(void);

extern int getTypeLength(unsigned char type);
extern unsigned char checksum(char * packet);
extern int getName(struct Device dev, char * buffer, int buffer_len);
extern int sendControl(struct Device dev);
extern int queryData(struct Device * dev);

#endif /* SENSOR_H_ */
