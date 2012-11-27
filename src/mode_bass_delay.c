/*
 * mode_bass_delay.c
 *
 *  Created on: Oct 18, 2012
 *      Author: owen
 */



#include "arm_math.h"
#include "oscillator.h"
#include "pp6.h"
#include "line.h"
#include "vcf.h"
#include "sadsr.h"
#include "audio.h"
#include "comb.h"
#include "mode_bass_delay.h"


extern float miditof[];

static float32_t sig, f;

static sin_oscillator sin1;

static uint32_t new_note;

static float32_t amp;

static line framp;

static sadsr amp_env;

static float32_t cents = 0.f;


static vcf_filter filter;


void mode_bass_delay_init(void){
	f = 50.0;
	amp = 0;
	comb_init();
	sadsr_init(&amp_env);
	vcf_filter_init(&filter);
}

float32_t mode_bass_delay_sample_process (void) {

	f = c_to_f(line_process(&framp));
	f = f * ((pp6_get_knob_3()) + 1);

	//comb_set_dtime(pp6_get_knob_1() + (1 / f));
	comb_set_dtime((pp6_get_knob_1() * .1f) + (2 / f));


	comb_set_fb((pp6_get_knob_2() * (pp6_get_knob_1() + .1f) ) + (1 - (pp6_get_knob_1() + .1f)) );

	// get sin wave
	sin_set(&sin1, f * 2, 1.f);
	sig = sin_process(&sin1);

	// apply envelop
	amp = sadsr_process(&amp_env);
	sig = amp * amp * sig;

	// make it more of a square wave
 	//a = 50.f; //this is the cool tone
 	//sig = sig*(ABS(sig) + a)/((sig * sig) + (a-1)*ABS(sig) + 1);
 	sig *= .1f;

 	// process comb
	sig = comb_process(sig);

	// remove artifacts
	vcf_filter_set(&filter, 8000.f,  1.f);

	sig = vcf_filter_process(&filter, sig);

	return sig;
}

void mode_bass_delay_control_process (void) {


	static uint32_t note_dur;

	//release = 1.54f;
	//f = miditof[pp6_get_note()] * .6f;
	//f = c_to_f(cents);

	if (pp6_get_synth_note_start() ){
		cents = ((float32_t)pp6_get_synth_note() * 100.f);
		//f = c_to_f(cents);


		//line_set(&framp, f * pp6_get_knob_2()  * 10.f );
		line_set(&framp, cents +2400.f);   // 2400 cents sharp
		line_go(&framp, cents, .01f);
		sadsr_set(&amp_env, .001f, .01f + (pp6_get_knob_1() * .5f), .01f,  .1f);
		sadsr_go(&amp_env);
		new_note = 1;
		//sin_reset(&sin1);
		note_dur = 0;
	}
	if (pp6_get_synth_note_stop()){
		//if (note_dur < 100)
		//	sadsr_set(&amp_env, .001f, .01f, .015, .6f);
		sadsr_release(&amp_env);
	}
	note_dur++;
}
