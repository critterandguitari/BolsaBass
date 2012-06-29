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

void pp6_change_mode(void){
	GPIO_ToggleBits(GPIOE, GPIO_Pin_0);
	pp6.mode ++;
	pp6.mode &= 1;
}
uint32_t pp6_get_mode(void){
	return pp6.mode;
}

uint32_t pp6_get_keys(void) {
		return pp6.keys;
}

void pp6_set_note(uint8_t note) {
	pp6.note = note;
}

void pp6_set_note_start (void ) {
	pp6.note_end = 0;
	pp6.playing = 1;
	pp6.note_start = 1;
}
uint8_t pp6_get_note_start(void) {
	uint8_t tmp;
	tmp = pp6.note_start;
	pp6.note_start = 0;
	return tmp;
}

uint8_t pp6_get_note(void) {
	return pp6.note;
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
	pp6.knob_1 = knobs[0] / 65536.f;
	pp6.knob_2 = knobs[1] / 65536.f;
	pp6.knob_3 = knobs[2] / 65536.f;

	channel++;
	if (channel > 2) channel = 0;

	if (channel == 0) ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 1, ADC_SampleTime_15Cycles);
	if (channel == 1) ADC_RegularChannelConfig(ADC3, ADC_Channel_11, 1, ADC_SampleTime_15Cycles);
	if (channel == 2) ADC_RegularChannelConfig(ADC3, ADC_Channel_10, 1, ADC_SampleTime_15Cycles);

	ADC_SoftwareStartConv(ADC3);
}

void pp6_leds_init(void) {
	GPIO_InitTypeDef  GPIO_InitStructure;
	/* GPIOE Periph clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);


	/* Configure PE0, PE1 in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	//GPIO_ToggleBits(GPIOE, GPIO_Pin_1);
	//GPIO_ToggleBits(GPIOE, GPIO_Pin_0);

	// Bank LED PB4, 5, 8
	/*RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);*/
}

void pp6_keys_init(void) {

	GPIO_InitTypeDef  GPIO_InitStructure;

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
