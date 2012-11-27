/*
 * mode_fm_ramp.c
 *
 *  Created on: Oct 4, 2012
 *      Author: owen
 */

#include "arm_math.h"
#include "oscillator.h"
#include "pp6.h"
#include "line.h"
#include "vcf.h"
#include "sadsr.h"
#include "mode_fm_ramp.h"
#include "timer.h"

extern float miditof[];

static float32_t sig, f, target_f, index;

static uint32_t new_note;

static float32_t amp;

static line framp;

static sadsr amp_env, index_env;

static FM_oscillator fm;





void mode_fm_ramp_init(void){
	amp = 0;
	sadsr_init(&amp_env);
	sadsr_init(&index_env);
}

float32_t mode_fm_ramp_sample_process (void) {


	f = c_to_f(line_process(&framp)) * (pp6_get_knob_3() +  1.f);

	index = sadsr_process(&index_env);

	sig = FM_oscillator_process(&fm, f, 1.0f,  index * 3.f) * .5;

	amp = sadsr_process(&amp_env);

	//a = b = 2;
	//c = powf(a, b);

	// scale sig to 0 - 1



	/*
	a = pp6_get_knob_1();
	sig += 1.f;
	sig *= 2.f;

 	if (sig < a)
		sig = sig;
	else if (sig > a)
		sig = a + (sig-a)/(1+((sig-a)/(1-a))) * (sig-a)/(1+((sig-a)/(1-a)));
	else if (sig > 1)
		sig = (a+1)/2;*/



	//a = pp6_get_knob_1() * 100.f;
	//sig = sig*(ABS(sig) + a)/((sig * sig) + (a-1)*ABS(sig) + 1);


	return sig * amp * amp * .5f;
}

void mode_fm_ramp_control_process (void) {
	static uint32_t note_dur = 0;

	//f = miditof[pp6_get_note()] * ((pp6_get_knob_3()) + 1.f);

	if (pp6_get_synth_note_start() ){

		target_f = (float32_t)pp6_get_synth_note() * 100.f;
		line_go(&framp, target_f, 2.f);

		sadsr_set(&amp_env, .01f, .1f, 1.f, .6f);
		sadsr_go(&amp_env);

		sadsr_set(&index_env, .01f, pp6_get_knob_2() * 2.f, 1.f, 0);
		sadsr_go(&index_env);


		new_note = 1;
		note_dur = 0;
	}
	if (pp6_get_synth_note_stop()){
		if (note_dur < 100)
			sadsr_set(&amp_env, .01f, .1f, .1, .6f);
		sadsr_release(&amp_env);
	}
	note_dur++;
}
