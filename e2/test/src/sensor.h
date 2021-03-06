/*
 * sensor.h
 *
 *  Created on: 2018/04/13
 *      Author: shts
 */

#ifndef SENSOR_H_
#define SENSOR_H_

enum AD_C {
	ad_0 = 0, ad_1 = 1
};
enum sensor {
	CR_sen = 0, R_sen, CL_sen, L_sen //センサの順番通り
};


typedef struct sen {
	short sen;//現在データ
	short old[9];//過去データ
	short ref_wall;//中央時のセンサ値
	short non_threshold;//壁なし
	short diff_threshold;//壁切れ
	short diff;//センサ差分
} SEN;

void init_Sensor(void);

void init_ADC(void);

int get_Sensor(int ch, int ad_c);


#endif /* SENSOR_H_ */
