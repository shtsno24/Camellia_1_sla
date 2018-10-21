/*
 * logger.h
 *
 *  Created on: 2018/09/16
 *      Author: shts
 */

#ifndef LOGGER_H_
#define LOGGER_H_

typedef union l_data{
	unsigned char flag;
	int int16;
	unsigned int uint16;
	float float16;
}L_DATA;

typedef struct log{
	unsigned char run;
	unsigned char field,size;
	unsigned char time,data_num;

	char data_title[6][20];
	L_DATA data[20][6];

}LOG;

void tim_Logger();

void init_Logger();

void write_Logger();

void dump_Logger();

#endif /* LOGGER_H_ */
