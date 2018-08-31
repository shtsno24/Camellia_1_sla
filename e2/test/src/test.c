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

#define round(A)((int)(A + 0.5))

int route_index, i;

extern SPC spec;
extern SEN r_sen, cr_sen, l_sen, cl_sen;
extern MOT r_motor, l_motor;
extern VEH vehicle;
extern SW Switch;
extern CMT_01 tim;
extern MAP map;

enum mode {
	astar_sla = 0, astar = 1, show_map = 2, test = 3, run = 4
};

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
		wait_ms(100);
		drv_Status_LED(Rst_status_LED, off);
		switch (Switch.rot_sw) {

		case astar_sla:
			spec.sta_LED_flag = 0;
			map.pos_x = 0;
			map.pos_y = 1;
			map.tar_x = map.goal_x;
			map.tar_y = map.goal_y;
			map.direction = 0;
			spec.run_interruption = 0;
			UX_effect(alart);

			switch_Motor(on);
			wait_ms(100);
			move_half_450(off);

			while (spec.run_interruption != 1) {
				update_Wall_map();
				update_A_dist_map();
				map.tmp_path = generate_A_path();
				init_A_dist_map();

				if (map.tmp_path == 1) {
					map.direction += 1;
					move_Right_450();
				} else if (map.tmp_path == 3) {
					map.direction += 3;
					move_Left_450();
				} else if (map.tmp_path == 0) {
					map.direction += 0;
					move_Forward_450();
				} else if (map.tmp_path == 2) {
					map.direction += 2;
					if (map.wall == 7 || map.wall == 11 || map.wall == 13
							|| map.wall == 14) {
						move_Backward();
					} else {
						move_Backward_2();
					}
				}
				map.direction %= 4;

				detect_Direction();
				if (map.pos_x == map.tar_x & map.pos_y == map.tar_y) {
					spec.run_interruption = 1;
				}
			}
			update_Wall_map();
			move_Backward_break();
			map.direction += 2;
			map.direction %= 4;
			detect_Direction();

			map.tar_x = 0;
			map.tar_y = 0;
			init_Path();
			init_Dist_map();
			update_Dist_map();
			generate_Path();

			spec.run_interruption = 0;

			wait_ms(300);

			UX_effect(alart);
//			move_half_450(off);

			while (spec.run_interruption != 1) {
				update_Wall_map();
				update_A_dist_map();
				map.tmp_path = generate_A_path();
				init_A_dist_map();

				if (map.tmp_path == 1) {
					map.direction += 1;
					move_Right_450();
				} else if (map.tmp_path == 3) {
					map.direction += 3;
					move_Left_450();
				} else if (map.tmp_path == 0) {
					map.direction += 0;
					move_Forward_450();
				} else if (map.tmp_path == 2) {
					map.direction += 2;
					if (map.wall == 7 || map.wall == 11 || map.wall == 13
							|| map.wall == 14) {
						move_Backward();
					} else {
						move_Backward_2();
					}
				}
				map.direction %= 4;

				detect_Direction();
				if (map.pos_x == map.tar_x & map.pos_y == map.tar_y) {
					spec.run_interruption = 1;
				}
			}
			update_Wall_map();
			move_half_450(on);
			wait_ms(300);
			drv_Motor(0.0, 0.0, 0.0, 159.0, 400.0, 3000.0, 2000.0, off, left);
			drv_Status_LED(Green, on);
			drv_Motor(0.0, 0.0, 0.0, 25.0, 1.0, 3000.0, 2000.0, on, left);
			wait_ms(300);
			drv_Motor(spec.half_block + 30.0, 200.0, 1000.0, 0.0, 0.0, 10.0,
					1000.0, on, back);
			wait_ms(300);

			switch_Motor(off);
			spec.sta_LED_flag = 0;
			map.pos_x = 0;
			map.pos_y = 0;
			init_Path();
			init_Dist_map();
			update_Dist_map();
			generate_Path();
			break;

		case run:
//			spec.sta_LED_flag = 0;
//			map.pos_x = 0;
//			map.pos_y = 1;
//			map.direction = 0;
//			spec.run_interruption = 0;
//			UX_effect(alart);
//			route_index = 1;

//			switch_Motor(on);
//			mot_app2(spec.half_block, 330, 2000, straight, on);
//
//			while (map.path[route_index] != 4) {
//				update_Wall_map();
//
//				if (map.path[route_index] == 1) {
//					map.direction += 1;
//					move_Right();
//
//				} else if (map.path[route_index] == 3) {
//					map.direction += 3;
//					move_Left();
//				} else if (map.path[route_index] == 0) {
//					map.direction += 0;
//					move_Forward();
//				} else {
//					map.direction += 2;
//					move_Backward_2();
//				}
//				map.direction %= 4;
//
//				detect_Direction();
//				route_index += 1;
//			}
//			update_Wall_map();
//			mot_app(spec.half_block, 310, 2000, straight, on);
			spec.sta_LED_flag = 0;
			map.pos_x = 0;
			map.pos_y = 1;
			map.tar_x = map.goal_x;
			map.tar_y = map.goal_y;
			map.direction = 0;
			spec.run_interruption = 0;
			UX_effect(alart);

			switch_Motor(on);
			wait_ms(100);
			move_half_450(off);

			while (spec.run_interruption != 1) {
				update_Wall_map();
				update_A_dist_map();
				map.tmp_path = generate_A_path();
				init_A_dist_map();

				if (map.tmp_path == 1) {
					map.direction += 1;
					move_Right();
				} else if (map.tmp_path == 3) {
					map.direction += 3;
					move_Left();
				} else if (map.tmp_path == 0) {
					map.direction += 0;
					move_Forward_450();
				} else if (map.tmp_path == 2) {
					map.direction += 2;
					if (map.wall == 7 || map.wall == 11 || map.wall == 13
							|| map.wall == 14) {
						move_Backward();
					} else {
						move_Backward_2();
					}
				}
				map.direction %= 4;

				detect_Direction();
				if (map.pos_x == map.tar_x & map.pos_y == map.tar_y) {
					spec.run_interruption = 1;
				}
			}
			update_Wall_map();
			move_Backward_break();

			wait_ms(300);
			switch_Motor(off);
			spec.sta_LED_flag = 0;
			map.pos_x = 0;
			map.pos_y = 0;
			init_Path();
			init_Dist_map();
			update_Dist_map();
			generate_Path();
			break;

		case astar:
			spec.sta_LED_flag = 0;
			map.pos_x = 0;
			map.pos_y = 1;
			map.tar_x = map.goal_x;
			map.tar_y = map.goal_y;
			map.direction = 0;
			spec.run_interruption = 0;
			UX_effect(alart);

			switch_Motor(on);
			wait_ms(100);
			move_half_450(off);

			while (spec.run_interruption != 1) {
				update_Wall_map();
				update_A_dist_map();
				map.tmp_path = generate_A_path();
				init_A_dist_map();

				if (map.tmp_path == 1) {
					map.direction += 1;
					move_Right();
				} else if (map.tmp_path == 3) {
					map.direction += 3;
					move_Left();
				} else if (map.tmp_path == 0) {
					map.direction += 0;
					move_Forward_450();
				} else if (map.tmp_path == 2) {
					map.direction += 2;
					if (map.wall == 7 || map.wall == 11 || map.wall == 13
							|| map.wall == 14) {
						move_Backward();
					} else {
						move_Backward_2();
					}
				}
				map.direction %= 4;

				detect_Direction();
				if (map.pos_x == map.tar_x & map.pos_y == map.tar_y) {
					spec.run_interruption = 1;
				}
			}
			update_Wall_map();
			move_Backward_break();
			map.direction += 2;
			map.direction %= 4;
			detect_Direction();

			map.tar_x = 0;
			map.tar_y = 0;
			init_Path();
			init_Dist_map();
			update_Dist_map();
			generate_Path();

			spec.run_interruption = 0;

			wait_ms(300);

			UX_effect(alart);
			//			move_half_450(off);

			while (spec.run_interruption != 1) {
				update_Wall_map();
				update_A_dist_map();
				map.tmp_path = generate_A_path();
				init_A_dist_map();

				if (map.tmp_path == 1) {
					map.direction += 1;
					move_Right();
				} else if (map.tmp_path == 3) {
					map.direction += 3;
					move_Left();
				} else if (map.tmp_path == 0) {
					map.direction += 0;
					move_Forward_450();
				} else if (map.tmp_path == 2) {
					map.direction += 2;
					if (map.wall == 7 || map.wall == 11 || map.wall == 13
							|| map.wall == 14) {
						move_Backward();
					} else {
						move_Backward_2();
					}
				}
				map.direction %= 4;

				detect_Direction();
				if (map.pos_x == map.tar_x & map.pos_y == map.tar_y) {
					spec.run_interruption = 1;
				}
			}
			update_Wall_map();
			move_half_450(on);
			wait_ms(300);
			drv_Motor(0.0, 0.0, 0.0, 159.0, 400.0, 3000.0, 2000.0, off, left);
			drv_Status_LED(Green, on);
			drv_Motor(0.0, 0.0, 0.0, 25.0, 1.0, 3000.0, 2000.0, on, left);
			wait_ms(300);
			drv_Motor(spec.half_block + 30.0, 200.0, 1000.0, 0.0, 0.0, 10.0,
					1000.0, on, back);
			wait_ms(300);

			switch_Motor(off);
			spec.sta_LED_flag = 0;
			map.pos_x = 0;
			map.pos_y = 0;
			init_Path();
			init_Dist_map();
			update_Dist_map();
			generate_Path();
			break;

		case show_map:
			map.tar_x = map.goal_x;
			map.tar_y = map.goal_y;
			spec.sta_LED_flag = 0;
			map.direction = 0;
			UX_effect(alart);
			print_Wall_map();
			print_Searched_map();
			init_Path();
			update_Dist_map();
			generate_Path();
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
			myprintf("%s\n",
					"l_motor.tar_vel, r_motor.tar_vel, vehicle.ang, vehicle.tar_ang, vehicle.rot_vel, vehicle.tar_rot_vel");
			spec.sta_LED_flag = 0;
			wait_ms(1000);
//			move_half_450(off);
//			for (i = 0; i < 4; i++) {
//				move_Right();
//			}
//			move_half_450(on);

			drv_Status_LED(Rst_status_LED, off);
			move_half_450(off);
			drv_Status_LED(Red, on);
			move_Forward_450();
			drv_Status_LED(Green, on);
			move_Left_450();
			move_Forward_450();
			drv_Status_LED(Red, on);
			move_half_450(on);
			drv_Status_LED(Green, on);
			wait_ms(300);

//			while (PB.DR.BIT.B5 != 0)
//				;
//			switch_Motor(on);
//			UX_effect(alart);

//			move_half_450(off);
//			drv_Status_LED(Red, on);
//			for (i = 0; i < 2; i++) {
//				move_Forward_450();
//			}
//			move_Backward();
//			for (i = 0; i < 2; i++) {
//				move_Forward_450();
//			}
//			move_half_450(on);
//
//			wait_ms(300);
//			switch_Motor(off);

//			move_Right();
//			move_Right();
//			drv_Motor(270.0, 500.0, 1000.0, 0.0, 0.0, 0.0, 1000.0, on,
//					straight);
//			drv_Motor(1500.0, 1300.0, 1000.0, 0.0, 0.0, 0.0, 1500.0, on,
//					straight);
			wait_ms(300);
			drv_Status_LED(Red, off);
			drv_Status_LED(Green, off);

			spec.sta_LED_flag = 0;

			break;
		}

		switch_Motor(off);

	}

}

