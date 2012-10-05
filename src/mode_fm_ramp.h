/*
 * mode_fm_ramp.h
 *
 *  Created on: Oct 4, 2012
 *      Author: owen
 */

#ifndef MODE_FM_RAMP_H_
#define MODE_FM_RAMP_H_


#include "arm_math.h"

void mode_fm_ramp_init(void);

float32_t mode_fm_ramp_sample_process (void) ;

void mode_fm_ramp_control_process (void);


#endif /* MODE_FM_RAMP_H_ */
