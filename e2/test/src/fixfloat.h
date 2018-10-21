/*
 * fixfloat.h
 *
 *  Created on: 2018/09/28
 *      Author: shts
 */

#ifndef FIXFLOAT_H_
#define FIXFLOAT_H_

typedef union q15 {
	/*宣言は
	 * MSB
	 * ↓
	 * LSB
	 * の順で入る仕様
	 * */
	unsigned long byte;
	struct {
		long :4;
		long integer :16;
		long radix :4;
		long :4;
	} bit;
} q15;

#endif /* FIXFLOAT_H_ */
