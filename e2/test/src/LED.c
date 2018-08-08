/*
 * LED.c
 *
 *  Created on: 2018/04/13
 *      Author: shts
 */
#include "iodefine.h"
#include "LED.h"

void init_LED(void) {
	PE.DRL.BIT.B10 = 0;
	PE.DRL.BIT.B8 = 0;
	PE.DRL.BIT.B6 = 0;

	PE.DRL.BIT.B9 = 0;
	PE.DRL.BIT.B11 = 0;
	PE.DRL.BIT.B12 = 0;
	PE.DRL.BIT.B13 = 0;
}

void drv_Sensor_LED(char ch, char status) {
	/*
	 PE9 : sensor_right
	 PE11 : sensor_center_right
	 PE13 : sensor_center_left
	 PE12 : sensor_left
	 */
	switch (ch) {
	case R_LED:
		PE.DRL.BIT.B9 = status;
		break;
	case CR_LED:
		PE.DRL.BIT.B11 = status;
		break;
	case CL_LED:
		PE.DRL.BIT.B13 = status;
		break;
	case L_LED:
		PE.DRL.BIT.B12 = status;
		break;
	}
}

void drv_Status_LED(char ch, char status) {
	/*
	 * B8  : Red
	 * B6  : Yerrow
	 * B10 : Green
	 *
	 * */
	switch (ch) {
	case Red:
		PE.DRL.BIT.B8 = status;
		break;
	case Yerrow:
		PE.DRL.BIT.B6 = status;
		break;
	case Green:
		PE.DRL.BIT.B10 = status;
		break;
	case Rst_status_LED:
		PE.DRL.BIT.B8 = 0;
		PE.DRL.BIT.B6 = 0;
		PE.DRL.BIT.B10 = 0;
		break;
	}
}
