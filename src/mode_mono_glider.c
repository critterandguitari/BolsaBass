/*
 * mode_mono_glider.c
 *
 *  Created on: Jul 5, 2012
 *      Author: owen
 */
#include "arm_math.h"
#include "oscillator.h"
#include "pp6.h"
#include "line.h"
#include "vcf.h"
#include "sadsr.h"
#include "audio.h"
#include "mode_mono_glider.h"

extern float miditof[];


float32_t sig, f, amp, cents;
bl_saw saw1, saw2, saw3, saw4;
sadsr amp_env;

void mode_mono_glider_init(void){

	sadsr_init(&amp_env);
}

float32_t mode_mono_glider_sample_process (void) {

	f = c_to_f(cents) * (pp6_get_knob_3() + 1);

	bl_saw_set(&saw1, f *  ((pp6_get_knob_3() * 1.f) + 1));
	bl_saw_set(&saw2, f *  ((pp6_get_knob_3() * 1.f) + 1) +   (pp6_get_knob_1() * 2.f) );
	bl_saw_set(&saw3, f *  ((pp6_get_knob_3() * 1.f) + 1) +   (pp6_get_knob_1() * 4.f) );
	bl_saw_set(&saw4, f *  ((pp6_get_knob_3() * 1.f) + 1) +   (pp6_get_knob_1() * 6.f) );



	sig = (bl_saw_process(&saw1) * .1f) + (bl_saw_process(&saw2) * .1f) + (bl_saw_process(&saw3) * .1f) + (bl_saw_process(&saw4) * .1f);
	amp = sadsr_process(&amp_env);
	sig *= amp * amp;
	return sig;
}

void mode_mono_glider_control_process (void) {

	if (pp6_get_synth_note_start() ){
		cents = ((float32_t)pp6_get_synth_note() * 100.f);
		sadsr_set(&amp_env, .01f, 1.f, .2f, .6f);
		sadsr_go(&amp_env);
	}
	if (pp6_get_synth_note_stop()){
		sadsr_release(&amp_env);
	}


}
