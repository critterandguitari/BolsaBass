/*
 * mode_wave_adder.h
 *
 *  Created on: Aug 17, 2012
 *      Author: owen
 */

#ifndef MODE_WAVE_ADDER_H_
#define MODE_WAVE_ADDER_H_

#include "arm_math.h"

void mode_wave_adder_init(void);

float32_t mode_wave_adder_sample_process (void) ;

void mode_wave_adder_control_process (void);


#endif /* MODE_WAVE_ADDER_H_ */
