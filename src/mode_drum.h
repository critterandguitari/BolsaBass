/*
 * mode_drum.h
 *
 *  Created on: Jul 21, 2012
 *      Author: owen
 */

#ifndef MODE_DRUM_H_
#define MODE_DRUM_H_

#include "arm_math.h"

void mode_drum_init(void);

float32_t mode_drum_sample_process (void) ;

void mode_drum_control_process (void);



#endif /* MODE_DRUM_H_ */
