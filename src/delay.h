/*
 * delay.h
 *
 *  Created on: Oct 19, 2012
 *      Author: owen
 */

#ifndef DELAY_H_
#define DELAY_H_


#include "arm_math.h"
#include "audio.h"

#define MAX_WG_DELAY 10000


float32_t delay_process(float32_t in);
void delay_init(void);
void delay_set_dtime (float32_t dtime); // in seconds
void delay_set_fb (float32_t fb);

#endif /* DELAY_H_ */
