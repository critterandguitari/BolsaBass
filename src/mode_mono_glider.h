/*
 * mode_mono_glider.h
 *
 *  Created on: Jul 5, 2012
 *      Author: owen
 */

#ifndef MODE_MONO_GLIDER_H_
#define MODE_MONO_GLIDER_H_

#include "arm_math.h"

void mode_mono_glider_init(void);
float32_t mode_mono_glider_sample_process (void) ;
void mode_mono_glider_control_process (void);

#endif /* MODE_MONO_GLIDER_H_ */
