/*
 * pp6.h
 *
 *  Created on: Jun 23, 2012
 *      Author: owen
 */

#ifndef PP6_H_
#define PP6_H_

#include "arm_math.h"

#define MODE_LED_BLUE_ON GPIO_WriteBit(GPIOB, GPIO_Pin_9, 0)
#define MODE_LED_RED_ON GPIO_WriteBit(GPIOE, GPIO_Pin_0, 0)
#define MODE_LED_GREEN_ON GPIO_WriteBit(GPIOE, GPIO_Pin_1, 0)
#define MODE_LED_BLUE_OFF GPIO_WriteBit(GPIOB, GPIO_Pin_9, 1)
#define MODE_LED_RED_OFF GPIO_WriteBit(GPIOE, GPIO_Pin_0, 1)
#define MODE_LED_GREEN_OFF GPIO_WriteBit(GPIOE, GPIO_Pin_1, 1)

#define AUX_LED_BLUE_ON GPIO_WriteBit(GPIOB, GPIO_Pin_4, 0)
#define AUX_LED_RED_ON GPIO_WriteBit(GPIOB, GPIO_Pin_5, 0)
#define AUX_LED_GREEN_ON GPIO_WriteBit(GPIOB, GPIO_Pin_8, 0)
#define AUX_LED_BLUE_OFF GPIO_WriteBit(GPIOB, GPIO_Pin_4, 1)
#define AUX_LED_RED_OFF GPIO_WriteBit(GPIOB, GPIO_Pin_5, 1)
#define AUX_LED_GREEN_OFF GPIO_WriteBit(GPIOB, GPIO_Pin_8, 1)

#define BLACK 0
#define RED 1
#define YELLOW 2
#define GREEN 3
#define CYAN 4
#define BLUE 5
#define MAGENTA 6


typedef struct {
	float32_t knob_1;
	float32_t knob_2;
	float32_t knob_3;
	uint8_t note_start;
	uint8_t note_stop;
	uint8_t playing;
	uint8_t note;
	uint32_t keys;
	uint32_t mode;
	uint8_t mode_button_pressed;
	uint8_t mode_button_released;
	uint32_t aux;
	uint8_t aux_button_pressed;
	uint8_t aux_button_released;
	uint8_t num_keys_down;
	uint8_t mode_led;
	uint8_t aux_led;
} pocket_piano;

float32_t pp6_get_knob_1(void);
float32_t pp6_get_knob_2(void);
float32_t pp6_get_knob_3(void);

uint8_t pp6_get_note_start(void);
void pp6_set_note_start (void );

uint8_t pp6_get_note_stop(void);
void pp6_set_note_stop (void );
void pp6_clear_flags(void);

uint8_t pp6_is_playing (void);

uint8_t pp6_get_note(void);
void pp6_set_note(uint8_t note);

uint32_t pp6_get_keys(void);
uint8_t pp6_get_num_keys_down(void);

void pp6_change_mode(void);
uint32_t pp6_get_mode(void);
void pp6_set_mode(uint32_t mode);

void pp6_change_aux(void);
uint32_t pp6_get_aux(void);
void pp6_set_aux(uint32_t aux);


void pp6_keys_init(void);
void pp6_keys_update(void);
void pp6_leds_init(void);
void pp6_leds_update(uint8_t bank_led, uint8_t mode_led);
void pp6_knobs_init(void);
void pp6_knobs_update(void);

uint8_t pp6_get_mode_led(void);
uint8_t pp6_get_aux_led(void);
void pp6_set_mode_led(uint8_t mode_led);
void pp6_set_aux_led(uint8_t bank_led);

void  pp6_set_mode_button_pressed(void);
void pp6_set_mode_button_released(void);
uint8_t pp6_mode_button_pressed(void);
uint8_t pp6_mode_button_released(void);

void  pp6_set_aux_button_pressed(void);
void pp6_set_aux_button_released(void);
uint8_t pp6_aux_button_pressed(void);
uint8_t pp6_aux_button_released(void);


#endif /* PP6_H_ */
