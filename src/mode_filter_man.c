/*
 * mode_filter_man.c
 *
 *  Created on: Jun 23, 2012
 *      Author: owen
 */

#include "stm32f4xx.h"
#include "arm_math.h"

#include "oscillator.h"
#include "pp6.h"
#include "line.h"
#include "vcf.h"
#include "sadsr.h"
#include "mode_filter_man.h"

extern float miditof[];


static float32_t sig, f, f2, f2_ratio, filter_cutoff;
static bl_saw saw, saw2;
static vcf_filter filter;
static float32_t amp = 0.f;
static sadsr amp_env;
static uint32_t timer;
static uint32_t octave_shift;
static line framp;

void mode_filter_man_init(void){

	f = 0;
	vcf_filter_init(&filter);
	sadsr_init(&amp_env);

}


float32_t mode_filter_man_sample_process (void) {



	//sig = bl_saw_process(&saw) * .75;

	f = c_to_f(line_process(&framp)) * (pp6_get_knob_3() +  1.f);
	f2_ratio = 1.005f;//(pp6_get_knob_2() + 1);
	f2 = f * f2_ratio;

	bl_saw_set(&saw, f);
	bl_saw_set(&saw2, f2);

	sig = (bl_saw_process(&saw) * .4) + (bl_saw_process(&saw2) * .4);

	//amp -= .00001f;
	//if (amp < 0) amp = 0.f;

	amp = sadsr_process(&amp_env);

	sig *= amp * amp;

		//if (mode)
	sig = vcf_filter_process(&filter, sig);

	return sig;
}

void mode_filter_man_control_process (void) {

	static float32_t target_f;
	static uint32_t note_dur;


	if (pp6_get_note_start()){
		note_dur=0;
		target_f = (float32_t)pp6_get_note() * 100.f;
		line_go(&framp, target_f, (pp6_get_knob_2() + .001f) * 5000.f);


		// only if there is one note down (staccato)
		if (pp6_get_num_keys_down() == 1){
			sadsr_set(&amp_env, .01f, 1.f, 1.f, .6f);
			sadsr_go(&amp_env);
			bl_saw_reset(&saw);
			bl_saw_reset(&saw2);
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
	vcf_filter_set(&filter, (pp6_get_knob_1() * 10000.f) + f * 2,  (1.f - pp6_get_knob_1()) * 3.f);



	timer++;
	if (timer > (pp6_get_knob_2() * 500)){
		filter_cutoff = ((float32_t)RNG_GetRandomNumber()  / 2147483648.0f) * 600.f;
		timer = 0;
		octave_shift++;
		octave_shift &= 1;

	}

}
