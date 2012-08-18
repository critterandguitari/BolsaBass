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

static float32_t sig, f, amp, cutoff, cutoff_scale;
static bl_saw saw;
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


	cutoff = sadsr_process(&filter_env) ;

	//cutoff_scale = 4000.f +  (pp6_get_knob_1() * 3000.f);  // for shorter envelopes (also controlled by knob 1), start cutoff sweep lower
	cutoff_scale = 7000.f;  // for shorter envelopes (also controlled by knob 1), start cutoff sweep lower


	// log cutoff ramp using cents,  starts 6000 cents above f
	vcf_filter_set(&filter, ((c_to_f_ratio(cutoff * cutoff_scale)) * f) + f * 2, pp6_get_knob_2() * 3.f );


	sig = bl_saw_process(&saw) * .75;

	amp = sadsr_process(&amp_env);

	sig = vcf_filter_process(&filter, sig);
	sig *= amp * amp;

	return sig;
}

void mode_filter_envelope_control_process (void) {
	static uint32_t note_dur = 0;




	f = c_to_f((float32_t)pp6_get_note() * 100.f);



	if (pp6_get_note_start()){
		sadsr_set(&amp_env, .01f, 1.f, 1.54f, .6f);
		sadsr_set(&filter_env, .001f, ((pp6_get_knob_1() + .001f) * 4.f) + .01f, 10.f, 0);
		/*sadsr_set(&amp_env, midi_cc[4], midi_cc[5] * 4.f, midi_cc[6] * 4.f, midi_cc[7]);
		sadsr_set(&filter_env, midi_cc[0], midi_cc[1] * 4.f, midi_cc[2] * 4.f, midi_cc[3]);*/

		sadsr_go(&amp_env);
		sadsr_go(&filter_env);
		note_dur = 0;

	}
	if (pp6_get_note_stop()){
		if (note_dur < 100)
			sadsr_set(&amp_env, .01f, .1f, .2, .6f);
		sadsr_release(&amp_env);
		sadsr_release(&filter_env);
	}
	note_dur++;
}
