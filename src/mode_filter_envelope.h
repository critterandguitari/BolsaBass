/*
 * mode_filter_envelope.h
 *
 *  Created on: Jul 6, 2012
 *      Author: owen
 */

#ifndef MODE_FILTER_ENVELOPE_H_
#define MODE_FILTER_ENVELOPE_H_


#include "arm_math.h"

void mode_filter_envelope_init(void);

float32_t mode_filter_envelope_sample_process (void);

void mode_filter_envelope_control_process (void) ;

#endif /* MODE_FILTER_ENVELOPE_H_ */
