/*
 * map.c
 *
 *  Created on: 2018/04/14
 *      Author: shts
 */
#include "iodefine.h"
#include "map.h"
#include "LED.h"
#include "sensor.h"
#include "serial.h"

MAP map;

extern SEN r_sen, l_sen, cr_sen, cl_sen;

void init_Map(void) {
	int i, j;

	map.pos_x = 0;
	map.pos_y = 0;
	map.direction = 0;
	map.goal_x = 1;
	map.goal_y = 0;
	map.tar_x = map.goal_x;
	map.tar_y = map.goal_y;
	map.map_size = mp_size;
	map.tmp_path = 0;
	map.wall = 0;

	for (i = 0; i < map.map_size - 1; i++) {
		map.wall_map_x[i] = 0;
		map.wall_map_y[i] = 0;
		map.searched_map_x[i] = 0xffffffff;
		map.searched_map_y[i] = 0xffffffff;
	}

	map.wall_map_x[0] = 1;

	for (i = 0; i < map.map_size; i++) {
		for (j = 0; j < map.map_size; j++) {
			map.a_dist_map[i][j] = 255;
			map.dist_map[i][j] = 255;
		}
	}

	for (i = 0; i < 2; i++) {
		for (j = 0; j < 2; j++) {
			map.a_dist_map[map.tar_x + i][map.tar_y + j] = 0;
			map.dist_map[map.tar_x + i][map.tar_y + j] = 0;
		}
	}
}

void detect_Direction() {
	if (map.direction == 0) {
		map.pos_y += 1;
		/*
		 sta_LED_drv(Red, off);
		 sta_LED_drv(Yerrow, off);
		 sta_LED_drv(Green, on);
		 */
	} else if (map.direction == 1) {
		map.pos_x += 1;
		/*
		 sta_LED_drv(Red, on);
		 sta_LED_drv(Yerrow, off);
		 sta_LED_drv(Green, off);
		 */
	} else if (map.direction == 3) {
		map.pos_x -= 1;
		/*
		 sta_LED_drv(Red, off);
		 sta_LED_drv(Yerrow, on);
		 sta_LED_drv(Green, off);
		 */
	} else if (map.direction == 2) {
		map.pos_y -= 1;
		/*
		 sta_LED_drv(Red, off);
		 sta_LED_drv(Yerrow, off);
		 sta_LED_drv(Green, off);
		 */
	}
	drv_Status_LED(Red, map.pos_y & 1);
	drv_Status_LED(Yerrow, (map.pos_x & 1) >> 0);
	drv_Status_LED(Green, (map.pos_x & 2) >> 1);

}

void update_Wall_map() {
	/*
	 * wall : W-S-E-N
	 * 		 MSB   LSB
	 *
	 * map_x : vertical walls to x-axis
	 * map_y : vertical walls to y-axis
	 * */

	map.wall = 0;
	if (r_sen.sen > r_sen.non_threshold) {
		map.wall |= 1 << ((1 + map.direction) % 4);
	}
	if (l_sen.sen > l_sen.non_threshold) {
		map.wall |= 1 << ((3 + map.direction) % 4);
	}
	if (cl_sen.sen > cl_sen.non_threshold) {
		map.wall |= 1 << ((0 + map.direction) % 4);
	}

	if (map.pos_x < map.map_size - 1) {
		map.wall_map_x[map.pos_x] |= ((map.wall & 2) >> 1) << map.pos_y;
		map.searched_map_x[map.pos_x] &= ~(1 << map.pos_y);
	}
	if (map.pos_x - 1 >= 0) {
		map.wall_map_x[map.pos_x - 1] |= ((map.wall & 8) >> 3) << map.pos_y;
		map.searched_map_x[map.pos_x - 1] &= ~(1 << map.pos_y);
	}

	if (map.pos_y < map.map_size - 1) {
		map.wall_map_y[map.pos_y] |= ((map.wall & 1) >> 0) << map.pos_x;
		map.searched_map_y[map.pos_y] &= ~(1 << map.pos_x);
	}
	if (map.pos_y - 1 >= 0) {
		map.wall_map_y[map.pos_y - 1] |= ((map.wall & 4) >> 2) << map.pos_x;
		map.searched_map_y[map.pos_y - 1] &= ~(1 << map.pos_x);
	}
}

void print_Wall_map_x(int row) {
	int i, mask = 1;
	mask <<= row;

	for (i = 0; i < map.map_size - 1; i++) {
		myprintf("  ");
		if (map.wall_map_x[i] & mask) {
			myprintf("|");
		} else {
			myprintf(" ");
		}
	}

}

void print_Wall_map_y(int row) {
	int i, mask;
	for (i = 0; i < map.map_size - 1; i++) {
		mask = 1 << i;
		if (map.wall_map_y[row] & mask) {
			myprintf("--");
		} else {
			myprintf("  ");
		}
		myprintf("+");
	}
	mask <<= 1;
	if (map.wall_map_y[row] & mask) {
		myprintf("--");
	} else {
		myprintf("  ");
	}

}

void print_Wall_map() {
	int i;
	myprintf("\n");
	for (i = 0; i < map.map_size; i++) {
		myprintf("+--");
	}
	myprintf("+\n|");

	print_Wall_map_x(map.map_size - 1);
	myprintf("  |\n+");
	for (i = 1; i < map.map_size; i++) {
		print_Wall_map_y(map.map_size - 1 - i);
		myprintf("+\n|");
		print_Wall_map_x(map.map_size - 1 - i);
		myprintf("  |\n+");
	}

	for (i = 0; i < map.map_size; i++) {
		myprintf("--+");
	}
	myprintf("\n\n");
}

char read_Wall_map(char x, char y) {

	/*
	 *  wall : W-S-E-N
	 * 		 MSB   LSB
	 */

	char wall = 0;

	//myprintf("\n\n");

	if (x < map.map_size - 1) {
		wall |= ((map.wall_map_x[x] & (1 << y)) >> y) << 1;
		//myprintf("%d\n", ((mixed_map_x[x] & (1 << y)) >> y) << 1);
	}
	if (x - 1 >= 0) {
		wall |= ((map.wall_map_x[x - 1] & (1 << y)) >> y) << 3;
		//myprintf("%d\n", ((mixed_map_x[x - 1] & (1 << y)) >> y) << 3);
	}

	if (y < map.map_size - 1) {
		wall |= (map.wall_map_y[y] & (1 << x)) >> x;
		//myprintf("%d\n", (mixed_map_y[y] & (1 << x)) >> x);
	}
	if (y - 1 >= 0) {
		wall |= ((map.wall_map_y[y - 1] & (1 << x)) >> x) << 2;
		//myprintf("%d\n", ((mixed_map_y[y - 1] & (1 << x)) >> x) << 2);
	}
	return wall;
}

void print_Searched_map_x(int row) {
	int i, mask = 1;
	mask <<= row;

	for (i = 0; i < map.map_size - 1; i++) {
		myprintf("  ");
		if (map.searched_map_x[i] & mask) {
			myprintf("|");
		} else {
			myprintf(" ");
		}
	}

}

void print_Searched_map_y(int row) {
	int i, mask;
	for (i = 0; i < map.map_size - 1; i++) {
		mask = 1 << i;
		if (map.searched_map_y[row] & mask) {
			myprintf("--");
		} else {
			myprintf("  ");
		}
		myprintf("+");
	}
	mask <<= 1;
	if (map.searched_map_y[row] & mask) {
		myprintf("--");
	} else {
		myprintf("  ");
	}

}

void print_Searched_map() {
	int i;
	myprintf("\n");
	for (i = 0; i < map.map_size; i++) {
		myprintf("+--");
	}
	myprintf("+\n|");

	print_Searched_map_x(map.map_size - 1);
	myprintf("  |\n+");
	for (i = 1; i < map.map_size; i++) {
		print_Searched_map_y(map.map_size - 1 - i);
		myprintf("+\n|");
		print_Searched_map_x(map.map_size - 1 - i);
		myprintf("  |\n+");
	}

	for (i = 0; i < map.map_size; i++) {
		myprintf("--+");
	}
	myprintf("\n\n");
}

void mix_Map() {
	int i;
	for (i = 0; i < map.map_size - 1; i++) {
		map.mixed_map_x[i] = map.wall_map_x[i] | map.searched_map_x[i];
		map.mixed_map_y[i] = map.wall_map_y[i] | map.searched_map_y[i];
	}
}

void print_Mixed_map_x(int row) {
	int i, mask = 1;
	mask <<= row;

	for (i = 0; i < map.map_size - 1; i++) {
		myprintf("  ");
		if (map.mixed_map_x[i] & mask) {
			myprintf("|");
		} else {
			myprintf(" ");
		}
	}

}

void print_Mixed_map_y(int row) {
	int i, mask;
	for (i = 0; i < map.map_size - 1; i++) {
		mask = 1 << i;
		if (map.mixed_map_y[row] & mask) {
			myprintf("--");
		} else {
			myprintf("  ");
		}
		myprintf("+");
	}
	mask <<= 1;
	if (map.mixed_map_y[row] & mask) {
		myprintf("--");
	} else {
		myprintf("  ");
	}

}

void print_Mixed_map() {
	int i;
	myprintf("\n");
	for (i = 0; i < map.map_size; i++) {
		myprintf("+--");
	}
	myprintf("+\n|");

	print_Mixed_map_x(map.map_size - 1);
	myprintf("  |\n+");
	for (i = 1; i < map.map_size; i++) {
		print_Mixed_map_y(map.map_size - 1 - i);
		myprintf("+\n|");
		print_Mixed_map_x(map.map_size - 1 - i);
		myprintf("  |\n+");
	}

	for (i = 0; i < map.map_size; i++) {
		myprintf("--+");
	}
	myprintf("\n\n");
}

char read_Mixed_map(char x, char y) {

	/*
	 *  wall : W-S-E-N
	 * 		 MSB   LSB
	 */

	char wall = 0;

	myprintf("\n\n");

	if (x < map.map_size - 1) {
		wall |= ((map.mixed_map_x[x] & (1 << y)) >> y) << 1;
		//myprintf("%d\n", ((mixed_map_x[x] & (1 << y)) >> y) << 1);
	}
	if (x - 1 >= 0) {
		wall |= ((map.mixed_map_x[x - 1] & (1 << y)) >> y) << 3;
		//myprintf("%d\n", ((mixed_map_x[x - 1] & (1 << y)) >> y) << 3);
	}

	if (y < map.map_size - 1) {
		wall |= (map.mixed_map_y[y] & (1 << x)) >> x;
		//myprintf("%d\n", (mixed_map_y[y] & (1 << x)) >> x);
	}
	if (y - 1 >= 0) {
		wall |= ((map.mixed_map_y[y - 1] & (1 << x)) >> x) << 2;
		//myprintf("%d\n", ((mixed_map_y[y - 1] & (1 << x)) >> x) << 2);
	}
	return wall;

}

void update_A_dist_map() {
	unsigned char buff_x = 0, buff_y = 0, wall, dist = 0;
	int i, j, k;

	while (map.a_dist_map[map.pos_x][map.pos_y] == 255) {
		for (i = 0; i < map.map_size; i++) {
			for (j = 0; j < map.map_size; j++) {
				if (map.a_dist_map[i][j] == dist) {
					buff_x = i;
					buff_y = j;
					wall = read_Wall_map(buff_x, buff_y);

					if ((wall & 2) != 2) {
						if (buff_x != (map.map_size - 1)) {
							if (map.a_dist_map[buff_x + 1][buff_y] == 255) {
								map.a_dist_map[buff_x + 1][buff_y] = dist + 1;
							}
						}
					}
					if ((wall & 8) != 8) {
						if (buff_x != 0) {
							if (map.a_dist_map[buff_x - 1][buff_y] == 255) {
								map.a_dist_map[buff_x - 1][buff_y] = dist + 1;
							}
						}
					}

					if ((wall & 1) != 1) {
						if (buff_y != (map.map_size - 1)) {
							if (map.a_dist_map[buff_x][buff_y + 1] == 255) {
								map.a_dist_map[buff_x][buff_y + 1] = dist + 1;
							}
						}
					}

					if ((wall & 4) != 4) {
						if (buff_y != 0) {
							if (map.a_dist_map[buff_x][buff_y - 1] == 255) {
								map.a_dist_map[buff_x][buff_y - 1] = dist + 1;
							}
						}
					}
				}
			}
		}
		dist += 1;
	}

}

char generate_A_path() {
	/*
	 * =================
	 * about path[]
	 * 0:forward
	 * 1:right
	 * 2:backward
	 * 3:left
	 * 4:goal
	 * =================
	 * wall : W-S-E-N
	 * 		 MSB   LSB
	 * =================
	 * dir : 0 North
	 * 		 1 East
	 * 		 2 South
	 * 		 3 West
	 * =================
	 * */
	char x = map.pos_x, y = map.pos_y, dir = map.direction, rel_dir, wall, dist,
			min_dist, pri_flag;
	int i = 0;

	wall = read_Wall_map(x, y);
	dist = map.a_dist_map[x][y];
	pri_flag = 4;

	if ((wall & 4) == 0) {
		if (y - 1 >= 0) {
			if (map.a_dist_map[x][y - 1] <= dist) {
				dist = map.a_dist_map[x][y - 1];
				min_dist = 2;
				if (min_dist == dir) {
					pri_flag = dir;
				} else if (pri_flag != 4) {
					pri_flag = min_dist;
				}
			}
		}
	}

	if ((wall & 2) == 0) {
		if (x + 1 < map.map_size) {
			if (map.a_dist_map[x + 1][y] <= dist) {
				dist = map.a_dist_map[x + 1][y];
				min_dist = 1;
				if (min_dist == dir) {
					pri_flag = dir;
				} else if (pri_flag != 4) {
					pri_flag = min_dist;
				}
			}
		}
	}

	if ((wall & 8) == 0) {
		if (x - 1 >= 0) {
			if (map.a_dist_map[x - 1][y] <= dist) {
				dist = map.a_dist_map[x - 1][y];
				min_dist = 3;
				if (min_dist == dir) {
					pri_flag = dir;
				} else if (pri_flag != 4) {
					pri_flag = min_dist;
				}
			}
		}
	}

	if ((wall & 1) == 0) {
		if (y + 1 < map.map_size) {
			if (map.a_dist_map[x][y + 1] <= dist) {
				dist = map.a_dist_map[x][y + 1];
				min_dist = 0;
				if (min_dist == dir) {
					pri_flag = dir;
				} else if (pri_flag != 4) {
					pri_flag = min_dist;
				}
			}
		}
	}

	if (pri_flag != 4) {
		min_dist = pri_flag; //keep forward if some distance takes minimum distance
	}

	rel_dir = min_dist - dir; //convert absolute direction to relative direction
	if (rel_dir < 0) {
		rel_dir += 4;
	}
	return rel_dir;
}

void init_A_dist_map() {
	int i, j;

	for (i = 0; i < map.map_size; i++) {
		for (j = 0; j < map.map_size; j++) {
			map.a_dist_map[i][j] = 255;
		}
	}

	if (map.tar_x == 0 && map.tar_y == 0) {
		map.a_dist_map[map.tar_x][map.tar_y] = 0;
	} else {
		for (i = 0; i < 2; i++) {
			for (j = 0; j < 2; j++) {
				map.a_dist_map[map.tar_x + i][map.tar_y + j] = 0;
			}
		}
	}

}

void init_Dist_map() {
	int i, j;

	for (i = 0; i < map.map_size; i++) {
		for (j = 0; j < map.map_size; j++) {
			map.dist_map[i][j] = 255;
		}
	}

	if (map.tar_x == 0 && map.tar_y == 0) {
		map.dist_map[map.tar_x][map.tar_y] = 0;
	} else {
		for (i = 0; i < 2; i++) {
			for (j = 0; j < 2; j++) {
				map.dist_map[map.tar_x + i][map.tar_y + j] = 0;
			}
		}
	}

}

void print_Dist_map() {
	int i, j;

	myprintf("\n");
	for (i = 0; i < map.map_size; i++) {
		for (j = 0; j < map.map_size; j++) {
			myprintf("%3d ", map.dist_map[j][map.map_size - 1 - i]);
		}
		myprintf("\n");
	}
}

char read_Dist_map(char x, char y) {
	return map.dist_map[x][y];
}

void update_Dist_map() {
	unsigned char buff_x = 0, buff_y = 0, wall, dist = 0;
	int i, j, k;
	mix_Map();
//TODO	print_Mixed_map();
	while (map.dist_map[0][0] == 255) {
		for (i = 0; i < map.map_size; i++) {
			for (j = 0; j < map.map_size; j++) {
				if (map.dist_map[i][j] == dist) {
					buff_x = i;
					buff_y = j;
					wall = read_Mixed_map(buff_x, buff_y);

					if ((wall & 2) != 2) {
						if (buff_x != (map.map_size - 1)) {
							if (map.dist_map[buff_x + 1][buff_y] == 255) {
								map.dist_map[buff_x + 1][buff_y] = dist + 1;
							}
						}
					}
					if ((wall & 8) != 8) {
						if (buff_x != 0) {
							if (map.dist_map[buff_x - 1][buff_y] == 255) {
								map.dist_map[buff_x - 1][buff_y] = dist + 1;
							}
						}
					}

					if ((wall & 1) != 1) {
						if (buff_y != (map.map_size - 1)) {
							if (map.dist_map[buff_x][buff_y + 1] == 255) {
								map.dist_map[buff_x][buff_y + 1] = dist + 1;
							}
						}
					}

					if ((wall & 4) != 4) {
						if (buff_y != 0) {
							if (map.dist_map[buff_x][buff_y - 1] == 255) {
								map.dist_map[buff_x][buff_y - 1] = dist + 1;
							}
						}
					}
				}
			}
		}
//TODO print_Dist_map();
		dist += 1;
	}
}

void generate_Path() {
	/*
	 * =================
	 * about path[]
	 * 0:forward
	 * 1:right
	 * 2:backward
	 * 3:left
	 * 4:goal
	 * 5:right180
	 * 6:left180
	 * 7:right90
	 * 8:left90
	 * =================
	 * wall : W-S-E-N
	 * 		 MSB   LSB
	 * =================
	 * dir : 0 North
	 * 		 1 East
	 * 		 2 South
	 * 		 3 West
	 * =================
	 * */
	char x = 0, y = 0, dir = map.direction, rel_dir = map.direction, wall, dist,
			min_dist, pri_flag;
	int i = 0, j = 0;

//TODO	myprintf("=======\n");

	while (map.dist_map[x][y] != 0) {
		wall = read_Mixed_map(x, y);
		dist = map.dist_map[x][y];
		pri_flag = 4;

		if ((wall & 1) == 0) {
			if (y + 1 < map.map_size) {
				if (map.dist_map[x][y + 1] <= dist) {
					dist = map.dist_map[x][y + 1];
					min_dist = 0;
					if (dir == min_dist) {
						pri_flag = dir;
					}
				}

			}
		}
		if ((wall & 2) == 0) {
			if (x + 1 < map.map_size) {
				if (map.dist_map[x + 1][y] <= dist) {
					dist = map.dist_map[x + 1][y];
					min_dist = 1;
					if (dir == min_dist) {
						pri_flag = dir;
					}
				}

			}
		}
		if ((wall & 4) == 0) {
			if (y - 1 >= 0) {
				if (map.dist_map[x][y - 1] <= dist) {
					dist = map.dist_map[x][y - 1];
					min_dist = 2;
					if (dir == min_dist) {
						pri_flag = dir;
					}
				}

			}
		}
		if ((wall & 8) == 0) {
			if (x - 1 >= 0) {
				if (map.dist_map[x - 1][y] <= dist) {
					dist = map.dist_map[x - 1][y];
					min_dist = 3;
					if (dir == min_dist) {
						pri_flag = dir;
					}
				}

			}
		}

		if (pri_flag != 4) {
			min_dist = pri_flag;
//TODO			myprintf("====\n");
		}

		if (min_dist == 0) {
			y += 1;
		} else if (min_dist == 1) {
			x += 1;
		} else if (min_dist == 2) {
			y -= 1;
		} else if (min_dist == 3) {
			x -= 1;
		}

		rel_dir = min_dist - dir;
		if (rel_dir < 0) {
			rel_dir += 4;
		}

		dir = min_dist;

//TODO		myprintf("%d\n", rel_dir);
//		map.path[i] = rel_dir;

		if (map.path_test[i - 1].index == rel_dir && i > 0 && rel_dir == 0) {
			map.path_test[i - 1].block_num += 1;
		} else if (rel_dir == 0 && map.path_test[i - 1].index == 1
				&& map.path_test[i - 2].index == 1
				&& map.path_test[i - 3].index == 0) {
			i -= 2;
			map.path_test[i - 1].block_num -= 1;
			map.path_test[i].block_num = 1;
			map.path_test[i].index = 5;
			i += 1;
		} else if (rel_dir == 0 && map.path_test[i - 1].index == 3
				&& map.path_test[i - 2].index == 3
				&& map.path_test[i - 3].index == 0) {
			i -= 2;
			map.path_test[i - 1].block_num -= 1;
			map.path_test[i].block_num = 1;
			map.path_test[i].index = 6;
			i += 1;
		} else {
			map.path_test[i].index = rel_dir;
			map.path_test[i].block_num = 1;
			i += 1;
		}
//TODO
		myprintf("(%d,%d) %d\n", map.path_test[i - 1].index,
				map.path_test[i - 1].block_num, i - 1);
	}
	map.path_test[0].block_num -= 1;
	map.path_test[i].index = 0;
	map.path_test[i].block_num = 1;
}

void init_Path() {
	int i;
	for (i = 0; i < map.map_size * map.map_size; i++) {
//		map.path[i] = 4;
		map.path_test[i].block_num = 0;
		map.path_test[i].index = 4;
	}
}

void print_Path() {
	int i = 0;
	while (map.path_test[i].index != 4) {
		myprintf("(%d,%d)\n", map.path_test[i].index,
				map.path_test[i].block_num);
		i += 1;
	}
}

unsigned char check_pos() {
	char i, j, lim;
	if (map.tar_x == 0 && map.tar_y == 0) {
		lim = 1;
	} else {
		lim = 2;
	}

	for (i = 0; i < lim; i++) {
		for (j = 0; j < lim; j++) {
			if (map.pos_x == map.tar_x + i && map.pos_y == map.tar_y + j) {
				return 1;
			}
		}
	}
	return 0;
}
