/*
 * MTU.c
 *
 *  Created on: 2017/09/17
 *      Author: shts
 */
#include "iodefine.h"
#include "util.h"
#include "MTU.h"
#include "motor.h"
#include "LED.h"

#define round(A)((int)(A + 0.5))

extern SPC spec;
extern MOT r_motor, l_motor;
extern VEH vehicle;

void init_MTU(void) {

	STB.CR4.BIT._MTU2 = 0; //スタンバイ解除
	MTU2.TSTR.BYTE = 0x00;	//タイマストップ

	MTU20.TIER.BIT.TGIEA = 1; //enable interrupt
	MTU21.TIER.BIT.TGIEA = 1; //enable interrupt

	INTC.IPRD.BIT._MTU20G = 0xF; //set interrupt priority
	INTC.IPRD.BIT._MTU21G = 0xE; //set interrupt priority

	MTU20.TCR.BIT.TPSC = 1; //set clock 25/4=6.25MHz
	MTU21.TCR.BIT.TPSC = 1; //set clock 25/4=6.25MHz

	MTU20.TCNT = 0;	// reset counter
	MTU21.TCNT = 0;

	MTU20.TCR.BIT.CCLR = 1; //TGRAのコンペアマッチでTCNTクリア
	MTU20.TIOR.BIT.IOA = 1; //0→0
	MTU20.TIOR.BIT.IOB = 2; //0→1

	MTU21.TCR.BIT.CCLR = 1; //TGRAのコンペアマッチでTCNTクリア
	MTU21.TIOR.BIT.IOA = 1; //0→0
	MTU21.TIOR.BIT.IOB = 2; //0→1

	MTU2.TOER.BYTE = 0xFF; // permission of PWM output

	MTU20.TGRA = spec.pwm_base_clock; // set PWM clock
	MTU20.TGRB = spec.pwm_half_clock; // set duty

	MTU21.TGRA = spec.pwm_base_clock; // set PWM clock
	MTU21.TGRB = spec.pwm_half_clock; // set duty

	MTU20.TMDR.BIT.MD = 2; //PWMモード1
	MTU21.TMDR.BIT.MD = 2; //PWMモード1

}

void stop_MTU(char cst) {
//stop MTU2's timer
	switch (cst) {
	case cst0:
		MTU2.TSTR.BIT.CST0 = 0;
		break;
	case cst1:
		MTU2.TSTR.BIT.CST1 = 0;
		break;
	case cst2:
		MTU2.TSTR.BIT.CST2 = 0;
		break;
	case cst3:
		MTU2.TSTR.BIT.CST3 = 0;
		MTU2.TSTR.BIT.CST4 = 0;
		break;
	case cst_all:
		MTU2.TSTR.BIT.CST0 = 0;
		MTU2.TSTR.BIT.CST1 = 0;
		MTU2.TSTR.BIT.CST2 = 0;
		MTU2.TSTR.BIT.CST3 = 0;
		MTU2.TSTR.BIT.CST4 = 0;
		break;
	}
}

void start_MTU(char cst) {
//start MTU2's timer
	switch (cst) {
	case cst0:
		MTU2.TSTR.BIT.CST0 = 1;
		break;
	case cst1:
		MTU2.TSTR.BIT.CST1 = 1;
		break;
	case cst2:
		MTU2.TSTR.BIT.CST2 = 1;
		break;
	case cst3:
		MTU2.TSTR.BIT.CST3 = 1;
		MTU2.TSTR.BIT.CST4 = 1;
		break;
	case cst_all:
		MTU2.TSTR.BIT.CST0 = 1;
		MTU2.TSTR.BIT.CST1 = 1;
		MTU2.TSTR.BIT.CST2 = 1;
		MTU2.TSTR.BIT.CST3 = 1;
		MTU2.TSTR.BIT.CST4 = 1;
		break;
	}
}

void change_Duty_MTU20(void) {
	/*
	 * this function operates R_motor
	 * write this function to interrupt_handlers.c
	 * */

//	stop_MTU(cst0);
//	r_motor.duty = (int) (spec.step_dist / r_motor.vel * 12500000);
//	r_motor.duty = (int) (spec.step_dist / r_motor.vel * 25000000);
	if (r_motor.vel == 0.0) {
		r_motor.vel += 0.001;
	}
	r_motor.duty = (int) (spec.step_dist / r_motor.vel * 6250000);
	if (r_motor.duty < 0) {
		PE.DRL.BIT.B1 = 1; //R_CW/CCW(0 : forward, 1 : backward)
		r_motor.cnt--;
	} else {
		PE.DRL.BIT.B1 = 0; //R_CW/CCW(0 : forward, 1 : backward)
		r_motor.cnt++;
	}

	if (vehicle.end_flag > 0) {
		r_motor.cnt = 0;
	}
//	r_motor.dist = spec.step_dist * r_motor.cnt;

	//change duty
	MTU20.TCNT = 0; //reset counter
	MTU20.TGRA = abs(r_motor.duty); //(1/v)*(step_distance / MTU_clock_duty)
	MTU20.TGRB = abs(round(r_motor.duty / 2)); //(1/v)*(step_distance / MTU_clock_duty)
	MTU20.TSR.BIT.TGFA = 0; //reset flag

//	start_MTU(cst0);

}

void change_Duty_MTU21(void) {
	/*
	 * this function operates L_motor
	 * write this function to interrupt_handlers.c
	 */
//	stop_MTU(cst1);
//	l_motor.duty = (int) (spec.step_dist / l_motor.vel * 12500000);
//	l_motor.duty = (int) (spec.step_dist / l_motor.vel * 25000000);
	if (l_motor.vel == 0.0) {
		l_motor.vel += 0.001;
	}
	l_motor.duty = (int) (spec.step_dist / l_motor.vel * 6250000);
	if (l_motor.duty < 0) {
		PE.DRL.BIT.B5 = 0; //L_CW/CCW(1 : forward, 0 : backward)
		l_motor.cnt--;
	} else {
		PE.DRL.BIT.B5 = 1; //L_CW/CCW(1 : forward, 0 : backward)
		l_motor.cnt++;
	}
	if (vehicle.end_flag > 0) {
		l_motor.cnt = 0;
	}

//	l_motor.dist = spec.step_dist * l_motor.cnt;
	//change duty
	MTU21.TCNT = 0; //reset counter
	MTU21.TGRA = abs(l_motor.duty);
	MTU21.TGRB = abs(round(l_motor.duty / 2));
	MTU21.TSR.BIT.TGFA = 0; //reset flag

//	start_MTU(cst1);
}
