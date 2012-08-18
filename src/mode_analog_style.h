/*
 * mode_analog_style.h
 *
 *  Created on: Aug 18, 2012
 *      Author: owen
 */

#ifndef MODE_ANALOG_STYLE_H_
#define MODE_ANALOG_STYLE_H_

#include "arm_math.h"


void mode_analog_style_init(void);

float32_t mode_analog_style_sample_process (void) ;

void mode_analog_style_control_process (void);

#endif /* MODE_ANALOG_STYLE_H_ */
