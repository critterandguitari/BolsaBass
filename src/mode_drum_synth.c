/*
 * mode_drum_synth.c
 *
 *  Created on: Jul 21, 2012
 *      Author: owen
 */


#include "stm32f4xx.h"
#include "arm_math.h"
#include "oscillator.h"
#include "pp6.h"
#include "line.h"
#include "vcf.h"
#include "sadsr.h"
#include "mode_drum_synth.h"

extern float miditof[];


static float32_t sig, f;
static bl_saw saw;
static vcf_filter filter;
static float32_t amp = 0.f;
static sadsr amp_env;

void mode_drum_synth_init(void){

	f = 0;
	vcf_filter_init(&filter);
	sadsr_init(&amp_env);

}


float32_t mode_drum_synth_sample_process (void) {

	//bl_saw_set(&saw, f);
	sig = ((float32_t) RNG_GetRandomNumber() / 2147483648.f) * .6f;// / 2147483648.0f;;

	amp = sadsr_process(&amp_env);

	sig *= amp * amp;

		//if (mode)
	sig = vcf_filter_process(&filter, sig);

	return sig;
}

void mode_drum_synth_control_process (void) {
	f = miditof[pp6_get_note()] * .6f * (pp6_get_knob_3() +  1.f);
	if (pp6_get_note_start()){
		sadsr_set(&amp_env, .01f, 1.f, 10.f, .6f);
		sadsr_go(&amp_env);
	}
	if (pp6_get_note_stop()){
		sadsr_release(&amp_env);
	}
	vcf_filter_set(&filter, (pp6_get_knob_1() * 6000.f) + 100.f, pp6_get_knob_2() * 3.9f );
	//vcf_filter_set(&filter, f * 2.f, pp6_get_knob_2() * 1.9f + 2.f );
}
