/*
 * pp6.h
 *
 *  Created on: Jun 23, 2012
 *      Author: owen
 */

#ifndef PP6_H_
#define PP6_H_


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
	uint32_t mode;
} pocket_piano;


float32_t get_knob_1(void);
float32_t get_knob_2(void);
float32_t get_knob_3(void);

uint8_t pp6_get_note_start(void);
void pp6_set_note_start (void );

uint8_t pp6_get_note_end(void);
void pp6_set_note_end (void );

uint8_t pp6_is_playing (void);

uint8_t pp6_get_note(void);
void pp6_set_note(uint8_t note);

uint32_t pp6_get_keys(void);

void pp6_change_mode(void);
uint32_t pp6_get_mode(void);

void pp6_keys_init(void);
void pp6_keys_update(void);
void pp6_leds_init(void);
void pp6_leds_update(uint8_t bank_led, uint8_t mode_led);
void pp6_knobs_init(void);
void pp6_knobs_update(void);






uint8_t pp6_get_mode_led(void);
uint8_t pp6_get_bank_led(void);
uint8_t pp6_set_mode_led(uint8_t mode_led);
uint8_t pp6_set_bank_led(uint8_t bank_led);


#endif /* PP6_H_ */
