/*
 * logger.c
 *
 *  Created on: 2018/09/16
 *      Author: shts
 */
#include "typedefine.h"
#include "iodefine.h"
#include "serial.h"
#include "init.h"
#include "util.h"
#include "LED.h"
#include "CMT.h"
#include "sensor.h"
#include "MTU.h"
#include "motor.h"
#include "map.h"
#include "math.h"
#include "logger.h"
#include "string.h"

LOG logger;
extern MOT r_motor, l_motor;
extern VEH vehicle;
extern SEN r_sen, cr_sen, l_sen, cl_sen;
extern SPC spec;

void init_Logger() {
	/*
	 * Initialize logger
	 *
	 * size     : Size of the Logger
	 * field    : Number of monitored variable
	 * run      : 0 : stop logger
	 * 			  1 : standby logger
	 * 			  2 : start logger
	 * 			  When you want to run logger, you should be this flag "2".
	 * 		      Then logging data only one time.
	 * data_num : Point position of data
	 * time     : [ms]
	 *
	 * */
	int i, j;

	logger.size = 1;
	logger.field = 2;
	logger.run = 0;
	logger.data_num = 0;
	logger.time = 0;

	strcpy(logger.data_title[0], "l_motor.vel");
	strcpy(logger.data_title[1], "r_motor.vel");
//	strcpy(logger.data_title[2], "vehicle.tar_vel");
//	strcpy(logger.data_title[3], "vehicle.vel");
//	strcpy(logger.data_title[4], "vehicle.tar_rot_vel");
//	strcpy(logger.data_title[5], "vehicle.rot_vel");

	for (i = 0; i < logger.size; i++) {
		for (j = 0; j < logger.field; j++) {
			logger.data[i][j].uint16 = 0;
		}
	}
}

void tim_Logger() {
	/*
	 * timer for logger
	 * */
	if (logger.run == 1) {
		logger.time += 1;
		if (logger.time >= 500) {
			logger.run = 2;
		}
	}
}

void write_Logger() {
	/*
	 * write variables to logger.data
	 * */
	if (logger.run == 2 && logger.size > logger.data_num) {
		logger.data[logger.data_num][0].float16 = l_motor.vel;
		logger.data[logger.data_num][1].float16 = r_motor.vel;
//		logger.data[logger.data_num][2].float16 = vehicle.vel;
//		logger.data[logger.data_num][3].float16 = vehicle.tar_vel;
//		logger.data[logger.data_num][4].float16 = vehicle.rot_vel;
//		logger.data[logger.data_num][5].float16 = vehicle.rot_vel;
		logger.run = 1;
		logger.time = 0;
		logger.data_num += 1;
		return;
	} else {
		return;
	}
}

void dump_Logger() {
	/*
	 * dump all data from logger.data
	 * */
	int i, j;
	logger.data_num = 0;
	for (j = 0; j < logger.size; j++) {
		for (i = 0; i < logger.field; i++) {
			myprintf("%f,", logger.data[j][i].float16);
		}
		myprintf("\n");
	}
}

