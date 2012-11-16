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


#include "mode_simple_sin.h"
#include "mode_wave_adder.h"
#include "mode_analog_style.h"
#include "mode_filter_envelope.h"
#include "mode_simple_fm.h"
#include "mode_bass_delay.h"

// Secret mode
#include "mode_plurden.h"


// from the DAC driver CS4344.c
extern unsigned int software_index;
extern unsigned int hardware_index;
extern short play_buf[];
extern uint32_t sample_clock;

// for keeping track of time
static uint32_t sample_clock_last = 0;

static void Delay(__IO uint32_t nCount);
static void flash_led_record_enable(void);

// led stuff
static uint32_t led_counter = 0;  // for the above flash function
static uint8_t aux_led_color = BLACK;

// MIDI buffer
uint8_t  uart_recv_buf[32];
uint8_t  uart_recv_buf_write = 0;
uint8_t  uart_recv_buf_read = 0;
uint8_t tmp8;

extern pocket_piano pp6;


// for the eq
EQSTATE eq;

int main(void)
{
	uint8_t i;


	Delay(20000);
	 // enable random number generator
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	RNG_Cmd(ENABLE);

	// initialize low level stuff
	uart_init();
	timer_init();

	// initialize piano
	pp6_init();

	// initialize modes
	mode_simple_sin_init();
	mode_wave_adder_init();
	mode_analog_style_init();
	mode_filter_envelope_init();
	mode_simple_fm_init();
	mode_bass_delay_init();

	// secret mode
	mode_plurden_init();

	// setup codec
	CS4344_init();

	float32_t sig, f;
	q15_t  wave;

	f = 50.0;

	// echo
	/*uint8_t ch;
	while (1){

		while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET); // Wait for Character

		ch = USART_ReceiveData(USART1);
		AUX_LED_RED_ON;
		while (USART_GetFlagStatus (USART1, USART_FLAG_TXE) == RESET);
		USART_SendData (USART1, ch);
		AUX_LED_RED_OFF;

	}*/

	/*while (1){
		t = TIM_GetCounter(TIM2);
		TIM_SetCounter(TIM1,0);
		t_spent = t - t_last;
		t_last = t;
	}*/

	uint32_t aux_button_depress_time = 0;
	uint32_t count = 0;

	// the bass boost is hardcoded in eq.c
	init_3band_state(&eq, 880, 5000, SR);


	// get initial midi channel
	// update keys has to be called > 8 times for key press to be debounced
	for (i = 0; i < 9; i++)
		pp6_keys_update();

	for (i = 0; i < 16; i++) {
		if (!((pp6_get_keys() >> i) & 1))
			break;
	}

	// check if secret mode is enabled
	if (!((pp6_get_keys() >> 17) & 1)) {
		pp6_enable_secret_mode();
		pp6_flash_aux_led(200);
		pp6_flash_mode_led(200);
	}



	// no key was pressed, defalut to ch 1
	if (i == 16) i = 0;

	// initialize midi library
	midi_init(i + 1);

	// go!
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

        // process MIDI, pp6 will be updated from midi.c handlers if there are any relevant midi events
        if (uart_recv_buf_read != uart_recv_buf_write){
            tmp8 = uart_recv_buf[uart_recv_buf_read];
            uart_recv_buf_read++;
            uart_recv_buf_read &= 0x1f;
            recvByte(tmp8);
        }

        // empty the tx buffer
        uart_service_tx_buf();

        pp6_check_for_midi_clock();

        if (pp6_midi_clock_present()){
        	if (pp6_get_midi_clock_tick()){
        		seq_tick();
        		pp6_clear_midi_clock_tick();
        	}
        }

		/*
		 * Control Rate
		 */
		if ((!(sample_clock & 0x3F)) && (sample_clock != sample_clock_last)){
			sample_clock_last = sample_clock;
			pp6_keys_update();
			pp6_knobs_update();

			pp6_flash_update();

			// every 128 times, check if knobs got moved
			count++;
			count &= 0x7f;
			if (!count) {
				pp6_check_knobs_touched();   // this sets the knobs touched flags
			}

			pp6_get_key_events();

			if (pp6_mode_button_pressed()){
				pp6_change_mode();
			}

			// SEQUENCER
			if (!pp6_midi_clock_present()){
				seq_tick();
			}

			if (seq_get_status() == SEQ_STOPPED){

				pp6_set_aux_led(BLACK);

				// aux button gets pressed and held
				if ( (!(( pp6_get_keys() >> 16) & 1)) ) {
					aux_button_depress_time++;
					if (aux_button_depress_time > 500){
						aux_button_depress_time = 0;
						seq_set_status(SEQ_RECORD_ENABLE);
					}
				}

				if (pp6_aux_button_pressed() || pp6_get_midi_start()) {
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
					seq_set_status(SEQ_STOPPED);
				}
				if (pp6_note_on_flag()){
					seq_set_status(SEQ_RECORDING);
					seq_start_recording();
					seq_log_events();
					seq_log_knobs(pp6_get_knob_array());
					//seq_log_first_note(pp6_get_note_on());
				}
				if (pp6_get_midi_start()) {
					seq_set_status(SEQ_RECORDING);
					seq_start_recording();
					seq_log_first_note_null();   // sequence doesn't start with a note
				}
			}
			else if (seq_get_status() == SEQ_RECORDING){

				pp6_set_aux_led(RED);


				seq_log_events();

				// stop recording
				if (pp6_aux_button_pressed() || seq_get_auto_stop()) {
					seq_stop_recording();
					seq_set_status(SEQ_PLAYING);
					seq_enable_knob_playback();
					aux_button_depress_time = 0;
					seq_clear_auto_stop();
				}
				if (pp6_get_midi_stop()) {   // if a midi stop is received, stop recording, and dont play
					seq_stop_recording();
					seq_set_status(SEQ_STOPPED);
					aux_button_depress_time = 0;
					seq_clear_auto_stop();
				}

			}
			else if (seq_get_status() == SEQ_PLAYING) {

				seq_play_knobs();  	// play knobs
				seq_play_tick();  	// play notes
				aux_led_color = GREEN;

				pp6_set_aux_led(aux_led_color);

				// flash white on rollover
				if (seq_get_time() == 0) pp6_flash_aux_led(75);

				// aux button gets pressed and held
				if ( (!(( pp6_get_keys() >>16) & 1)) ) {
					aux_button_depress_time++;
					if (aux_button_depress_time > 500){
						aux_button_depress_time = 0;
						seq_set_status(SEQ_RECORD_ENABLE);
						pp6_set_synth_note_stop();  // stop the synth
						pp6_turn_off_all_on_notes();
					}
				}
				// aux button swithes to stop
				if (pp6_aux_button_pressed() || pp6_get_midi_stop()) {
					seq_set_status(SEQ_STOPPED);
					aux_button_depress_time = 0;
					pp6_set_synth_note_stop();   // stop the synth
					pp6_turn_off_all_on_notes();
				}
			}

			// END SEQUENCER

			// check for events
			// TODO:  limit calls to sendNoteOn and Off to 8 so buffer isn't overrun  (or have it check room in buffer)
			for (i = 0; i < 128; i++) {
				if (pp6_get_note_state(i) != pp6_get_note_state_last(i)) {
					if (pp6_get_note_state(i)) {
						sendNoteOn(1, i, 100);
						pp6_set_synth_note_start();
						pp6_set_synth_note(i);
					}
					else {
						sendNoteOff(1, i, 0);
						if (i ==  pp6_get_synth_note()) pp6_set_synth_note_stop();  // if it equals the currently playing note, shut it off
					}
				}
			}
			pp6_set_current_note_state_to_last();

			// smooth the knobs here in case they are playing back
			pp6_smooth_knobs();

			//t1 =  timer_get_time();

			if (pp6_get_mode() == 0)  mode_simple_sin_control_process();
			if (pp6_get_mode() == 1)  mode_wave_adder_control_process ();
			if (pp6_get_mode() == 2)  mode_analog_style_control_process();
			if (pp6_get_mode() == 3)  mode_filter_envelope_control_process();
			if (pp6_get_mode() == 4)  mode_simple_fm_control_process();
			if (pp6_get_mode() == 5)  mode_bass_delay_control_process();
			if (pp6_get_mode() == 6)  mode_plurden_control_process();   // SECRET MODE ??

			//t2 = timer_get_time();
			//t = t2 - t1;

			// clear all the events
			pp6_clear_flags();
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
				if (pp6_get_mode() == 5) sig = mode_bass_delay_sample_process();
				if (pp6_get_mode() == 6) sig = mode_plurden_sample_process();

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
