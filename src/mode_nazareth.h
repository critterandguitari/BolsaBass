/*
 * mode_nazareth.h
 *
 *  Created on: Jul 13, 2012
 *      Author: owen
 */

#ifndef MODE_NAZARETH_H_
#define MODE_NAZARETH_H_
#include "arm_math.h"

void mode_nazareth_init(void);

float32_t mode_nazareth_sample_process (void) ;

void mode_nazareth_control_process (void);



#endif /* MODE_NAZARETH_H_ */
