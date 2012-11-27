/*
 * mode_analog_style.c
 *
 *  Created on: Aug 18, 2012
 *      Author: owen
 */


#include "stm32f4xx.h"
#include "arm_math.h"

#include "oscillator.h"
#include "pp6.h"
#include "line.h"
#include "vcf.h"
#include "sadsr.h"
#include "mode_analog_style.h"

extern float miditof[];


static float32_t sig, f, filter_cutoff;
static bl_saw saw;
static vcf_filter filter;
static float32_t amp = 0.f;
static sadsr amp_env;
static line framp;

void mode_analog_style_init(void){

	f = 0;
	vcf_filter_init(&filter);
	sadsr_init(&amp_env);

}


float32_t mode_analog_style_sample_process (void) {

	//((c_to_f_ratio(cutoff * cutoff_scale)) * f) + f * 2


	f = c_to_f(line_process(&framp)) * (pp6_get_knob_3() +  1.f);
	//vcf_filter_set(&filter, ( c_to_f_ratio(pp6_get_knob_1() * 6000.f) * f) + f * 2,  pp6_get_knob_2() * 3.75f);

	// scale filter cutoff so its locked to frequency, for high notes from MIDI this might be too high, so cap it below the nyquist
	filter_cutoff = ( pp6_get_knob_1() * 60.f * f) + f * 2;
	if (filter_cutoff > (NYQUIST - 1000)) {
		filter_cutoff = (NYQUIST - 1000);  // a little below nyquist
	}
	vcf_filter_set(&filter, filter_cutoff,  pp6_get_knob_2() * 3.7f);


	bl_saw_set(&saw, f);

	sig = (bl_saw_process(&saw) * .5) ;


	amp = sadsr_process(&amp_env);

	sig *= amp * amp;

	sig = vcf_filter_process(&filter, sig);

	return sig;
}

void mode_analog_style_control_process (void) {

	static float32_t target_f;
	static uint32_t note_dur;


	if (pp6_get_synth_note_start()){
		note_dur=0;
		target_f = (float32_t)pp6_get_synth_note() * 100.f;
		line_go(&framp, target_f, 2.f);


		// only if there is one note down (staccato)
		//if (pp6_get_num_keys_down() == 1){   // this breaks the sequencer....
			sadsr_set(&amp_env, .01f, 1.f, 1.f, .6f);
			sadsr_go(&amp_env);
		//}

	}
	if (pp6_get_synth_note_stop()){

		//if (note_dur < 100)
		//	sadsr_set(&amp_env, .01f, .1f, .15, .6f);

		sadsr_set(&amp_env, .01f, .1f, (note_dur * 128) / SR, .6f);
		sadsr_release(&amp_env);

	}
	note_dur++;



	//vcf_filter_set(&filter, (pp6_get_knob_1() * 6000.f) + 100.f,  (1.f - pp6_get_knob_1()) * 3.f);



}
