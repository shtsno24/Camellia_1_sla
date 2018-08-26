/*
 * calc.c
 *
 *  Created on: 2018/04/14
 *      Author: shts
 */

#include "iodefine.h"
#include "sensor.h"
#include "motor.h"
#include "calc.h"
#include "math.h"
#include "util.h"
#include "LED.h"

extern SEN r_sen, l_sen, cr_sen, cl_sen;
extern SPC spec;
extern MOT r_motor, l_motor;
extern VEH vehicle;

void calc_diff(void) {
	short ref_boost_L, ref_boost_R;

	if (abs(r_sen.diff) > r_sen.diff_threshold) {
		ref_boost_R = 170;  //変化量が一定以上なら、閾値を引き上げる
	} else {
		ref_boost_R = 0; //変化量が一定以下なら、設定通りの閾値
	}

	if (abs(l_sen.diff) > l_sen.diff_threshold) {
		ref_boost_L = 170;  //変化量が一定以上なら、閾値を引き上げる
	} else {
		ref_boost_L = 0; //変化量が一定以下なら、設定通りの閾値
	}

	if (vehicle.select_flag == sla_on) {
		spec.diff = 0.0;
		return;
	} else {
		if ((r_sen.sen >= r_sen.non_threshold + ref_boost_R)
				&& (l_sen.sen >= l_sen.non_threshold + ref_boost_L)) {
			spec.diff = (float) ((l_sen.sen - l_sen.ref_wall)
					- (r_sen.sen - r_sen.ref_wall));

			if (spec.sta_LED_flag == 1) {
				drv_Status_LED(Green, on);
				drv_Status_LED(Yerrow, off);
				drv_Status_LED(Red, off);
			}

		} else if ((r_sen.sen >= r_sen.non_threshold + ref_boost_R)
				&& (l_sen.sen < l_sen.non_threshold + ref_boost_L)) {
			spec.diff = (float) (-2 * (r_sen.sen - r_sen.ref_wall));

			if (spec.sta_LED_flag == 1) {
				drv_Status_LED(Green, off);
				drv_Status_LED(Yerrow, on);
				drv_Status_LED(Red, off);
			}

		} else if ((r_sen.sen < r_sen.non_threshold + ref_boost_R)
				&& (l_sen.sen >= l_sen.non_threshold + ref_boost_L)) {
			spec.diff = (float) (2 * (l_sen.sen - l_sen.ref_wall));

			if (spec.sta_LED_flag == 1) {
				drv_Status_LED(Green, off);
				drv_Status_LED(Yerrow, off);
				drv_Status_LED(Red, on);
			}

		} else {
//			spec.diff = (float) (l_motor.cnt - r_motor.cnt) * 10;
			spec.diff = 0.0;
			if (spec.sta_LED_flag == 1) {
				drv_Status_LED(Green, off);
				drv_Status_LED(Yerrow, off);
				drv_Status_LED(Red, off);
			}

		}
	}

}

void calc_veh_vel() {
	/*
	 * calculate vehicle's velocity
	 * */
	if (vehicle.vel <= vehicle.tar_vel) {
		vehicle.buff_vel = vehicle.vel + vehicle.tar_acc * 0.001;
	} else if (vehicle.vel > vehicle.tar_vel) {
		vehicle.buff_vel = vehicle.vel - vehicle.tar_acc * 0.001;
	}
}

void calc_veh_rot_vel() {
	/*
	 * calculate vehicle's angular velocity
	 * */
	if (vehicle.rot_vel <= vehicle.tar_rot_vel) {
		vehicle.buff_rot_vel = vehicle.rot_vel + vehicle.tar_rot_acc * 0.001;
	} else if (vehicle.rot_vel > vehicle.tar_rot_vel) {
		vehicle.buff_rot_vel = vehicle.rot_vel - vehicle.tar_rot_acc * 0.001;
	}
}

void calc_2mot_vel() {
	/*
	 *  convert vehicle's velocity and angular velocity to motors' velocity
	 * */
	r_motor.tar_vel = vehicle.buff_vel
			- spec.tread * vehicle.buff_rot_vel / 2.0;
	l_motor.tar_vel = vehicle.buff_vel
			+ spec.tread * vehicle.buff_rot_vel / 2.0;
}

void calc_mot_vel() {
	/*
	 * follow motor velocity to target value
	 * */
	spec.kp_l = vehicle.vel * 0.001;
	spec.kp_r = vehicle.vel * 0.001;

	if (l_motor.tar_vel + spec.kp_l * spec.diff > l_motor.vel) {
		l_motor.vel += (l_motor.acc * 0.001);

	} else if (l_motor.tar_vel + spec.kp_l * spec.diff <= l_motor.vel) {
		l_motor.vel -= (l_motor.acc * 0.001);
	}

	if (r_motor.tar_vel - spec.kp_r * spec.diff > r_motor.vel) {
		r_motor.vel += (r_motor.acc * 0.001);

	} else if (r_motor.tar_vel - spec.kp_r * spec.diff <= r_motor.vel) {
		r_motor.vel -= (r_motor.acc * 0.001);
	}
}

void calc_2vel() {
	/*
	 * convert motors' velocity to vehicle's velocity and angular velocity
	 *
	 * */
	vehicle.vel = (l_motor.tar_vel + r_motor.tar_vel) / 2;
	vehicle.rot_vel = (l_motor.tar_vel - r_motor.tar_vel) / spec.tread;
//	vehicle.dist = (l_motor.dist + r_motor.dist) / 2;
//	vehicle.ang = (l_motor.dist - r_motor.dist) / spec.tread;
	vehicle.dist += vehicle.vel * 0.001;
	vehicle.ang += vehicle.rot_vel * 0.001;
}

void calc_dist_end() {
	/*
	 * raise end flag(sla_off)
	 * */

	if (fabs(vehicle.dist) >= fabs(vehicle.tar_dist)) {
		vehicle.end_flag = 1;
	}
}

void calc_ang_end() {
	/*
	 * raise end flag(sla_on)
	 * */

	if (fabs(vehicle.ang) >= fabs(vehicle.tar_ang)) {
		vehicle.end_flag = 2;
	}
}

void calc_vel() {
	calc_veh_rot_vel();
	calc_veh_vel();

	calc_2mot_vel();

	calc_mot_vel();

	calc_2vel();
}

void end_signal() {
	switch (vehicle.select_flag) {
	case sla_on:
//		calc_ang_end();
		if (fabs(vehicle.ang) >= fabs(vehicle.tar_ang)) {
			vehicle.end_flag = 2;
		}
		break;
	default:
//		calc_dist_end();
		if (fabs(vehicle.dist) >= fabs(vehicle.tar_dist)) {
			vehicle.end_flag = 1;
		}
		break;
	}
}
