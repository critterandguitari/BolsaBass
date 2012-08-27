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
#include "sequencer.h"
#include "eq.h"
#include "audio.h"

#include "mode_filter_man.h"
#include "mode_simple_sin.h"
#include "mode_mono_glider.h"
#include "mode_filter_envelope.h"
#include "mode_nazareth.h"
#include "mode_drum.h"
#include "mode_drum_synth.h"
#include "mode_simple_fm.h"
#include "mode_octave_mirror.h"
#include "mode_wave_adder.h"
#include "mode_analog_style.h"



extern unsigned int software_index;
extern unsigned int hardware_index;
extern short play_buf[];
extern uint32_t sample_clock;


extern pocket_piano pp6;



void Delay(__IO uint32_t nCount);
static void flash_led_record_enable(void);



// led stuff
uint32_t led_counter = 0;  // for the above flash function
uint8_t aux_led_color = BLACK;

// MIDI buffer
volatile uint8_t  uart_recv_buf[32];
volatile uint8_t  uart_recv_buf_write = 0;
uint8_t  uart_recv_buf_read = 0;
uint8_t tmp8;

// for the eq
EQSTATE eq;

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

	pp6_set_aux_led(BLACK);

	// init codec
	CS4344_init();

	float32_t sig, f;
	q15_t  wave;

	f = 50.0;
	uint32_t k, k_last, note, note_last, i;
	k = note = note_last = 0xFFFFFFFF;

	pp6_set_mode(0);

	mode_filter_man_init();
	mode_simple_sin_init();
	mode_filter_envelope_init();
	mode_simple_fm_init();
	mode_octave_mirror_init();
	mode_wave_adder_init();
	mode_analog_style_init();



	uart_init();
	midi_init();
	timer_init();

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

	uint32_t aux_button_depress_time = 0;
	uint32_t t, t1, t2;
	uint32_t count = 0;

	// the bass boost is hardcoded in eq.c
	init_3band_state(&eq, 880, 5000, SR);

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

			// every 128 times, check if knobs got moved
			count++;
			count &= 0x7f;
			if (!count) {
				pp6_check_knobs_touched();   // this sets the knobs touched flags
			}

			// scan keys 16 keys
			k = pp6_get_keys();
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
			if ( (!((k>>17) & 1)) &&  (((k_last>>17) & 1)) ){
				pp6_set_mode_button_pressed();
			}
			if ( (!((k>>16) & 1)) &&  (((k_last>>16) & 1)) ){
				pp6_set_aux_button_pressed();
			}
			if ( (((k>>16) & 1)) &&  (!((k_last>>16) & 1)) ){
				pp6_set_aux_button_released();
			}


			if (pp6_mode_button_pressed()){
				pp6_change_mode();
			}

			// SEQUENCER
			seq_tick();

			if (seq_get_status() == SEQ_STOPPED){

				pp6_set_aux_led(BLACK);

				// aux button gets pressed and held
				if ( (!((k>>16) & 1)) ) {
					aux_button_depress_time++;
					if (aux_button_depress_time > 500){
						aux_button_depress_time = 0;
						seq_set_status(SEQ_RECORD_ENABLE);
					}
				}

				if (pp6_aux_button_pressed()) {
					if (seq_get_length()) {  // only play if positive length
						seq_enable_knob_playback();
						seq_set_status(SEQ_PLAYING);
					}
					else seq_set_status(SEQ_STOPPED);
					seq_rewind();
					aux_button_depress_time = 0;
				}
			}
			else if (seq_get_status() == SEQ_RECORD_ENABLE){
				flash_led_record_enable();
				if (pp6_aux_button_pressed()) {
				//	if (seq_get_length()) seq_set_status(SEQ_PLAYING);  // only play if positive length
				//	else
						seq_set_status(SEQ_STOPPED);
				}
				if (pp6_get_note_start()){
					seq_set_status(SEQ_RECORDING);
					seq_log_first_note(pp6_get_note());
				}
			}
			else if (seq_get_status() == SEQ_RECORDING){

				pp6_set_aux_led(RED);

				seq_log_knobs(pp6_get_knob_array());

				if (pp6_get_note_start()){
					seq_log_note_start(pp6_get_note());
				}
				if (pp6_get_note_stop()) {
					seq_log_note_stop(pp6_get_note());
				}

				// stop recording
				if (pp6_aux_button_pressed() || seq_get_auto_stop() ) {
					seq_stop_recording();
					seq_set_status(SEQ_PLAYING);
					aux_button_depress_time = 0;
					seq_clear_auto_stop();
				}
			}
			else if (seq_get_status() == SEQ_PLAYING) {
				seq_play_tick();  // run the sequencer

				if (pp6_any_knobs_touched()) {
					seq_disable_knob_playback();
				}

				if (seq_knob_playback_enabled()) {
					pp6_set_knob_array(seq_play_knobs());
					aux_led_color = CYAN;
				} else {
					aux_led_color = GREEN;
				}

				// flash white on rollover
				if (seq_get_time() < 75) pp6_set_aux_led(7);
				else pp6_set_aux_led(aux_led_color);

				// aux button gets pressed and held
				if ( (!((k>>16) & 1)) ) {
					aux_button_depress_time++;
					if (aux_button_depress_time > 500){
						aux_button_depress_time = 0;
						seq_set_status(SEQ_RECORD_ENABLE);
						pp6_set_note_stop();
					}
				}
				// aux button swithes to stop
				if (pp6_aux_button_pressed()) {
					seq_set_status(SEQ_STOPPED);
					aux_button_depress_time = 0;
					pp6_set_note_stop();
				}
			}

			// END SEQUENCER


			// store keys for next time
			k_last = k;

			// smooth the knobs here in case they are playing back
			pp6_smooth_knobs();

			t1 =  timer_get_time();
			if (pp6_get_mode() == 0)  mode_simple_sin_control_process();   // rampi
			if (pp6_get_mode() == 1)   mode_wave_adder_control_process ();   //
			if (pp6_get_mode() == 2)  mode_analog_style_control_process();
			if (pp6_get_mode() == 3)  mode_filter_envelope_control_process();
			if (pp6_get_mode() == 4)  mode_simple_fm_control_process();
			if (pp6_get_mode() == 5)  mode_nazareth_control_process();
			t2 = timer_get_time();
			t = t2 - t1;

			pp6_clear_flags();
			//seq_time++;
			led_counter++;

		}

		/*
		 * Sample Rate
		 */
		if (software_index != hardware_index){
			if (software_index & 1){   // channel

				//pp6_set_aux_led(RED);

				if (pp6_get_mode() == 0) sig = mode_simple_sin_sample_process();
				if (pp6_get_mode() == 1) sig = mode_wave_adder_sample_process();
				if (pp6_get_mode() == 2) sig = mode_analog_style_sample_process();
				if (pp6_get_mode() == 3) sig = mode_filter_envelope_sample_process();
				if (pp6_get_mode() == 4) sig = mode_simple_fm_sample_process();
				if (pp6_get_mode() == 5) sig = 0;//mode_nazareth_sample_process();

				//if ( (!((k>>16) & 1)) )
				// eq it
					sig = do_3band(&eq, sig);

				arm_float_to_q15(&sig, &wave, 1);

				// put the r
				play_buf[software_index] = wave;
				software_index++;
				software_index &= 0xf;

				//pp6_set_aux_led(0);
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


void flash_led_record_enable() {
	if (led_counter > 150){
		led_counter = 0;
		if (pp6_get_aux_led()){
			pp6_set_aux_led(0);
		}
		else {
			pp6_set_aux_led(1);
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
