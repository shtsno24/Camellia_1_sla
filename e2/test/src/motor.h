/*
 * motor.h
 *
 *  Created on: 2018/04/14
 *      Author: shts
 */

#ifndef MOTOR_H_
#define MOTOR_H_

typedef struct prm {
	struct {
		float kp;
		float kd;
	} gain;
	struct {
		float max_vel;
		float mid_vel;
		float min_vel;
		float tire_dim;
		float step;
	} straight;
	struct {
		float offset_dist_in;
		float offset_dist_out;
		float tire_dim;

		float angle;
		float vel;
		float max_rot_vel;
	} pow_turn;
	struct {
		float offset_dist_in;
		float offset_dist_out;
		float tire_dim;

		float angle;
		float vel;
		float max_rot_vel;
	} pow_turn_180;
	struct {
		float offset_dist_in;
		float offset_dist_out;
		float tire_dim;

		float angle;
		float vel;
		float max_rot_vel;
	} pow_turn_90;
} PRM;

enum motor {
	R_motor = 0, L_motor = 1
};

enum turn {
	back = 0, left = 1, right = 2, straight = 3,
};

typedef struct mot {
	int cnt; //ステップ数
	int duty; //デューティー

	float vel; //現在の速度
	float acc; //現在の設定加速度
	/*
	 float max_vel; //最大設定速度
	 float min_vel; //最小設定速度
	 float max_acc; //最大設定加速度
	 float min_acc; //最小設定加速度
	 */
	float tar_vel; //現在の設定速度
	float dist; //距離

	unsigned char rot_dir_flag; //回転方向
	unsigned char stop_flag;
	unsigned char end_flag;

} MOT;

void init_Params(void);

void init_Motor(void);

void drv_Motor(float, float, float, float, float, float, float, unsigned char,
		unsigned char);

void switch_Motor(char);

void start_Motor(void);

void mot_app(float, float, int, char, char);

void mot_app2(float, float, int, char, char);

void mot_sla_app(float, float, float, float, float, int, char);

void move_half_400(char);

void move_Left_400(PRM*);

void move_Right_400(PRM*);

void move_Left_180_s(unsigned char, PRM*);

void move_Right_180_s(unsigned char, PRM*);

void move_Left_90_s(unsigned char, PRM*);

void move_Right_90_s(unsigned char, PRM*);

void move_half_450(char);

void move_Right_450(void);

void move_half(char, float);

void move_Left(float);

void move_Right(float);

void move_Left_sla(float, PRM*);

void move_Right_sla(float, PRM*);

void move_Forward(float, float);

void move_Backward(float);

void move_Backward_2(float);

void move_Backward_sla(float);

void move_Backward_break(void);

#endif /* MOTOR_H_ */
