/***************************************************************/
/*                                                             */
/*      PROJECT NAME :  test                                   */
/*      FILE         :  test.c                                 */
/*      DESCRIPTION  :  Main Program                           */
/*      CPU SERIES   :  SH-2                                   */
/*      CPU TYPE     :  SH7125                                 */
/*                                                             */
/*      This file is generated by e2 studio.                   */
/*                                                             */
/***************************************************************/

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

#define round(A)((int)(A + 0.5))

int route_index, i;
float vel, m_vel, OFFSET;
unsigned char target_area[2][2];
unsigned char searchtimes = 0, flag = 0;
PRM* p;

extern SPC spec;
extern SEN r_sen, cr_sen, l_sen, cl_sen;
extern MOT r_motor, l_motor;
extern VEH vehicle;
extern SW Switch;
extern CMT_01 tim;
extern MAP map;
extern LOG logger;
extern PRM params[4];

enum mode {
	astar_sla = 0, astar = 1, show_map = 2, test = 3, run = 4
};

void init_target() {
//			unsigned char target_area[4][2] = {{mp_size-2,0},{map.goal_x,map.goal_y},{0,mp_size-2},{0,0}};//[point][i=0;pos_x,i=1;pos_y]
	target_area[0][0] = 0;
	target_area[0][1] = mp_size - 2;

	target_area[1][0] = map.goal_x;
	target_area[1][1] = map.goal_y;

	target_area[2][0] = mp_size - 2;
	target_area[2][1] = 0;

	target_area[3][0] = 0;
	target_area[3][1] = 0;
}

void sen_calibration() {

	UX_effect(alart);

	cr_sen.non_threshold = cr_sen.sen;
	cl_sen.non_threshold = cl_sen.sen;
	r_sen.ref_wall = r_sen.sen;
	l_sen.ref_wall = l_sen.sen;
	while (PB.DR.BIT.B5 != 0) {
		//remove walls
		drv_Status_LED(Green, on);
	}

	UX_effect(alart);
	myprintf("ref_wall : %d | %d\n", l_sen.ref_wall, r_sen.ref_wall);

	cr_sen.non_threshold += cr_sen.sen;
	cl_sen.non_threshold += cl_sen.sen;
	r_sen.non_threshold = r_sen.sen;
	l_sen.non_threshold = l_sen.sen;

	cr_sen.non_threshold /= 2;
	cl_sen.non_threshold /= 2;

	while (PB.DR.BIT.B5 != 0) {
		drv_Status_LED(Yerrow, on);
	}

	myprintf("non_wall : %d | %d | %d | %d\n", l_sen.non_threshold,
			cl_sen.non_threshold, cr_sen.non_threshold, r_sen.non_threshold);
	UX_effect(alart);
	drv_Status_LED(Rst_status_LED, off);
}

int main(void) {
	float v_r = 0;
	float v_l = 0;
	init_sci1();
	init_ALL();
	init_target();

	switch_Motor(off);

	while (1) {
		while (spec.batt <= 11.3) {
			get_Battery_Voltage();
			myprintf("Low_battery\n");
			UX_effect(error);
			myprintf("\033[1A");
		}
		myprintf("\n");
		drv_Status_LED(Red, off);

		while (PB.DR.BIT.B5 != 0) {
			get_Battery_Voltage();
			select_Mode();
			myprintf("batt : %f\n", spec.batt);
			myprintf("sen : %d | %d | %d | %d\n", l_sen.sen, cl_sen.sen,
					cr_sen.sen, r_sen.sen);
			myprintf("mode : %d\n", Switch.rot_sw);
			wait_ms(100);
			myprintf("\033[3A");
		}
		myprintf("\033[2J");
		UX_effect(click);
		wait_ms(100);
		drv_Status_LED(Rst_status_LED, off);
		switch (Switch.rot_sw) {

		case astar_sla:
			wait_ms(1000);
			Switch.rot_sw = 0;
			while (PB.DR.BIT.B5 != 0) {
				myprintf("params : %d\n", Switch.rot_sw);
				select_Params(2);
				myprintf("\033[1A");
				wait_ms(200);
			}
			wait_ms(1000);
			drv_Status_LED(Rst_status_LED, off);
			PE.DRL.BIT.B2 = 1; //reset (0 : off, 1 : on)
			drv_Status_LED(Rst_status_LED, off);
			i = 0;
			searchtimes += Switch.rot_sw;
			spec.sta_LED_flag = 0;
			map.pos_x = 0;
			map.pos_y = 1;
			map.direction = 0;

			if (searchtimes == 0) {
				map.tar_x = target_area[i][0];
				map.tar_y = target_area[i][1];
			} else {
				map.tar_x = map.goal_x;
				map.tar_y = map.goal_y;
			}

			spec.run_interruption = 0;
			spec.step_dist = spec.tire_dim * 3.1415926
					* (spec.step_angle / 360.0); //[mm]
			spec.kp_l = 0.2;
			spec.kp_r = 0.2;
			spec.kd_l = 0.3;
			spec.kd_r = 0.3;
			UX_effect(alart);

			PE.DRL.BIT.B2 = 0; //reset (0 : off, 1 : on)
			switch_Motor(on);
			wait_ms(1000);
			move_half_400(off);

			while (spec.run_interruption != 1) {
				vehicle.dist = 0;
				init_A_dist_map();
				update_Wall_map();
				update_A_dist_map();
				if (spec.run_interruption == 2) {
					break;
				}
				map.tmp_path = generate_A_path();
				OFFSET = vehicle.dist;
				if (map.tmp_path == R_small) {
					map.direction += 1;

					spec.tire_dim = params[0].pow_turn.tire_dim; //[mm]
					spec.step_dist = spec.tire_dim * 3.1415926
							* (spec.step_angle / 360.0); //[mm]

					move_Right_sla(vehicle.dist, &params[0]);
				} else if (map.tmp_path == L_small) {
					map.direction += 3;

					spec.tire_dim = params[0].pow_turn.tire_dim; //[mm]
					spec.step_dist = spec.tire_dim * 3.1415926
							* (spec.step_angle / 360.0); //[mm]

					move_Left_sla(vehicle.dist, &params[0]);
				} else if (map.tmp_path == Forward) {
					map.direction += 0;
					spec.tire_dim = 51.0; //[mm]

					spec.step_dist = spec.tire_dim * 3.1415926
							* (spec.step_angle / 360.0); //[mm]

					move_Forward(vehicle.dist, 550);
				} else if (map.tmp_path == Backward) {
					map.direction += 2;
					spec.tire_dim = params[0].pow_turn.tire_dim; //[mm]
					spec.step_dist = spec.tire_dim * 3.1415926
							* (spec.step_angle / 360.0); //[mm]

					if (map.wall == 7 || map.wall == 11 || map.wall == 13
							|| map.wall == 14) {
						move_Backward(vehicle.dist);
					} else {
						move_Backward_2(-5);
					}
				}
				map.direction %= 4;
				detect_Direction();
				flag = check_pos();

				if (flag > 0) {
					if (map.pos_x == 0 && map.pos_y == 0) {
						spec.run_interruption = 1;
					} else {
						if (searchtimes == 0) {
							i += 1;
							map.tar_x = target_area[i][0];
							map.tar_y = target_area[i][1];
						} else {
							map.tar_x = 0;
							map.tar_y = 0;
						}
						if (flag == 2) {
							map.direction += 0;
							spec.tire_dim = params[0].straight.tire_dim; //[mm]

							spec.step_dist = spec.tire_dim * 3.1415926
									* (spec.step_angle / 360.0); //[mm]

							move_Forward(0, 550);
							map.direction %= 4;
							detect_Direction();
						}
					}
				}

			}

			if (spec.run_interruption == 1) {
				update_Wall_map();
				move_half_400(on);
				wait_ms(300);
				drv_Motor(0.0, 0.0, 0.0, 180.0, 330.0, 0.0, 1000.0, right, on);
				drv_Status_LED(Green, on);
				wait_ms(50);
				drv_Motor(70, 230.0, spec.motor_min_vel, 0.0, 0.0, 0.0, 1200.0,
						back, on);

				wait_ms(300);
				switch_Motor(off);
				spec.sta_LED_flag = 0;
				map.pos_x = 0;
				map.pos_y = 0;
				map.tar_x = map.goal_x;
				map.tar_y = map.goal_y;
				map.direction = 0;
				init_Path();
				init_Dist_map();
				update_Dist_map();
				generate_Path();
				searchtimes = 1;
				Switch.rot_sw = astar_sla;
			} else if (spec.run_interruption == 2) {
				move_half(on, vehicle.dist);
				wait_ms(300);
				UX_effect(error);
				UX_effect(error);
				UX_effect(error);
			}
			break;
		case run:
			PE.DRL.BIT.B2 = 1; //reset (0 : off, 1 : on)
			wait_ms(1000);
			Switch.rot_sw = 0;
			while (PB.DR.BIT.B5 != 0) {
				myprintf("params : %d\n", Switch.rot_sw);
				select_Params(4);
				wait_ms(200);
				myprintf("\033[1A");
			}
			wait_ms(100);
			drv_Status_LED(Rst_status_LED, off);

			spec.sta_LED_flag = 0;
			map.pos_x = 0;
			map.pos_y = 1;
			map.direction = 0;
			spec.run_interruption = 0;

			spec.tire_dim = 50.4; //[mm]
			spec.step_dist = spec.tire_dim * 3.1415926
					* (spec.step_angle / 360.0); //[mm]

			p = &params[Switch.rot_sw];
			spec.kp_l = p->gain.kp;
			spec.kp_r = p->gain.kp;
			spec.kd_l = p->gain.kd;
			spec.kd_r = p->gain.kd;
			UX_effect(alart);
			route_index = 0;

			PE.DRL.BIT.B2 = 0; //reset (0 : off, 1 : on)
			switch_Motor(on);
			wait_ms(1000);
			move_half_400(off);

			while (map.path_test[route_index].index != End) {

				if (map.path_test[route_index].index == R_small) {
					spec.tire_dim = (p->pow_turn.tire_dim); //[mm]
					spec.step_dist = spec.tire_dim * 3.1415926
							* (spec.step_angle / 360.0); //[mm]

					move_Right_400(p);
				} else if (map.path_test[route_index].index == L_small) {
					spec.tire_dim = (p->pow_turn.tire_dim); //[mm]
					spec.step_dist = spec.tire_dim * 3.1415926
							* (spec.step_angle / 360.0); //[mm]

					move_Left_400(p);
				} else if (map.path_test[route_index].index == R_180) {
					spec.tire_dim = (p->pow_turn_180.tire_dim); //[mm]
					spec.step_dist = spec.tire_dim * 3.1415926
							* (spec.step_angle / 360.0); //[mm]

					move_Right_180_s(map.path_test[route_index].block_num, p);
				} else if (map.path_test[route_index].index == L_180) {
					spec.tire_dim = (p->pow_turn_180.tire_dim); //[mm]
					spec.step_dist = spec.tire_dim * 3.1415926
							* (spec.step_angle / 360.0); //[mm]

					move_Left_180_s(map.path_test[route_index].block_num, p);
				} else if (map.path_test[route_index].index == R_90) {
					spec.tire_dim = (p->pow_turn_180.tire_dim); //[mm]
					spec.step_dist = spec.tire_dim * 3.1415926
							* (spec.step_angle / 360.0); //[mm]

					move_Right_90_s(map.path_test[route_index].block_num, p);
				} else if (map.path_test[route_index].index == L_90) {
					spec.tire_dim = (p->pow_turn_180.tire_dim); //[mm]
					spec.step_dist = spec.tire_dim * 3.1415926
							* (spec.step_angle / 360.0); //[mm]

					move_Left_90_s(map.path_test[route_index].block_num, p);
				} else if (map.path_test[route_index].index == Forward) {
					spec.tire_dim = (p->straight.tire_dim); //[mm]
					spec.step_dist = spec.tire_dim * 3.1415926
							* (spec.step_angle / 360.0); //[mm]

					if (map.path_test[route_index].block_num <= 8) {
						vel = p->straight.min_vel
								+ p->straight.step * map.path_test[route_index].block_num;
						i = 0;
					} else {
						vel = p->straight.max_vel;
						i = 0;
					}
					if (map.path_test[route_index + 1].index == R_180
							|| map.path_test[route_index + 1].index == L_180) {
						m_vel = p->straight.mid_vel;
						if (vel < m_vel) {
							vel = m_vel;
						}
					} else {
						m_vel = p->straight.min_vel;
					}

					drv_Motor(
							(spec.full_block + i)
									* map.path_test[route_index].block_num, vel,
							m_vel, 0.0, 0.0, 0.0, 2000.0, straight, off);
				} else {
					spec.tire_dim = (p->pow_turn.tire_dim); //[mm]
					spec.step_dist = spec.tire_dim * 3.1415926
							* (spec.step_angle / 360.0); //[mm]

					move_Backward_2(0);
				}
				route_index += 1;
			}
			move_half_400(on);

			wait_ms(300);
			switch_Motor(off);
			spec.sta_LED_flag = 0;
			Switch.rot_sw = run;
			break;

		case astar:
			wait_ms(1000);
			Switch.rot_sw = 0;
			while (PB.DR.BIT.B5 != 0) {
				myprintf("params : %d\n", Switch.rot_sw);
				select_Params(2);
				wait_ms(200);
				myprintf("\033[1A");
			}
			wait_ms(1000);
			drv_Status_LED(Rst_status_LED, off);
			PE.DRL.BIT.B2 = 1; //reset (0 : off, 1 : on)
			drv_Status_LED(Rst_status_LED, off);
			i = 0;
			searchtimes += Switch.rot_sw;
			spec.sta_LED_flag = 0;
			map.pos_x = 0;
			map.pos_y = 1;
			map.direction = 0;

			if (searchtimes == 0) {
				map.tar_x = target_area[i][0];
				map.tar_y = target_area[i][1];
			} else {
				map.tar_x = map.goal_x;
				map.tar_y = map.goal_y;
			}

			spec.run_interruption = 0;

			spec.tire_dim = 50.6; //[mm]
			spec.step_dist = spec.tire_dim * 3.1415926
					* (spec.step_angle / 360.0); //[mm]

			spec.kp_l = 0.35;
			spec.kp_r = 0.35;
			spec.kd_l = 0.7;
			spec.kd_r = 0.7;
			UX_effect(alart);

			PE.DRL.BIT.B2 = 0; //reset (0 : off, 1 : on)
			switch_Motor(on);
			wait_ms(1000);
			move_half_400(off);

			while (spec.run_interruption != 1) {
				vehicle.dist = 0;
				init_A_dist_map();
				update_Wall_map();
				update_A_dist_map();
				if (spec.run_interruption == 2) {
					break;
				}
				map.tmp_path = generate_A_path();
				if (map.tmp_path == R_small) {
					map.direction += 1;
					move_Right(vehicle.dist);
				} else if (map.tmp_path == L_small) {
					map.direction += 3;
					move_Left(vehicle.dist);
				} else if (map.tmp_path == Forward) {
					map.direction += 0;
					move_Forward(vehicle.dist, 650);
				} else if (map.tmp_path == Backward) {
					map.direction += 2;
					if (map.wall == 7 || map.wall == 11 || map.wall == 13
							|| map.wall == 14) {
						move_Backward(vehicle.dist);
					} else {
						move_Backward_2(vehicle.dist);
					}
				}
				map.direction %= 4;
				detect_Direction();

				if (check_pos() == 1) {
					if (map.pos_x == 0 && map.pos_y == 0) {
						spec.run_interruption = 1;
					} else {
						if (searchtimes == 0) {
							i += 1;
							map.tar_x = target_area[i][0];
							map.tar_y = target_area[i][1];
						} else {
							map.tar_x = 0;
							map.tar_y = 0;
						}
					}
				}

			}

			if (spec.run_interruption == 1) {
				update_Wall_map();
				move_half_400(on);
				wait_ms(300);
				drv_Motor(0.0, 0.0, 0.0, 180.0, 330.0, 0.0, 1000.0, right, on);
				drv_Status_LED(Green, on);
				wait_ms(50);
				drv_Motor(70, 230.0, spec.motor_min_vel, 0.0, 0.0, 0.0, 1200.0,
						back, on);

				wait_ms(300);
				switch_Motor(off);
				spec.sta_LED_flag = 0;
				map.pos_x = 0;
				map.pos_y = 0;
				map.tar_x = map.goal_x;
				map.tar_y = map.goal_y;
				map.direction = 0;
				init_Path();
				init_Dist_map();
				update_Dist_map();
				generate_Path();
				searchtimes = 1;
				Switch.rot_sw = astar;
			} else if (spec.run_interruption == 2) {
				move_half(on, vehicle.dist);
				wait_ms(300);
				UX_effect(error);
				UX_effect(error);
				UX_effect(error);
			}
			break;

		case show_map:
			map.tar_x = map.goal_x;
			map.tar_y = map.goal_y;
			spec.sta_LED_flag = 0;
			map.direction = 0;
			UX_effect(alart);
			dump_Logger();
			print_Wall_map();
			print_Searched_map();
			print_Mixed_map();
			print_Dist_map();
			init_Path();
			init_Dist_map();
			update_Dist_map();
			generate_Path();
			print_Path();
			break;

		case test:
//
			UX_effect(alart);
			switch_Motor(on);
			spec.kp_l = 0.5;
			spec.kp_r = 0.5;
			spec.sta_LED_flag = 0;
			wait_ms(300);
			spec.tire_dim = 51.0; //[mm]
			spec.step_dist = spec.tire_dim * 3.1415926
					* (spec.step_angle / 360.0); //[mm]
			drv_Status_LED(Rst_status_LED, off);

//			drv_Motor(spec.full_block * 6, 750.0, spec.motor_min_vel, 0.0,
//					0.0, 0.0, 1800.0, straight, on);

//			for (i = 0; i < 16; i++) {
//				drv_Motor(0.0, 0.0, 0.0, 90.0, 330.0, 0.0, 750.0, right, on);
//				wait_ms(300);
//			}

//			drv_Motor(spec.half_block, 600.0, 600.0, 0.0, 0.0, 0.0, 1800.0,
//					straight, off);
//			for (i = 0; i < 4; i++) {
//				drv_Motor(spec.full_block, 600.0, 600.0, 0.0, 0.0, 0.0, 1800.0,
//						straight, off);
//				move_Left_400(&params[2]);
//			}
//			drv_Motor(spec.full_block, 600.0, 600, 0.0, 0.0, 0.0, 1800.0,
//					straight, off);
//			drv_Motor(spec.half_block, 600.0, spec.motor_min_vel, 0.0, 0.0, 0.0,
//					1800.0, straight, on);
//
			drv_Motor(spec.half_block * 3, 750.0, 750.0, 0.0, 0.0, 0.0, 1800.0,
					straight, off);
			for (i = 0; i < 1; i++) {
				spec.tire_dim = 51.0; //[mm]
				spec.step_dist = spec.tire_dim * 3.1415926
						* (spec.step_angle / 360.0); //[mm]

				drv_Motor(spec.full_block, 750.0, 750, 0.0, 0.0, 0.0, 1800.0,
						straight, off);

				spec.tire_dim = 50.4; //[mm]
				spec.step_dist = spec.tire_dim * 3.1415926
						* (spec.step_angle / 360.0); //[mm]

				move_Right_90_s(3, &params[2]);
			}
			spec.tire_dim = 51.0; //[mm]
			spec.step_dist = spec.tire_dim * 3.1415926
					* (spec.step_angle / 360.0); //[mm]

			drv_Motor(spec.full_block, 750.0, 750, 0.0, 0.0, 0.0, 1800.0,
					straight, off);
			drv_Motor(spec.half_block * 3, 750.0, spec.motor_min_vel, 0.0, 0.0,
					0.0, 1800.0, straight, on);

			wait_ms(300);
			drv_Status_LED(Red, off);
			drv_Status_LED(Green, off);
			logger.run = 0; //stop logger
			spec.sta_LED_flag = 0;
			break;

//			drv_Status_LED(Rst_status_LED, off);
//			UX_effect(alart);
//			switch_Motor(on);
//			wait_ms(10000);
//			switch_Motor(off);
//			break;

		}

		switch_Motor(off);

	}

}

