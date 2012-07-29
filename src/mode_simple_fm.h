/*
 * mode_simple_fm.h
 *
 *  Created on: Jul 27, 2012
 *      Author: owen
 */

#ifndef MODE_SIMPLE_FM_H_
#define MODE_SIMPLE_FM_H_

#include "arm_math.h"

void mode_simple_fm_init(void);

float32_t mode_simple_fm_sample_process (void) ;

void mode_simple_fm_control_process (void);

#endif /* MODE_SIMPLE_FM_H_ */
