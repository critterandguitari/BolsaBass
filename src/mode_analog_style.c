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


static float32_t sig, f, f2, f2_ratio, filter_cutoff;
static bl_saw saw, saw2;
static vcf_filter filter;
static float32_t amp = 0.f;
static sadsr amp_env;
static uint32_t timer;
static uint32_t octave_shift;
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
	vcf_filter_set(&filter, ( pp6_get_knob_1() * 16.f * f) + f * 2,  pp6_get_knob_2() * 3.75f);


	bl_saw_set(&saw, f);

	sig = (bl_saw_process(&saw) * .75) ;


	amp = sadsr_process(&amp_env);

	sig *= amp * amp;

	sig = vcf_filter_process(&filter, sig);

	return sig;
}

void mode_analog_style_control_process (void) {

	static float32_t target_f;
	static uint32_t note_dur;


	if (pp6_get_note_start()){
		note_dur=0;
		target_f = (float32_t)pp6_get_note() * 100.f;
		line_go(&framp, target_f, 20.f);


		// only if there is one note down (staccato)
		if (pp6_get_num_keys_down() == 1){
			sadsr_set(&amp_env, .01f, 1.f, 1.f, .6f);
			sadsr_go(&amp_env);
		}

	}
	if (pp6_get_note_stop()){

		//if (note_dur < 100)
		//	sadsr_set(&amp_env, .01f, .1f, .15, .6f);

		sadsr_set(&amp_env, .01f, .1f, (note_dur * 64) / SR, .6f);
		sadsr_release(&amp_env);

	}
	note_dur++;



	//vcf_filter_set(&filter, (pp6_get_knob_1() * 6000.f) + 100.f,  (1.f - pp6_get_knob_1()) * 3.f);



}
