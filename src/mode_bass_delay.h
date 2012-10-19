/*
 * mode_bass_delay.h
 *
 *  Created on: Oct 18, 2012
 *      Author: owen
 */

#ifndef MODE_BASS_DELAY_H_
#define MODE_BASS_DELAY_H_


#include "arm_math.h"

void mode_bass_delay_init(void);

float32_t mode_bass_delay_sample_process (void) ;

void mode_bass_delay_control_process (void);


#endif /* MODE_BASS_DELAY_H_ */
