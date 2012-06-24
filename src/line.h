/*
 * line.h
 *
 *  Created on: Apr 18, 2012
 *      Author: owen
 */

#ifndef LINE_H_
#define LINE_H_
#include "arm_math.h"
#include "audio.h"

typedef struct {
	float32_t target;
	float32_t value;
	float32_t step_slope;
	uint32_t num_steps;
	uint32_t steps_remain;
} line;

void line_set(line * l, float32_t v);
void line_go(line * l, float32_t v, float32_t t);
float32_t line_process(line * line);
uint32_t line_is_processing(line * line);

//float32_t line_multi(float32_t * pairs, uint32_t num_segments);

#endif /* LINE_H_ */
