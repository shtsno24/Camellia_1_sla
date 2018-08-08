/*
 * LED.h
 *
 *  Created on: 2018/04/13
 *      Author: shts
 */

#ifndef LED_H_
#define LED_H_

enum status_LED_ch {
	Red = 0, Yerrow = 1, Green = 2, Rst_status_LED = 3
};

enum sensor_LED_ch {
	CR_LED = 0, R_LED = 1, CL_LED = 2, L_LED = 3
};

void init_LED(void);

void drv_Sensor_LED(char, char);

void drv_Status_LED(char, char);

#endif /* LED_H_ */
