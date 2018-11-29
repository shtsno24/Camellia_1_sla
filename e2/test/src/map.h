/*
 * map.h
 *
 *  Created on: 2018/04/14
 *      Author: shts
 */

#ifndef MAP_H_
#define MAP_H_

#define mp_size 16

enum move{
	Forward = 0,
	R_small = 1,
	Backward = 2,
	L_small = 3,
	End = 4,
	R_180 = 5,
	L_180 = 6,
	R_90 = 7,
	L_90 = 8
};

typedef struct path {
	unsigned char index :4;
	unsigned char block_num :4;
} PATH;

typedef struct map {
	unsigned char tar_x;
	unsigned char tar_y;
	unsigned char pos_x;
	unsigned char pos_y;
	unsigned char goal_x;
	unsigned char goal_y;
	unsigned char direction;
	unsigned char wall;
	unsigned char tmp_path;
	unsigned char map_size;
//	unsigned char path[mp_size * mp_size];
	PATH path_test[256];
	unsigned char dist_map[mp_size][mp_size];
	unsigned char a_dist_map[mp_size][mp_size];
	unsigned int wall_map_x[mp_size - 1];
	unsigned int wall_map_y[mp_size - 1];
	unsigned int searched_map_x[mp_size - 1];
	unsigned int searched_map_y[mp_size - 1];
	unsigned int mixed_map_x[mp_size - 1];
	unsigned int mixed_map_y[mp_size - 1];
} MAP;

void init_Map();

void detect_Direction();

void update_Wall_map();

void print_Wall_map_x(int);

void print_Wall_map_y(int);

void print_Wall_map();

char read_Wall_map(char, char);

void print_Searched_map_x(int);

void print_Searched_map_y(int);

void print_Searched_map();

void mix_Map();

void print_Mixed_map_x(int);

void print_Mixed_map_y(int);

void print_Mixed_map();

char read_Mixed_map(char, char);

void update_A_dist_map();

char generate_A_path();

void init_A_dist_map();

void print_Dist_map();

char read_Dist_map(char, char);

void update_Dist_map();

void generate_Path();

void init_Path();

void print_Path();

unsigned char check_pos();

#endif /* MAP_H_ */
