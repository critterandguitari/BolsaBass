/*
 * pp6.c
 *
 *  Created on: Jun 23, 2012
 *      Author: owen
 */


#ifdef __USE_CMSIS
#include "stm32f4xx.h"
#endif
#include "pp6.h"


#define ABS(a)	   (((a) < 0) ? -(a) : (a))

pocket_piano pp6;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* You can monitor the converted value by adding the variable "ADC3ConvertedValue"
   to the debugger watch window */


// keypad values
uint32_t keys = 0xFFFFFFFF;

uint32_t keys_history[] = {
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
};

void pp6_init(void) {
	pp6.knob_touched[0] = 0;
	pp6.knob_touched[1] = 0;
	pp6.knob_touched[2] = 0;
	pp6.physical_notes_on = 0;
}

float32_t pp6_get_knob_1(void){
	return pp6.knob[0];
}
float32_t pp6_get_knob_2(void){
	return pp6.knob[1];
}
float32_t pp6_get_knob_3(void){
	return pp6.knob[2];
}

float32_t * pp6_get_knob_array(void){
	return pp6.knob;
}

void pp6_set_knob_array(float32_t * knobs){
	pp6.knob[0] = knobs[0];
	pp6.knob[1] = knobs[1];
	pp6.knob[2] = knobs[2];
}

// mode and aux buttons
void  pp6_set_mode_button_pressed(void){
	pp6.mode_button_pressed = 1;
}
void pp6_set_mode_button_released(void){
	pp6.mode_button_released = 1;
}
uint8_t pp6_mode_button_pressed(void){
	return pp6.mode_button_pressed;
}
uint8_t pp6_mode_button_released(void){
	return pp6.mode_button_released;
}

void  pp6_set_aux_button_pressed(void){
	pp6.aux_button_pressed = 1;
}
void pp6_set_aux_button_released(void){
	pp6.aux_button_released = 1;
}
uint8_t pp6_aux_button_pressed(void){
	return pp6.aux_button_pressed;
}
uint8_t pp6_aux_button_released(void){
	return pp6.aux_button_released;
}

// MODE
void pp6_change_mode(void){
	pp6.mode++;
	if (pp6.mode == 6) pp6.mode = 0;
	pp6_set_mode_led(pp6.mode + 1);
}

void pp6_set_mode(uint32_t mode){
	pp6.mode = mode;
	pp6_set_mode_led(pp6.mode + 1);
}

uint32_t pp6_get_mode(void){
	return pp6.mode;
}

// MODE LED
void pp6_set_mode_led(uint8_t led) {
	pp6.mode_led = led;
	if (led == 0) {MODE_LED_RED_OFF;MODE_LED_GREEN_OFF;MODE_LED_BLUE_OFF;}
	if (led == 1) {MODE_LED_RED_ON;MODE_LED_GREEN_OFF;MODE_LED_BLUE_OFF;}
	if (led == 2) {MODE_LED_RED_ON;MODE_LED_GREEN_ON;MODE_LED_BLUE_OFF;}
	if (led == 3) {MODE_LED_RED_OFF;MODE_LED_GREEN_ON;MODE_LED_BLUE_OFF;}
	if (led == 4) {MODE_LED_RED_OFF;MODE_LED_GREEN_ON;MODE_LED_BLUE_ON;}
	if (led == 5) {MODE_LED_RED_OFF;MODE_LED_GREEN_OFF;MODE_LED_BLUE_ON;}
	if (led == 6) {MODE_LED_RED_ON;MODE_LED_GREEN_OFF;MODE_LED_BLUE_ON;}
}

uint8_t pp6_get_mode_led(void){
	return pp6.mode_led;
}


// AUX LED
void pp6_set_aux_led(uint8_t led) {
	pp6.aux_led = led;
	if (led == 0) {AUX_LED_RED_OFF;AUX_LED_GREEN_OFF;AUX_LED_BLUE_OFF;}
	if (led == 1) {AUX_LED_RED_ON;AUX_LED_GREEN_OFF;AUX_LED_BLUE_OFF;}
	if (led == 2) {AUX_LED_RED_ON;AUX_LED_GREEN_ON;AUX_LED_BLUE_OFF;}
	if (led == 3) {AUX_LED_RED_OFF;AUX_LED_GREEN_ON;AUX_LED_BLUE_OFF;}
	if (led == 4) {AUX_LED_RED_OFF;AUX_LED_GREEN_ON;AUX_LED_BLUE_ON;}
	if (led == 5) {AUX_LED_RED_OFF;AUX_LED_GREEN_OFF;AUX_LED_BLUE_ON;}
	if (led == 6) {AUX_LED_RED_ON;AUX_LED_GREEN_OFF;AUX_LED_BLUE_ON;}
	if (led == 7) {AUX_LED_RED_ON;AUX_LED_GREEN_ON;AUX_LED_BLUE_ON;}

}

uint8_t pp6_get_aux_led(void) {
	return pp6.aux_led;
}

uint32_t pp6_get_keys(void) {
		return pp6.keys;
}


// the key being held down
void pp6_set_note(uint8_t note) {
	pp6.note = note;
}
uint8_t pp6_get_note(void) {
	return pp6.note;
}


// Events
void pp6_set_note_start (void ) {
	pp6.note_stop = 0;
	pp6.playing = 1;
	pp6.note_start = 1;
}
uint8_t pp6_get_note_start(void) {
	return pp6.note_start;
}
void pp6_set_note_stop(void){
	pp6.note_start = 0;
	pp6.note_stop = 1;
}
uint8_t pp6_get_note_stop(void){
	return pp6.note_stop;
}
void pp6_clear_flags(void){
	pp6.note_stop = 0;
	pp6.note_start = 0;
	pp6.aux_button_pressed = 0;
	pp6.aux_button_released = 0;
	pp6.mode_button_pressed = 0;
	pp6.mode_button_released = 0;
	pp6.knob_touched[0] = 0;
	pp6.knob_touched[1] = 0;
	pp6.knob_touched[2] = 0;
}

uint8_t pp6_get_num_keys_down(void){
	return pp6.num_keys_down;
}

void pp6_inc_physical_notes_on(void){
	pp6.physical_notes_on++;
}
void pp6_dec_physical_notes_on(void){
	if (pp6.physical_notes_on) pp6.physical_notes_on--;   // check it is positive in case it turns on while a key is pressed or during midi note
}

uint8_t pp6_get_physical_notes_on(void){
	return pp6.physical_notes_on;
}

/**
  * @brief  ADC3 channel12 with DMA configuration
  * @param  None
  * @retval None
  */
void pp6_knobs_init(void)
{
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	GPIO_InitTypeDef      GPIO_InitStructure;

	/* Enable ADC3 GPIO clocks ****************************************/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

	/* Configure ADC3 Channel12, 11, 10 pin as analog input ******************************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_1 | GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* ADC Common Init **********************************************************/
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	/* ADC3 Init ****************************************************************/
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC3, &ADC_InitStructure);

	/* ADC3 regular configuration *************************************/
	ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 1, ADC_SampleTime_15Cycles);

	/* Enable ADC3 */
	ADC_Cmd(ADC3, ENABLE);
	ADC_SoftwareStartConv(ADC3);
}

void pp6_knobs_update(void) {
	static uint8_t channel = 0;
	static uint32_t knobs[3];

	knobs[channel] = ADC_GetConversionValue(ADC3);

	/// ahhh so it was 65536 ,, then it started needing 4096 (the expected value) ??
	pp6.knob[0] = knobs[0] / 65536.f;
	pp6.knob[1] = knobs[1] / 65536.f;
	pp6.knob[2] = knobs[2] / 65536.f;

	/*pp6.knob[0] = knobs[0] / 4096.f;
	pp6.knob[1] = knobs[1] / 4096.f;
	pp6.knob[2] = knobs[2] / 4096.f;*/


	channel++;
	if (channel > 2) channel = 0;

	if (channel == 0) ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 1, ADC_SampleTime_15Cycles);
	if (channel == 1) ADC_RegularChannelConfig(ADC3, ADC_Channel_11, 1, ADC_SampleTime_15Cycles);
	if (channel == 2) ADC_RegularChannelConfig(ADC3, ADC_Channel_10, 1, ADC_SampleTime_15Cycles);

	ADC_SoftwareStartConv(ADC3);
}


// this should be called every half second or so
void pp6_check_knobs_touched (void) {
	static float32_t knobs_last[3] = {0, 0, 0};

	if (ABS(pp6.knob[0] - knobs_last[0]) > .1f) {
		pp6.knob_touched[0] = 1;
		knobs_last[0] = pp6.knob[0];
	}

	if (ABS(pp6.knob[1] - knobs_last[1]) > .1f) {
		pp6.knob_touched[1] = 1;
		knobs_last[1] = pp6.knob[1];
	}

	if (ABS(pp6.knob[2] - knobs_last[2]) > .1f) {
		pp6.knob_touched[2] = 1;
		knobs_last[2] = pp6.knob[2];
	}
}

uint8_t pp6_any_knobs_touched(void) {
	if (pp6.knob_touched[0] || pp6.knob_touched[1] || pp6.knob_touched[2])
		return 1;
	else
		return 0;
}

void pp6_smooth_knobs(void){


	static float32_t knob1 = 0;
	static float32_t knob2 = 0;
	static float32_t knob3 = 0;
	float32_t kFilteringFactor = .05f; // the smoothing factor 1 is no smoothing

	knob1 = (pp6.knob[0] * kFilteringFactor) + (knob1 * (1.0 - kFilteringFactor));
	pp6.knob[0] = knob1;

	knob2 = (pp6.knob[1] * kFilteringFactor) + (knob2 * (1.0 - kFilteringFactor));
	pp6.knob[1] = knob2;

	knob3 = (pp6.knob[2] * kFilteringFactor) + (knob3 * (1.0 - kFilteringFactor));
	pp6.knob[2] = knob3;
}





void pp6_leds_init(void) {
	GPIO_InitTypeDef  GPIO_InitStructure;
	/* GPIOE Periph clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);



	/* Configure PE0, PE1 in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_WriteBit(GPIOB, GPIO_Pin_4, 1);
	GPIO_WriteBit(GPIOB, GPIO_Pin_5, 1);
	GPIO_WriteBit(GPIOB, GPIO_Pin_8, 1);

}



void pp6_keys_init(void) {

	GPIO_InitTypeDef  GPIO_InitStructure;

	pp6.keys = 0xFFFFFFFF;

	/* Periph clocks enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);


	  /* Configure Buttons pin as input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_15 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_8;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_15 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_0 | GPIO_Pin_14 | GPIO_Pin_1;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

}


void pp6_keys_update(void){

	static uint8_t history_index;

	keys_history[(history_index++) & 0x7] =
			GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) |
			(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) << 1) |
			(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) << 2) |
			(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) << 3) |
			(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8) << 4) |
			(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_9) << 5) |
			(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_15) << 6) |
			(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_10) << 7) |
			(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) << 8) |
			(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11) << 9) |
			(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) << 10) |
			(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_12) << 11) |
			(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_13) << 12) |
			(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_0) << 13) |
			(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_14) << 14) |
			(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_1) << 15) |
			(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) << 16) |
			(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2) << 17);

	// debounce keys
	if ((keys_history[0] == keys_history[1]) &&
		(keys_history[0] == keys_history[2]) &&
		(keys_history[0] == keys_history[3]) &&
		(keys_history[0] == keys_history[4]) &&
		(keys_history[0] == keys_history[5]) &&
		(keys_history[0] == keys_history[6]) &&
		(keys_history[0] == keys_history[7]))
	{
		pp6.keys = keys_history[0];
	}
}
