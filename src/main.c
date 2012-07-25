/*
===============================================================================
 Name        : main.c
 Author      : 
 Version     :
 Copyright   : Copyright (C) 
 Description : main definition
===============================================================================
*/



#ifdef __USE_CMSIS
#include "stm32f4xx.h"
#endif

/* Includes ------------------------------------------------------------------*/

#include "arm_math.h"
#include "CS4344.h"
#include "pp6.h"
#include "miditof.h"
#include "uart.h"
#include "midi.h"

#include "mode_filter_man.h"
#include "mode_simple_sin.h"
#include "mode_mono_glider.h"
#include "mode_filter_envelope.h"
#include "mode_nazareth.h"
#include "mode_drum.h"
#include "mode_drum_synth.h"


extern unsigned int software_index;
extern unsigned int hardware_index;
extern short play_buf[];
extern uint32_t sample_clock;


extern pocket_piano pp6;



void Delay(__IO uint32_t nCount);


static uint32_t count = 0;
static uint32_t all_clock = 0;
static uint32_t machine_gun_period = 100;
static uint32_t aux_last, aux = 0;
static uint32_t last_hit;


// MIDI buffer
volatile uint8_t  uart_recv_buf[32];
volatile uint8_t  uart_recv_buf_write = 0;
uint8_t  uart_recv_buf_read = 0;
uint8_t tmp8;

int main(void)
{

//for(;;);
	Delay(20000);

  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
        system_stm32f4xx.c file
     */

	  // enable random number generator
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	RNG_Cmd(ENABLE);

	pp6_leds_init();
	pp6_knobs_init();
	pp6_keys_init();


	MODE_LED_RED_OFF;
	MODE_LED_BLUE_OFF;
	MODE_LED_GREEN_OFF;

	// init codec
	CS4344_init();

	float32_t sig, f;
	q15_t  wave;



	f = 50.0;
	uint32_t k, k_last, kdown, note, note_last, i;
	uint8_t ch;
	k = note = note_last = 0;

	pp6_set_mode(4);
	pp6_set_aux(0);

	mode_filter_man_init();
	mode_simple_sin_init();
	mode_filter_envelope_init();

	BANK_LED_RED_ON;

	uart_init();



	// echo
/*	for (;;){

		while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET); // Wait for Character

		ch = USART_ReceiveData(USART1);
		BANK_LED_RED_ON;
		while (USART_GetFlagStatus (USART1, USART_FLAG_TXE) == RESET);
		USART_SendData (USART1, ch);
		BANK_LED_RED_OFF;

	}*/

	midi_init();

	while (1)	{

	    /* Update WWDG counter */
	    //WWDG_SetCounter(127);

		// check for new midi
	    // buffer midi reception
	    if (!(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)){
	    	uart_recv_buf[uart_recv_buf_write] = USART_ReceiveData(USART1);
	        uart_recv_buf_write++;
	        uart_recv_buf_write &= 0x1f;  // 32 bytes
	    }

        // process MIDI
        if (uart_recv_buf_read != uart_recv_buf_write){
            tmp8 = uart_recv_buf[uart_recv_buf_read];
            uart_recv_buf_read++;
            uart_recv_buf_read &= 0x1f;
            recvByte(tmp8);
        }

		/*
		 * Control Rate
		 */
		if (!(sample_clock & 0x3F)){
			sample_clock++;
			pp6_keys_update();
			pp6_knobs_update();

			k = pp6_get_keys();

			// 16 keys
			pp6.num_keys_down = 0;
			for (i = 0; i < 16; i++) {
				if ( !((k>>i) & 1) ) {
					pp6.num_keys_down++;
				}
				if ( (!((k>>i) & 1)) &&  (((k_last>>i) & 1))  )  {  // new key down
					pp6_set_note(i);
					//pp6_set_note_start();
				}
				if ( ((k>>i) & 1) &&  (!((k_last>>i) & 1))  )  {  // key up
					// release it if playing
					if (i == pp6_get_note()){
						pp6_set_note_stop();
					}
				}
			}
			// mode button
			if ( (!((k>>17) & 1)) &&  (((k_last>>17) & 1)) ){
				pp6_change_mode();
			}
			// aux button
			if ( (!((k>>16) & 1)) &&  (((k_last>>16) & 1)) ){
				pp6_change_aux();
			}
			k_last = k;


			// DO MACHINE GUN
			count++;
			all_clock++;
			aux = pp6_get_aux();
			if (aux != aux_last) {
				aux_last = aux;
				machine_gun_period = (all_clock - last_hit) / 4;
				last_hit = all_clock;

			}
			if ((count > machine_gun_period) && pp6_get_num_keys_down() ) {
				//pp6_set_note_start();
				count = 0;
			}
			// end do machine gun

			if (pp6_get_mode() == 0)  mode_simple_sin_control_process();
			if (pp6_get_mode() == 1)  mode_filter_man_control_process();
			if (pp6_get_mode() == 2)  mode_mono_glider_control_process ();
			if (pp6_get_mode() == 3)  mode_filter_envelope_control_process();
			if (pp6_get_mode() == 4)  mode_nazareth_control_process();
			if (pp6_get_mode() == 5)  mode_drum_synth_control_process();
		}

		/*
		 * Sample Rate
		 */
		if (software_index != hardware_index){
			if (software_index & 1){   // channel


				if (pp6_get_mode() == 0) sig = mode_simple_sin_sample_process();
				if (pp6_get_mode() == 1) sig = mode_filter_man_sample_process();
				if (pp6_get_mode() == 2) sig = mode_mono_glider_sample_process();
				if (pp6_get_mode() == 3) sig = mode_filter_envelope_sample_process();
				if (pp6_get_mode() == 4) sig = mode_nazareth_sample_process();
				if (pp6_get_mode() == 5) sig = mode_drum_synth_sample_process();

				arm_float_to_q15(&sig, &wave, 1);

				// put the r
				play_buf[software_index] = wave;
				software_index++;
				software_index &= 0xf;
			}
			else {   // channel 2, do the same thing
				// put the l
				play_buf[software_index] = wave;
				software_index++;
				software_index &= 0xf;
	    	}
	    }
	}
}

/*
filter_man_control_process(){


}*/



/**
  * @brief  Delay Function.
  * @param  nCount:specifies the Delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
