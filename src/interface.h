/*
 * interface.h
 *
 *  Created on: Jun 7, 2012
 *      Author: owen
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include "arm_math.h"


typedef struct {
	float32_t knob_1;
	float32_t knob_2;
	float32_t knob_3;
	uint8_t note_start;
	uint8_t note_end;
	uint8_t playing;
	uint8_t note;
	uint32_t keys;
} pocket_piano;



void keys_init(void);
void keys_update(void);
void leds_init(void);
void knobs_init(void);
void knobs_update(void);


#endif /* INTERFACE_H_ */
