/*
 * mode_filter_man.h
 *
 *  Created on: Jun 23, 2012
 *      Author: owen
 */

#ifndef MODE_FILTER_MAN_H_
#define MODE_FILTER_MAN_H_

#include "arm_math.h"


void mode_filter_man_init(void);

float32_t mode_filter_man_sample_process (void) ;

void mode_filter_man_control_process (void);

#endif /* MODE_FILTER_MAN_H_ */
