/*
 * fixfloat.c
 *
 *  Created on: 2018/09/30
 *      Author: shts
 */
#include "fixfloat.h"

q15 int2fixed(int val) {
	q15 out;
	out.byte = 0;
	out.byte = val << 16;
	return out;
}

int fixed2int(q15 val) {
	int out = 0;
	out = val.bit.integer;
	return out;
}


q15 qadd(q15 val1, q15 val2) {
	q15 out;
	out.byte = val1.byte + val2.byte;
	return out;
}

