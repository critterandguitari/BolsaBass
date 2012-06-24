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

float32_t comb_process(float32_t in);
void comb_init(void);


#endif /* COMB_H_ */
