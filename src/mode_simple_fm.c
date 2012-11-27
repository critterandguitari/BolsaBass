/*
 * mode_simple_fm.c
 *
 *  Created on: Jul 27, 2012
 *      Author: owen
 */
#include "arm_math.h"
#include "oscillator.h"
#include "pp6.h"
#include "line.h"
#include "vcf.h"
#include "sadsr.h"
#include "mode_simple_fm.h"
#include "timer.h"

extern float miditof[];

static float32_t sig, f, target_f;

static uint32_t new_note;

static float32_t amp;

static line framp;

static sadsr amp_env, index_env;

static FM_oscillator fm, fm2;


static vcf_filter filter;



void mode_simple_fm_init(void){
	amp = 0;
	sadsr_init(&amp_env);
	sadsr_init(&index_env);
}

float32_t mode_simple_fm_sample_process (void) {

	uint32_t t, t1, t2;
	float32_t b, c;

	f = c_to_f(line_process(&framp)) * (pp6_get_knob_3() +  1.f);


	//sig = simple_FM(f, 1.f + (.05f * pp6_get_knob_1()),  pp6_get_knob_2() * 1.5f);
	sig = FM_oscillator_process(&fm, f, 1.f + (.04f * pp6_get_knob_1()),  pp6_get_knob_2() * 1.5f) * .5f;

	sig += FM_oscillator_process(&fm2, f * (1.f + (.03f * pp6_get_knob_1())), 1.f + (.05f * pp6_get_knob_1()),  pp6_get_knob_2() * 1.5f) * .5f;



	amp = sadsr_process(&amp_env);

	//a = b = 2;
	//c = powf(a, b);

	sig = sig * amp * amp * .5f;

	// remove artifacts
	vcf_filter_set(&filter, 4000.f,  1.f);

	sig = vcf_filter_process(&filter, sig);

	return sig;
}

void mode_simple_fm_control_process (void) {
	static uint32_t note_dur = 0;

	//f = miditof[pp6_get_note()] * ((pp6_get_knob_3()) + 1.f);

	if (pp6_get_synth_note_start() ){

		target_f = (float32_t)pp6_get_synth_note() * 100.f;
		line_go(&framp, target_f, 2.f);

		sadsr_set(&amp_env, .01f, .1f, 1.f, .6f);
		sadsr_go(&amp_env);

		sadsr_set(&index_env, .01f, pp6_get_knob_1() * 2.f, 1.f, 0);
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

