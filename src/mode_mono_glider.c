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
#include "mode_mono_glider.h"

extern float miditof[];


float32_t sig, f, amp;
bl_saw sq1, sq2;
sadsr amp_env;

void mode_mono_glider_init(void){
	//bl_saw_init(&sq1);
	//bl_saw_init(&sq2);
	bl_saw_set(&sq1, 440);
	bl_saw_set(&sq2, 460);
	sadsr_init(&amp_env);
}

float32_t mode_mono_glider_sample_process (void) {
	bl_saw_set(&sq1, f *  ((pp6_get_knob_3() * 4.f) + 1));
	bl_saw_set(&sq2, f *  ((pp6_get_knob_3() * 4.f) + 1) +   (pp6_get_knob_1() * 10.f) );


	sig = (bl_saw_process(&sq1) * .2f) + (bl_saw_process(&sq2) * .2f);
	amp = sadsr_process(&amp_env);
	sig *= amp * amp;
	return sig;
}

void mode_mono_glider_control_process (void) {

	f = miditof[pp6_get_note()] * .6f;
	if (pp6_get_note_start() ){
		sadsr_set(&amp_env, .01f, 1.f, .2f, .6f);
		sadsr_go(&amp_env);
		//sin_reset(&sin1);
	}
	if (pp6_get_note_stop()){
		sadsr_release(&amp_env);
	}


}
