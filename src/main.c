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
#include "timer.h"

#include "mode_filter_man.h"
#include "mode_simple_sin.h"
#include "mode_mono_glider.h"
#include "mode_filter_envelope.h"
#include "mode_nazareth.h"
#include "mode_drum.h"
#include "mode_drum_synth.h"
#include "mode_simple_fm.h"

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
	k = note = note_last = 0xFFFFFFFF;

	pp6_set_mode(0);
	pp6_set_aux(0);

	mode_filter_man_init();
	mode_simple_sin_init();
	mode_filter_envelope_init();
	mode_simple_fm_init();


	BANK_LED_RED_ON;

	uart_init();
	midi_init();


	// echo
/*	for (;;){

		while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET); // Wait for Character

		ch = USART_ReceiveData(USART1);
		BANK_LED_RED_ON;
		while (USART_GetFlagStatus (USART1, USART_FLAG_TXE) == RESET);
		USART_SendData (USART1, ch);
		BANK_LED_RED_OFF;

	}*/






	/*while (1){
		t = TIM_GetCounter(TIM2);
		TIM_SetCounter(TIM1,0);
		t_spent = t - t_last;
		t_last = t;
	}*/

	uint32_t t, t1, t2;
	uint32_t sequencer_enable = 0;
	timer_init();

	uint32_t seq_deltas[255];
	uint32_t seq_notes[255];
	uint32_t seq_events[255];
	uint32_t seq_index = 0;
	uint32_t seq_time = 0;
	uint32_t seq_recording = 0;
	uint32_t seq_playing = 0;
	uint32_t seq_length = 0;
	uint32_t seq_last_note_start = 0;
	uint32_t seq_last_note_start_index = 0;
#define SEQ_NOTE_STOP 2   // these should be standard midi
#define SEQ_NOTE_START 1


	while (1)	{




	    /* Update WWDG counter */
	    //WWDG_SetCounter(127);

		// check for new midi
	    // buffer midi reception
	   /* if (!(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)){
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
        }*/

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
					pp6_set_note_start();
				}
				if ( ((k>>i) & 1) &&  (!((k_last>>i) & 1))  )  {  // key up
					// release it if playing
					if (i == pp6_get_note()){
						pp6_set_note_stop();
					}
				}
			}




			// sequencer record mode, // mode and aux button down
			if ( (!((k>>16) & 1)) && (!((k>>17) & 1)) ){
				sequencer_enable = 1;
				seq_playing = 0;
				if (pp6_get_note_start()){   // and a note gets pressed
					MODE_LED_RED_ON;

					// start new recording
					if (!seq_recording){
						seq_deltas[0] = 0;
						seq_notes[0] = pp6_get_note();
						seq_events[0] = SEQ_NOTE_START;
						seq_time = 0;
						seq_length = 0;
						seq_index = 0;
						seq_recording = 1;
						seq_index++;
					}
					else { // continue current recording
						seq_deltas[seq_index] = seq_time;
						seq_notes[seq_index] = pp6_get_note();
						seq_events[seq_index] = SEQ_NOTE_START;
						seq_last_note_start = seq_time;
						seq_last_note_start_index = seq_index;
						seq_index++;
					}
				}
				if (pp6_get_note_stop()){   // note ends
					seq_deltas[seq_index] = seq_time;
					seq_notes[seq_index] = pp6_get_note();
					seq_events[seq_index] = SEQ_NOTE_STOP ;
					seq_index++;
				}

			}
			else {
				sequencer_enable = 0;
				// if a recording just finished
				if (seq_recording) {
					seq_deltas[seq_index] = seq_last_note_start;
					seq_notes[seq_index] = seq_notes[seq_last_note_start_index];
					seq_events[seq_index] = SEQ_NOTE_STOP; // just make it a note stop
					seq_events[seq_last_note_start_index] = SEQ_NOTE_STOP; // make last note a stop too
					seq_length = seq_last_note_start_index;
					seq_index = 0; // go back to the begining
					seq_time = 0;
					seq_recording = 0;
					seq_playing = 1;
				}
			}

			if (seq_playing && seq_length){
				if (seq_time >= seq_deltas[seq_index]){
					if (seq_events[seq_index] == SEQ_NOTE_START){
						pp6_set_note(seq_notes[seq_index]);
						pp6_set_note_start();
					}
					if (seq_events[seq_index] == SEQ_NOTE_STOP){
						pp6_set_note_stop();
					}
					seq_index++;
					if (seq_index > seq_length){
						seq_index = 0;
						seq_time = 0;
					}
				}
			}


			// only if sequencer is disabled
			if (!sequencer_enable) {
				// mode button
				if ( (!((k>>17) & 1)) &&  (((k_last>>17) & 1)) ){
					pp6_change_mode();
				}
				// aux button
				//if ( (!((k>>16) & 1)) &&  (((k_last>>16) & 1)) ){
				//	pp6_change_aux();
				//}
				// aux button on release
				if ( (((k>>16) & 1)) &&  (!((k_last>>16) & 1)) ){
					pp6_change_aux();
				}
			}


			k_last = k;


			// DO MACHINE GUN
		/*	count++;
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
			}*/
			// end do machine gun


			t1 =  timer_get_time();
			if (pp6_get_mode() == 0)  mode_simple_sin_control_process();
			if (pp6_get_mode() == 1)  mode_filter_man_control_process();
			if (pp6_get_mode() == 2)  mode_mono_glider_control_process ();
			if (pp6_get_mode() == 3)  mode_filter_envelope_control_process();
			if (pp6_get_mode() == 4)  mode_nazareth_control_process();
			if (pp6_get_mode() == 5)  mode_simple_fm_control_process();
			t2 = timer_get_time();
			t = t2 - t1;

			pp6_clear_flags();
			seq_time++;

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
				if (pp6_get_mode() == 5) sig = mode_simple_fm_sample_process();

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
