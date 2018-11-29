/*
 * motor.c
 *
 *  Created on: 2018/04/14
 *      Author: shts
 */
#include "iodefine.h"
#include "motor.h"
#include "serial.h"
#include "util.h"
#include "MTU.h"
#include "LED.h"
#include "logger.h"

PRM params[4];

MOT r_motor, l_motor;
extern VEH vehicle;
extern SPC spec;

void init_Params() {
	params[0].straight.max_vel = 1000;
	params[0].straight.mid_vel = 600;
	params[0].straight.min_vel = 500;
	params[0].straight.tire_dim = 51.0;

	params[0].pow_turn.angle = 82.6;
	params[0].pow_turn.max_rot_vel = 425.0;
	params[0].pow_turn.vel = params[0].straight.min_vel;
	params[0].pow_turn.offset_dist_in = 22.0;
	params[0].pow_turn.offset_dist_out = 22.0;
	params[0].pow_turn.tire_dim = 50.4;

	params[0].pow_turn_180.angle = 175.0;
	params[0].pow_turn_180.max_rot_vel = 385.0;
	params[0].pow_turn_180.vel = params[0].straight.mid_vel;
	params[0].pow_turn_180.offset_dist_in = 80;
	params[0].pow_turn_180.offset_dist_out = 90;
	params[0].pow_turn_180.tire_dim = 50.4;
	//=====================================
	params[1].straight.max_vel = 1200;
	params[1].straight.mid_vel = 600;
	params[1].straight.min_vel = 500;
	params[1].straight.tire_dim = 51.0;

	params[1].pow_turn.angle = 82.6;
	params[1].pow_turn.max_rot_vel = 425.0;
	params[1].pow_turn.vel = params[1].straight.min_vel;
	params[1].pow_turn.offset_dist_in = 22.7;
	params[1].pow_turn.offset_dist_out = 22.7;
	params[1].pow_turn.tire_dim = 50.4;

	params[1].pow_turn_180.angle = 175.0;
	params[1].pow_turn_180.max_rot_vel = 385.0;
	params[1].pow_turn_180.vel = params[1].straight.mid_vel;
	params[1].pow_turn_180.offset_dist_in = 80;
	params[1].pow_turn_180.offset_dist_out = 90;
	params[1].pow_turn_180.tire_dim = 50.4;
	//=====================================
	params[2].straight.max_vel = 1000;
	params[2].straight.mid_vel = 750;
	params[2].straight.min_vel = 600;
	params[2].straight.tire_dim = 51.0;

	params[2].pow_turn.angle = 80.5;
	params[2].pow_turn.max_rot_vel = 480.0;
	params[2].pow_turn.vel = params[2].straight.min_vel;
	params[2].pow_turn.offset_dist_in = 19.5;
	params[2].pow_turn.offset_dist_out = 19.5;
	params[2].pow_turn.tire_dim = 50.4;

	params[2].pow_turn_180.angle = 169.5;
	params[2].pow_turn_180.max_rot_vel = 487.0;
	params[2].pow_turn_180.vel = params[2].straight.mid_vel;
	params[2].pow_turn_180.offset_dist_in = 90;
	params[2].pow_turn_180.offset_dist_out = 100;
	params[2].pow_turn_180.tire_dim = 50.4;

	//=====================================
	params[3].straight.max_vel = 1500;
	params[3].straight.mid_vel = 750;
	params[3].straight.min_vel = 600;
	params[3].straight.tire_dim = 51.0;

	params[3].pow_turn.angle = 80.5;
	params[3].pow_turn.max_rot_vel = 480.0;
	params[3].pow_turn.vel = params[3].straight.min_vel;
	params[3].pow_turn.offset_dist_in = 19.5;
	params[3].pow_turn.offset_dist_out = 19.5;
	params[3].pow_turn.tire_dim = 50.4;

	params[3].pow_turn_180.angle = 169.5;
	params[3].pow_turn_180.max_rot_vel = 487.0;
	params[3].pow_turn_180.vel = params[3].straight.mid_vel;
	params[3].pow_turn_180.offset_dist_in = 90;
	params[3].pow_turn_180.offset_dist_out = 100;
	params[3].pow_turn_180.tire_dim = 50.4;
}

void init_Motor(void) {

	spec.motor_max_acc = 15000;
	spec.motor_max_vel = 1800;
	spec.motor_min_acc = 0;
	spec.motor_min_vel = 180;
	r_motor.acc = 0.0;
	l_motor.acc = 0.0;

	r_motor.vel = 0.0;
	l_motor.vel = 0.0;

	r_motor.cnt = 0.0;
	l_motor.cnt = 0.0;

	r_motor.end_flag = 0.0;
	l_motor.end_flag = 0.0;
	r_motor.stop_flag = 0.0;
	l_motor.stop_flag = 0.0;

	r_motor.rot_dir_flag = 0;
	l_motor.rot_dir_flag = 1;

	r_motor.dist = 0.0;

	PE.DRL.BIT.B2 = 1; //reset (0 : off, 1 : on)
	PE.DRL.BIT.B3 = 0; //excitation_enable (1 : on, 0 : off)

	PE.DRL.BIT.B1 = 0; //R_CW/CCW(0 : forward, 1 : backward)
	PE.DRL.BIT.B5 = 1; //L_CW/CCW(1 : forward, 0 : backward)

	PE.DRL.BIT.B0 = 1; //R_Clock
	PE.DRL.BIT.B4 = 1; //L_Clock

	PE.DRL.BIT.B2 = 0; //reset(0 : off, 1 : on)
}

void switch_Motor(char sw) {
	if (sw == on) {
		PE.DRL.BIT.B3 = 1;
	} else {
		PE.DRL.BIT.B3 = 0;
	}
}

void drv_Motor(float dist, float max_vel, float end_vel, float ang,
		float max_rot_vel, float end_rot_vel, float mot_acc,
		unsigned char direction, unsigned char stop_flag) {
	/*
	 * direction : straight, back, left or right (enum)
	 * each parameters must be absolute value
	 * ang[deg], rot_vel[deg/s], rot_acc[deg/s^2]
	 * select_flag : sla_on or sla_off (enum)
	 * stop_flag : on or off (enum)
	 * */
	float deceleration, acceleration, vel = vehicle.vel, rot_vel =
			vehicle.rot_vel;

	//	convert deg2rad
	ang = ang / 180.0 * 3.141592;
	max_rot_vel = max_rot_vel / 180.0 * 3.141592;
	end_rot_vel = end_rot_vel / 180.0 * 3.141592;

	//上下限設定
	if (mot_acc > spec.motor_max_acc) {
		mot_acc = spec.motor_max_acc;
	}
	//直進判定
	if (ang == 0.0) {
		vehicle.select_flag = sla_off;
	} else {
		vehicle.select_flag = sla_on;
	}

	switch (vehicle.select_flag) {
	case sla_on: //スラローム
		//角度計算

//		mot_acc = mot_acc / 180.0 * 3.141592;

		deceleration = (max_rot_vel * max_rot_vel - end_rot_vel * end_rot_vel)
				/ (2.0 * mot_acc);
		acceleration = (max_rot_vel * max_rot_vel - rot_vel * rot_vel)
				/ (2.0 * mot_acc);

		if (deceleration + acceleration > ang) {
			deceleration = ang / 2.0
					+ (rot_vel * rot_vel - end_rot_vel * end_rot_vel)
							/ (4.0 * mot_acc);
		}

		acceleration = ang - deceleration;

		if (direction == left) {
			deceleration *= -1;
			acceleration *= -1;
			max_rot_vel *= -1;
		}

		//加速フェーズ
		vehicle.tar_dist = dist;
		vehicle.tar_vel = max_vel;
		vehicle.tar_ang = acceleration;
		vehicle.tar_rot_vel = max_rot_vel;

		vehicle.end_flag = 0;
		r_motor.acc = mot_acc;
		l_motor.acc = mot_acc;
		vehicle.dist = 0;
		vehicle.ang = 0;

		start_MTU(cst0);
		start_MTU(cst1);
		while (1) {
			if (vehicle.end_flag > 0) {
				break;
			}
			write_Logger();
//			/*TODO*/myprintf("%f,%f,%f,%f,%f,%f\n", l_motor.vel, r_motor.vel,
//					vehicle.rot_vel, vehicle.tar_rot_vel, vehicle.tar_ang,
//					vehicle.ang);
		}

		//減速フェーズ
		vehicle.tar_dist = dist;
		vehicle.tar_vel = max_vel;

		vehicle.tar_ang = deceleration;
		vehicle.tar_rot_vel = end_rot_vel;
		if (direction == left) {
			vehicle.tar_rot_vel *= -1.0;
		}

		vehicle.end_flag = 0;
		vehicle.dist = 0;
		vehicle.ang = 0;

		while (1) {
			if (vehicle.end_flag > 0) {
				break;
			}
			write_Logger();
//			/*TODO*/myprintf("%f,%f,%f,%f,%f,%f\n", l_motor.vel, r_motor.vel,
//					vehicle.rot_vel, vehicle.tar_rot_vel, vehicle.tar_ang,
//					vehicle.ang);
		}
		//停止判定
		if (stop_flag == 1) {
			stop_MTU(cst0);
			stop_MTU(cst1);
			vehicle.tar_vel = 0.0;
			vehicle.tar_rot_vel = 0.0;
			r_motor.tar_vel = 0.0;
			l_motor.tar_vel = 0.0;
			r_motor.vel = 0.0;
			l_motor.vel = 0.0;

		}
		break;

	case sla_off:		//直進
		//距離計算
		deceleration = (max_vel * max_vel - end_vel * end_vel)
				/ (2.0 * mot_acc);
		acceleration = (max_vel * max_vel - vel * vel) / (2.0 * mot_acc);

		if (deceleration + acceleration > dist) {
			deceleration = dist / 2.0
					+ (vel * vel - end_vel * end_vel) / (4.0 * mot_acc);
		}

		if (deceleration > dist) {
			deceleration = dist;
		} else if (deceleration < 0) {
			deceleration = 0;
		}
		acceleration = dist - deceleration;

		if (direction == back) {
			acceleration *= -1;
			deceleration *= -1;
			max_vel *= -1;
		}
		//加速フェーズ
		vehicle.tar_dist = acceleration;
		vehicle.tar_vel = max_vel;
		vehicle.tar_ang = ang;
		vehicle.tar_rot_vel = max_rot_vel;

		vehicle.end_flag = 0;
		r_motor.acc = mot_acc;
		l_motor.acc = mot_acc;
		vehicle.dist = 0;
		vehicle.ang = 0;

		start_MTU(cst0);
		start_MTU(cst1);
		while (1) {
			if (vehicle.end_flag > 0) {
				break;
			}
			write_Logger();
//TODO			myprintf("%f,%f,%f,%f,%f,%f\n", l_motor.vel, r_motor.vel,
//					vehicle.vel, vehicle.tar_vel, vehicle.tar_dist,
//					vehicle.dist);
		}
		//減速フェーズ
		vehicle.tar_dist = deceleration;
		vehicle.tar_vel = end_vel;
		if (direction == back) {
			vehicle.tar_vel *= -1.0;
		}
		vehicle.tar_ang = ang;
		vehicle.tar_rot_vel = max_rot_vel;

		vehicle.end_flag = 0;
		vehicle.dist = 0;
		vehicle.ang = 0;

		while (1) {
			if (vehicle.end_flag > 0) {
				break;
			}
			write_Logger();
//TODO			myprintf("%f,%f,%f,%f,%f,%f\n", l_motor.vel, r_motor.vel,
//					vehicle.vel, vehicle.tar_vel, vehicle.tar_dist,
//					vehicle.dist);
		}
		//停止判定
		if (stop_flag == 1) {
			stop_MTU(cst0);
			stop_MTU(cst1);
			vehicle.tar_vel = 0.0;
			vehicle.tar_rot_vel = 0.0;
			r_motor.tar_vel = 0.0;
			l_motor.tar_vel = 0.0;
			r_motor.vel = 0.0;
			l_motor.vel = 0.0;
		}
		break;
	}

}

void move_half_400(char flag) {
	if (flag == on) {
		drv_Motor(spec.half_block, vehicle.vel, spec.motor_min_vel, 0.0, 0.0,
				0.0, 1800.0, straight, flag);
	} else {
		drv_Motor(spec.half_block, 450.0, 450.0, 0.0, 0.0, 0.0, 1800.0,
				straight, flag);
	}
}

void move_Left_400(PRM* prm) {
	drv_Status_LED(Rst_status_LED, off);
	drv_Motor(prm->pow_turn.offset_dist_in, prm->pow_turn.vel,
			prm->pow_turn.vel, 0.0, 0.0, 0.0, 1500.0, straight, off);
	drv_Status_LED(Yerrow, on);
	drv_Motor(0.0, prm->pow_turn.vel, prm->pow_turn.vel, prm->pow_turn.angle,
			prm->pow_turn.max_rot_vel, 0.0, 12000.0, left, off);
	drv_Status_LED(Green, on);
	drv_Motor(prm->pow_turn.offset_dist_out, prm->pow_turn.vel,
			prm->pow_turn.vel, 0.0, 0.0, 0.0, 9000.0, straight, off);
	drv_Status_LED(Rst_status_LED, off);
}

void move_Right_400(PRM* prm) {
	drv_Status_LED(Rst_status_LED, off);
	drv_Motor(prm->pow_turn.offset_dist_in, prm->pow_turn.vel,
			prm->pow_turn.vel, 0.0, 0.0, 0.0, 1500.0, straight, off);
	drv_Status_LED(Yerrow, on);
	drv_Motor(0.0, prm->pow_turn.vel, prm->pow_turn.vel, prm->pow_turn.angle,
			prm->pow_turn.max_rot_vel, 0.0, 12000.0, right, off);
	drv_Status_LED(Green, on);
	drv_Motor(prm->pow_turn.offset_dist_out, prm->pow_turn.vel,
			prm->pow_turn.vel, 0.0, 0.0, 0.0, 9000.0, straight, off);
	drv_Status_LED(Rst_status_LED, off);
}

void move_Left_180_s(unsigned char flag, PRM* prm) {
	drv_Motor(spec.half_block * (flag & 1) + prm->pow_turn_180.offset_dist_in,
			prm->pow_turn_180.vel, prm->pow_turn_180.vel, 0.0, 0.0, 0.0, 1500.0,
			straight, off);
	drv_Motor(0.0, prm->pow_turn_180.vel, prm->pow_turn_180.vel,
			prm->pow_turn_180.angle, prm->pow_turn_180.max_rot_vel, 0.0, 8000.0,
			left, off);
	drv_Motor(
			spec.half_block * ((flag & 2) >> 1)
					+ prm->pow_turn_180.offset_dist_out, prm->pow_turn_180.vel,
			prm->pow_turn_180.vel, 0.0, 0.0, 0.0, 9000.0, straight, off);
}

void move_Right_180_s(unsigned char flag, PRM* prm) {
	drv_Motor(spec.half_block * (flag & 1) + prm->pow_turn_180.offset_dist_in,
			prm->pow_turn_180.vel, prm->pow_turn_180.vel, 0.0, 0.0, 0.0, 1500.0,
			straight, off);
	drv_Motor(0.0, prm->pow_turn_180.vel, prm->pow_turn_180.vel,
			prm->pow_turn_180.angle, prm->pow_turn_180.max_rot_vel, 0.0, 8000.0,
			right, off);
	drv_Motor(
			spec.half_block * ((flag & 2) >> 1)
					+ prm->pow_turn_180.offset_dist_out, prm->pow_turn_180.vel,
			prm->pow_turn_180.vel, 0.0, 0.0, 0.0, 9000.0, straight, off);
}

void move_half(char flag, float offset) {
	if (flag == on) {
		drv_Motor(spec.half_block - offset, vehicle.vel, spec.motor_min_vel,
				0.0, 0.0, 0.0, 1800.0, straight, flag);
	} else {
		drv_Motor(spec.half_block - offset, 400.0, 400.0, 0.0, 0.0, 0.0, 1800.0,
				straight, flag);
	}
}

void move_Left(float offset) {
	move_half(on, offset);
	drv_Status_LED(Red, on);
	wait_ms(50);
	drv_Motor(0.0, 0.0, 0.0, 90.0, 400.0, 0.0, 750.0, left, on);
	drv_Status_LED(Green, on);
	wait_ms(50);
	move_half(off, 0);
	drv_Status_LED(Rst_status_LED, off);
}

void move_Right(float offset) {
	move_half(on, offset);
	drv_Status_LED(Red, on);
	wait_ms(50);
	drv_Motor(0.0, 0.0, 0.0, 90.0, 400.0, 0.0, 750.0, right, on);
	drv_Status_LED(Green, on);
	wait_ms(50);
	move_half(off, 0);
	drv_Status_LED(Rst_status_LED, off);
}

void move_Left_sla(float offset, PRM* prm) {
	drv_Status_LED(Rst_status_LED, off);
	drv_Motor((prm->pow_turn.offset_dist_in) - offset, prm->pow_turn.vel,
			prm->pow_turn.vel, 0.0, 0.0, 0.0, 1800.0, straight, off);
	drv_Status_LED(Yerrow, on);
	drv_Motor(0.0, prm->pow_turn.vel, prm->pow_turn.vel, prm->pow_turn.angle,
			prm->pow_turn.max_rot_vel, 0.0, 12000.0, left, off);
	drv_Status_LED(Green, on);
	drv_Motor(prm->pow_turn.offset_dist_out, prm->pow_turn.vel,
			prm->pow_turn.vel, 0.0, 0.0, 0.0, 9000.0, straight, off);
	drv_Status_LED(Rst_status_LED, off);
}

void move_Right_sla(float offset, PRM* prm) {

	drv_Status_LED(Rst_status_LED, off);
	drv_Motor((prm->pow_turn.offset_dist_in) - offset, prm->pow_turn.vel,
			prm->pow_turn.vel, 0.0, 0.0, 0.0, 1800.0, straight, off);
	drv_Status_LED(Yerrow, on);
	drv_Motor(0.0, prm->pow_turn.vel, prm->pow_turn.vel, prm->pow_turn.angle,
			prm->pow_turn.max_rot_vel, 0.0, 12000.0, right, off);
	drv_Status_LED(Green, on);
	drv_Motor(prm->pow_turn.offset_dist_out, prm->pow_turn.vel,
			prm->pow_turn.vel, 0.0, 0.0, 0.0, 9000.0, straight, off);
	drv_Status_LED(Rst_status_LED, off);
}

void move_Forward(float offset, float max_vel) {
	drv_Motor(spec.full_block - offset, max_vel, max_vel, 0.0, 0.0, 0.0, 1800.0,
			straight, off);
}

void move_Backward(float offset) {
	drv_Status_LED(Rst_status_LED, off);
	move_half(on, offset);
	wait_ms(50);

	drv_Status_LED(Red, on);
	drv_Motor(0.0, 0.0, 0.0, 180.0, 400.0, 0.0, 750.0, right, on);
	drv_Status_LED(Green, on);
	drv_Status_LED(Rst_status_LED, off);
	wait_ms(50);

	drv_Status_LED(Red, on);
	drv_Motor(50, spec.motor_min_vel, spec.motor_min_vel, 0.0, 0.0, 0.0,
			500.0, back, on);
	drv_Status_LED(Green, on);
	wait_ms(500);
	drv_Motor(spec.half_block + 10.0, 400.0, 400.0, 0.0, 0.0, 0.0, 800.0,
			straight, off);
	drv_Status_LED(Rst_status_LED, off);
}

void move_Backward_2(float offset) {
	drv_Status_LED(Rst_status_LED, off);
	move_half(on, offset);
	wait_ms(50);
	drv_Status_LED(Red, on);
	drv_Motor(0.0, 0.0, 0.0, 180.0, 400.0, 0.0, 750.0, right, on);
	drv_Status_LED(Rst_status_LED, off);
	wait_ms(50);
	drv_Status_LED(Red, on);
	move_half(off, 0);
	drv_Status_LED(Rst_status_LED, off);
}

void move_Backward_sla(float offset) {
	drv_Status_LED(Rst_status_LED, off);
	move_half(on, offset);
	wait_ms(50);

	drv_Status_LED(Red, on);
	drv_Motor(0.0, 0.0, 0.0, 180.0, 400.0, 0.0, 750.0, right, on);
	drv_Status_LED(Green, on);
	drv_Status_LED(Rst_status_LED, off);
	wait_ms(50);

	drv_Status_LED(Red, on);
	drv_Motor(50, spec.motor_min_vel, spec.motor_min_vel, 0.0, 0.0, 0.0, 500.0,
			back, on);
	drv_Status_LED(Green, on);
	wait_ms(500);
	drv_Motor(spec.half_block + 10.0, 400.0, 400.0, 0.0, 0.0, 0.0, 1800.0,
			straight, off);
	drv_Status_LED(Rst_status_LED, off);
}

void move_Backward_break() {
	drv_Status_LED(Rst_status_LED, off);
	move_half_400(on);
	wait_ms(50);
	drv_Status_LED(Red, on);
	drv_Motor(0.0, 0.0, 0.0, 180.0, 200.0, 0.0, 750.0, right, on);
	drv_Status_LED(Rst_status_LED, off);
	wait_ms(50);
	drv_Status_LED(Red, on);
	move_half_400(on);
	drv_Status_LED(Rst_status_LED, off);
}
