/*
 * util.h
 *
 *  Created on: 2018/04/13
 *      Author: shts
 */

#ifndef UTIL_H_
#define UTIL_H_

enum ux {
	error = 0, click = 1, alart = 2
};

enum on_off {
	off = 0, on = 1
};

enum ctl_flag {
	wall = 0, rot = 1, none = 2
};

enum sla_flag {
	sla_off = 0, sla_on = 1
};

typedef struct veh {
	float dist; //現在距離
	float tar_dist; //目標距離
	float vel; //現在速度
	float tar_vel; //目標速度
	float tar_acc; //目標加速度
	float buff_vel; //計算結果

	float ang; //[rad] 現在車体角
	float tar_ang; //[rad/s] 目標角速度
	float rot_vel; //[rad/s] 現在角速度
	float tar_rot_vel; //[rad/s] 目標角速度
	float tar_rot_acc; //[rad/s^2] 目標角加速度
	float buff_rot_vel; //計算結果

	unsigned char end_flag; // 終了可否
	unsigned char select_flag; //スラローム・直進選択
} VEH;

typedef struct spc {
	float tread; //[mm]
	float tire_dim; //[mm]
	float step_dist; //[mm]
	float step_angle; //[deg]

	short pwm_base_clock; //set interrupt duty
	short pwm_half_clock;

	float batt;
	float kp_l;
	float kp_r;
	float kd_l;
	float kd_r;
	float diff;
	float prev_diff;
	float r_distance;
	float l_distance;
	float full_block;
	float half_block;
	float motor_max_vel;
	float motor_min_vel;
	float motor_max_acc;
	float motor_min_acc;
	float vehicle_max_vel;
	float vehicle_min_vel;
	float vehicle_max_acc;
	float vehicle_min_acc;

	unsigned char cnt_ctl;
	unsigned char run_interruption;
	unsigned char sta_LED_flag;

} SPC;

typedef struct sw {
	unsigned char rot_sw; //ロータリーエンコーダ
	unsigned char rot_sw_now; //ロータリーエンコーダ
	unsigned char rot_sw_old; //ロータリーエンコーダ

	unsigned char tact_sw; //タクトスイッチ

} SW;

void init_Vehicle(void);

void init_Spec(void);

void init_IO(void);

void init_CPU(void);

void init_Buzzer(void);

void drv_Buzzer(char);

void UX_effect(char);

void init_Switch(void);

void detect_Rotate(void);

void select_Mode(void);

void select_Params(unsigned char);

void get_Battery_Voltage(void);
#endif /* UTIL_H_ */
