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
unsigned char searchtimes = 0;
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
	target_area[0][0] = 9;
	target_area[0][1] = 0;

	target_area[1][0] = map.goal_x;
	target_area[1][1] = map.goal_y;

	target_area[2][0] = 1;
	target_area[2][1] = 4;

	target_area[3][0] = 0;
	target_area[3][1] = 0;
	//{{, map.goal_y}, {0, 0}};//[point][i=0;pos_x,i=1;pos_y]
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
		}

		drv_Status_LED(Red, off);

		while (PB.DR.BIT.B5 != 0) {
			get_Battery_Voltage();
			select_Mode();
			myprintf("batt : %f\n", spec.batt);
			myprintf("sen : %d | %d | %d | %d\n", l_sen.sen, cl_sen.sen,
					cr_sen.sen, r_sen.sen);
			myprintf("mode : %d\n", Switch.rot_sw);
			wait_ms(100);
		}
		UX_effect(click);
		wait_ms(100);
		drv_Status_LED(Rst_status_LED, off);
		switch (Switch.rot_sw) {

		case astar_sla:
			wait_ms(1000);
			while (PB.DR.BIT.B5 != 0) {
				myprintf("params : %d\n", Switch.rot_sw);
				select_Params(4);
				wait_ms(200);
			}
			wait_ms(100);
			drv_Status_LED(Rst_status_LED, off);

			spec.sta_LED_flag = 0;
			map.pos_x = 0;
			map.pos_y = 1;
			map.direction = 0;
			spec.run_interruption = 0;
			spec.tire_dim = 50.5; //[mm]
			spec.step_dist = spec.tire_dim * 3.1415926
					* (spec.step_angle / 360.0); //[mm]
			spec.kp_l = 0.35;
			spec.kp_r = 0.35;
			UX_effect(alart);
			route_index = 0;

			switch_Motor(on);
			wait_ms(1000);
			move_half_400(off);
			p = &params[Switch.rot_sw];

			while (map.path_test[route_index].index != End) {

				if (map.path_test[route_index].index == R_small) {
					move_Right_400(p);
				} else if (map.path_test[route_index].index == L_small) {
					move_Left_400(p);
				} else if (map.path_test[route_index].index == R_180) {
					move_Right_180_s(map.path_test[route_index].block_num, p);
				} else if (map.path_test[route_index].index == L_180) {
					move_Left_180_s(map.path_test[route_index].block_num, p);
				} else if (map.path_test[route_index].index == Forward) {
					if (map.path_test[route_index].block_num <= 4) {
						vel = p->straight.min_vel
								+ 100 * map.path_test[route_index].block_num;
						i = 0;
					} else {
						vel = p->straight.max_vel;
						i = 2;
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
							m_vel, 0.0, 0.0, 0.0, 1800.0, straight, off);
				} else {
					move_Backward_2(0);
				}
				route_index += 1;
			}
			move_half_400(on);

			wait_ms(300);
			switch_Motor(off);
			spec.sta_LED_flag = 0;
			break;
//			spec.sta_LED_flag = 0;
//			map.pos_x = 0;
//			map.pos_y = 1;
//			map.tar_x = map.goal_x;
//			map.tar_y = map.goal_y;
//			map.direction = 0;
//			spec.run_interruption = 0;
//			UX_effect(alart);
//
//			switch_Motor(on);
//			wait_ms(1000);
//			move_half_450(off);
//
//			while (spec.run_interruption != 1) {
//				init_A_dist_map();
//				update_Wall_map();
//				update_A_dist_map();
//				map.tmp_path = generate_A_path();
//
//				if (map.tmp_path == 1) {
//					map.direction += 1;
//					move_Right_400();
//				} else if (map.tmp_path == 3) {
//					map.direction += 3;
//					move_Left_400();
//				} else if (map.tmp_path == 0) {
//					map.direction += 0;
//					move_Forward_400();
//				} else if (map.tmp_path == 2) {
//					map.direction += 2;
//					if (map.wall == 7 || map.wall == 11 || map.wall == 13
//							|| map.wall == 14) {
//						move_Backward();
//					} else {
//						move_Backward_2();
//					}
//				}
//				map.direction %= 4;
//
//				detect_Direction();
//				if (map.pos_x == map.tar_x & map.pos_y == map.tar_y) {
//					spec.run_interruption = 1;
//				}
//			}
//			update_Wall_map();
//			move_Backward_break();
//			map.direction += 2;
//			map.direction %= 4;
//			detect_Direction();
//
//			map.tar_x = 0;
//			map.tar_y = 0;
//			update_Wall_map();
//			update_A_dist_map();
//			init_Path();
//			init_Dist_map();
//			update_Dist_map();
//			generate_Path();
//
//			spec.run_interruption = 0;
//
//			wait_ms(300);
//
//			UX_effect(alart);
//
//			while (spec.run_interruption != 1) {
//				init_A_dist_map();
//				update_Wall_map();
//				update_A_dist_map();
//				map.tmp_path = generate_A_path();
//
//				if (map.tmp_path == 1) {
//					map.direction += 1;
//					move_Right_400();
//				} else if (map.tmp_path == 3) {
//					map.direction += 3;
//					move_Left_400();
//				} else if (map.tmp_path == 0) {
//					map.direction += 0;
//					move_Forward_400();
//				} else if (map.tmp_path == 2) {
//					map.direction += 2;
//					if (map.wall == 7 || map.wall == 11 || map.wall == 13
//							|| map.wall == 14) {
//						move_Backward();
//					} else {
//						move_Backward_2();
//					}
//				}
//				map.direction %= 4;
//
//				detect_Direction();
//				if (map.pos_x == map.tar_x && map.pos_y == map.tar_y) {
//					spec.run_interruption = 1;
//				}
//			}
//			update_Wall_map();
//			move_half_400(on);
//			wait_ms(300);
//			drv_Motor(0.0, 0.0, 180.0, 400.0, 2000.0, on, left, on);
//			wait_ms(300);
//			drv_Motor(spec.half_block + 30.0, 200.0, 0.0, 0.0, 1000.0, on, back,
//					on);
//			wait_ms(300);
//
//			switch_Motor(off);
//			spec.sta_LED_flag = 0;
//			map.pos_x = 0;
//			map.pos_y = 0;
//			init_Path();
//			init_Dist_map();
//			update_Dist_map();
//			generate_Path();
//			break;

		case run:
			wait_ms(1000);
			Switch.rot_sw = 0;
			while (PB.DR.BIT.B5 != 0) {
				myprintf("params : %d\n", Switch.rot_sw);
				select_Params(4);
				wait_ms(200);
			}
			wait_ms(100);
			drv_Status_LED(Rst_status_LED, off);

			spec.sta_LED_flag = 0;
			map.pos_x = 0;
			map.pos_y = 1;
			map.direction = 0;
			spec.run_interruption = 0;
			spec.tire_dim = 50.5; //[mm]
			spec.step_dist = spec.tire_dim * 3.1415926
					* (spec.step_angle / 360.0); //[mm]
			spec.kp_l = 0.35;
			spec.kp_r = 0.35;
			UX_effect(alart);
			route_index = 0;

			switch_Motor(on);
			wait_ms(1000);
			move_half_400(off);
			p = &params[Switch.rot_sw];

			while (map.path_test[route_index].index != End) {

				if (map.path_test[route_index].index == R_small) {
					move_Right_400(p);
				} else if (map.path_test[route_index].index == L_small) {
					move_Left_400(p);
				} else if (map.path_test[route_index].index == R_180) {
					move_Right_180_s(map.path_test[route_index].block_num, p);
				} else if (map.path_test[route_index].index == L_180) {
					move_Left_180_s(map.path_test[route_index].block_num, p);
				} else if (map.path_test[route_index].index == Forward) {
					if (map.path_test[route_index].block_num <= 4) {
						vel = p->straight.min_vel
								+ 100 * map.path_test[route_index].block_num;
						i = 0;
					} else {
						vel = p->straight.max_vel;
						i = 2;
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
							m_vel, 0.0, 0.0, 0.0, 1800.0, straight, off);
				} else {
					move_Backward_2(0);
				}
				route_index += 1;
			}
			move_half_400(on);

			wait_ms(300);
			switch_Motor(off);
			spec.sta_LED_flag = 0;
			break;

		case astar:
			wait_ms(1000);
			Switch.rot_sw = 0;
			while (PB.DR.BIT.B5 != 0) {
				myprintf("params : %d\n", Switch.rot_sw);
				select_Params(2);
				wait_ms(200);
			}
			wait_ms(100);
			drv_Status_LED(Rst_status_LED, off);
			searchtimes = Switch.rot_sw;

			i = 0;
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
			spec.tire_dim = 50.5; //[mm]
			spec.step_dist = spec.tire_dim * 3.1415926
					* (spec.step_angle / 360.0); //[mm]
			spec.kp_l = 0.37;
			spec.kp_r = 0.37;
			UX_effect(alart);

			switch_Motor(on);
			wait_ms(1000);
			move_half_400(off);

			while (spec.run_interruption != 1) {
				vehicle.dist = 0;
				init_A_dist_map();
				update_Wall_map();
				update_A_dist_map();
				map.tmp_path = generate_A_path();
				OFFSET = vehicle.dist;
				if (map.tmp_path == R_small) {
					map.direction += 1;
					move_Right(OFFSET);
				} else if (map.tmp_path == L_small) {
					map.direction += 3;
					move_Left(OFFSET);
				} else if (map.tmp_path == Forward) {
					map.direction += 0;
					move_Forward(OFFSET);
				} else if (map.tmp_path == Backward) {
					map.direction += 2;
					if (map.wall == 7 || map.wall == 11 || map.wall == 13
							|| map.wall == 14) {
						move_Backward();
					} else {
						move_Backward_2(OFFSET);
					}
				}
				map.direction %= 4;
				detect_Direction();
//				if (check_pos() == 1) {
//					if (map.pos_x == 0 && map.pos_y == 0) {
//						spec.run_interruption = 1;
//					} else {
//						map.tar_x = 0;
//						map.tar_y = 0;
//					}
//				}

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
			update_Wall_map();
			move_half_400(on);
			wait_ms(300);
			drv_Motor(0.0, 0.0, 0.0, 180.0, 330.0, 0.0, 1000.0, right, on);
			drv_Status_LED(Green, on);
			wait_ms(50);
			drv_Motor(spec.full_block - 40, 180.0, spec.motor_min_vel, 0.0, 0.0,
					0.0, 1200.0, back, on);

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
			/*
			 UX_effect(alart);
			 switch_Motor(on);
			 wait_ms(100);
			 mot_app2(spec.half_block, 330, 2000, straight, on);
			 myprintf("mot_app2");
			 mot_app(spec.half_block, 330, 2000, straight, on);
			 myprintf("mot_app");
			 wait_ms(100);

			 switch_Motor(off);

			 while (PB.DR.BIT.B5 != 0)
			 ;
			 switch_Motor(on);
			 UX_effect(alart);
			 wait_ms(100);
			 mot_app(spec.r_distance, 250, 2000, right, on);
			 myprintf("mot_app");
			 wait_ms(100);
			 switch_Motor(off);

			 while (PB.DR.BIT.B5 != 0)
			 ;
			 switch_Motor(on);
			 UX_effect(alart);
			 wait_ms(100);
			 mot_app(spec.l_distance, 250, 2000, left, on);
			 myprintf("mot_app");
			 wait_ms(100);
			 switch_Motor(off);

			 while (PB.DR.BIT.B5 != 0)
			 ;
			 switch_Motor(on);
			 UX_effect(alart);
			 wait_ms(100);
			 mot_app2(spec.half_block, 330, 2000, straight, on);
			 mot_app2(spec.full_block * 1, 330, 2000, straight, on);
			 mot_app(spec.half_block, 330, 2000, straight, on);
			 wait_ms(100);
			 switch_Motor(off);

			 spec.sta_LED_flag = 0;
			 */

			UX_effect(alart);
			switch_Motor(on);
			spec.sta_LED_flag = 0;
			wait_ms(300);
//			logger.run = 1; //standby logger

//			move_half_400(off);
//			move_Forward_400();
//			move_Forward_400();
//			move_half_400(on);

//			spec.tire_dim = 50.5; //[mm]
//			spec.step_dist = spec.tire_dim * 3.1415926
//					* (spec.step_angle / 360.0); //[mm]
//			move_half_400(off);
//			for (i = 0; i < 1; i++) {
//				drv_Motor(spec.full_block*14, 650.0, 450.0, 0.0, 0.0, 0.0, 1200.0, straight,
//						off);
//			}
//			move_half_400(on);

//			drv_Motor(0.0, 0.0, 0.0, 90.0 * 12, 330.0, 0.0, 750.0, right, on);

//			drv_Status_LED(Rst_status_LED, off);
//			move_half_400(off);
//			move_Forward_450();
//			for (i = 0; i < 8; i++) {
//				drv_Status_LED(Red, on);
//				move_Left_450();
//				move_Forward_450();
//				drv_Status_LED(Rst_status_LED, off);
//			}
//			move_half_450(on);
//			drv_Status_LED(Green, on);

//			while (PB.DR.BIT.B5 != 0)
//				;
//			switch_Motor(on);
//			UX_effect(alart);

//			move_half_450(off);
//			drv_Status_LED(Red, on);
//			for (i = 0; i < 0; i++) {
//				move_Forward_450();
//			}
//			move_Backward();
//			for (i = 0; i < 0; i++) {
//				move_Forward_450();
//			}
//			move_half_450(on);

//			move_Right();
//			move_Right();
//			drv_Motor(270.0, 500.0, 1000.0, 0.0, 0.0, 0.0, 1000.0, on,
//					straight);
//			drv_Motor(1500.0, 1300.0, 1000.0, 0.0, 0.0, 0.0, 1500.0, on,
//					straight);

			spec.tire_dim = 50.5; //[mm]
			drv_Status_LED(Rst_status_LED, off);
			drv_Motor(spec.full_block, 650.0, 650.0, 0.0, 0.0, 0.0, 1200.0,
					straight, off);

//			move_Left_180_s(3);
			move_Left_400(&params[0]);

			drv_Motor(spec.full_block, 650.0, spec.motor_min_vel, 0.0, 0.0, 0.0,
					1200.0, straight, on);

			wait_ms(300);
			drv_Status_LED(Red, off);
			drv_Status_LED(Green, off);
			logger.run = 0; //stop logger
			spec.sta_LED_flag = 0;

			break;
		}

		switch_Motor(off);

	}

}

