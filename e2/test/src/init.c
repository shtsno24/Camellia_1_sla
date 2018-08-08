/*
 * init.c
 *
 *  Created on: 2017/10/01
 *      Author: shts
 */

#include "iodefine.h"
#include "init.h"
#include "util.h"
#include "CMT.h"
#include "sensor.h"
#include "MTU.h"
#include "motor.h"
#include "map.h"
#include "LED.h"
#define round(A)((int)(A + 0.5))


extern SEN r_sen, cr_sen, l_sen, cl_sen;
extern MOT r_motor, l_motor;
extern MAP map;



void init_ALL(void) {
	init_CPU();
	init_IO();
	init_Spec();
	init_Vehicle();
	init_Buzzer();
	init_Sensor();
	init_Motor();
	init_CMT();
	init_LED();
	init_MTU();
	init_ADC();
	init_Map();
}
