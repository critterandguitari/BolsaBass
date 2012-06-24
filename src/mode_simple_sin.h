/*
 * mode_simple_sin.h
 *
 *  Created on: Jun 24, 2012
 *      Author: owen
 */

#ifndef MODE_SIMPLE_SIN_H_
#define MODE_SIMPLE_SIN_H_

#include "arm_math.h"

void mode_simple_sin_init(void);

float32_t mode_simple_sin_sample_process (void) ;

void mode_simple_sin_control_process (void);



#endif /* MODE_SIMPLE_SIN_H_ */
