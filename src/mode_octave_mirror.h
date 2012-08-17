/*
 * mode_octave_mirror.h
 *
 *  Created on: Aug 15, 2012
 *      Author: owen
 */

#ifndef MODE_OCTAVE_MIRROR_H_
#define MODE_OCTAVE_MIRROR_H_

#include "arm_math.h"


void mode_octave_mirror_init(void);

float32_t mode_octave_mirror_sample_process (void) ;

void mode_octave_mirror_control_process (void);

#endif /* MODE_OCTAVE_MIRROR_H_ */
