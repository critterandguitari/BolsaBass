/*
 * comb.h
 *
 *  Created on: Apr 20, 2012
 *      Author: owen
 */

#ifndef COMB_H_
#define COMB_H_

#include "arm_math.h"
#include "audio.h"

#define MAX_DELAY 10000


float32_t comb_process(float32_t in);
void comb_init(void);
void comb_set_dtime (float32_t dtime);
void comb_set_fb (float32_t fb);

#endif /* COMB_H_ */
