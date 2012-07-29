/*
 * mode_filter_envelope.c
 *
 *  Created on: Jul 6, 2012
 *      Author: owen
 */

#include "arm_math.h"
#include "oscillator.h"
#include "pp6.h"
#include "line.h"
#include "vcf.h"
#include "sadsr.h"
#include "mode_filter_envelope.h"

extern float miditof[];
extern float32_t midi_cc[];

static float32_t sig, f, amp, cutoff;
static bl_saw saw, saw2;
static vcf_filter filter;
static sadsr amp_env, filter_env;



void mode_filter_envelope_init(void){

	vcf_filter_init(&filter);
	sadsr_init(&amp_env);
	sadsr_init(&filter_env);
	amp = 0;
	f = 0;
	sig = 0;
	cutoff = 200.f;

}


float32_t mode_filter_envelope_sample_process (void) {

	bl_saw_set(&saw, f * (pp6_get_knob_3() + 1));
	bl_saw_set(&saw2, f * (pp6_get_knob_3() + 1) * (pp6_get_knob_1() + 1));

	//cutoff = ( sadsr_process(&filter_env) * f * 10.f ) + 200.f;//1000.f;// ;
	cutoff = sadsr_process(&filter_env) ;//(( sadsr_process(&filter_env) * 6000.f ) + 200.f) / 6000.f;//1000.f;// ;

	if (pp6_get_aux())
		vcf_filter_set(&filter, ((cutoff * cutoff) * 6000.f) + 200.f, pp6_get_knob_2() * 3.9f );
	else
		vcf_filter_set(&filter, ((cutoff * f * 40.f)) + f, pp6_get_knob_2() * 3.9f );
	//
	//	vcf_filter_set(&filter, ((cutoff) * 6000.f) + 200.f, pp6_get_knob_2() * 3.9f );


	//sig = bl_saw_process(&saw) * .75;

	sig = (bl_saw_process(&saw) * .3) + (bl_saw_process(&saw2) * .3);

	amp = sadsr_process(&amp_env);
	sig *= amp * amp;

	sig = vcf_filter_process(&filter, sig);

	return sig;
}

void mode_filter_envelope_control_process (void) {
	f = miditof[pp6_get_note()] * .6f;
	/*sadsr_set(&amp_env, .01f, 1.f, 1.f, .6f);
	sadsr_set(&filter_env, .001f, (pp6_get_knob_1() * 2.f) + .01f, 1.f, .1f);*/


	if (pp6_get_note_start()){
		sadsr_set(&amp_env, midi_cc[4], midi_cc[5] * 4.f, midi_cc[6] * 4.f, midi_cc[7]);
		sadsr_set(&filter_env, midi_cc[0], midi_cc[1] * 4.f, midi_cc[2] * 4.f, midi_cc[3]);

		sadsr_go(&amp_env);
		sadsr_go(&filter_env);
	}
	if (pp6_get_note_stop()){
		//sadsr_release(&amp_env);
		//sadsr_release(&filter_env);
	}
}
