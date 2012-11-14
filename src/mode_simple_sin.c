/*
 * mode_simple_sin.c
 *
 *  Created on: Jun 24, 2012
 *      Author: owen
 */


#include "arm_math.h"
#include "oscillator.h"
#include "pp6.h"
#include "line.h"
#include "vcf.h"
#include "sadsr.h"
#include "audio.h"
#include "mode_simple_sin.h"


extern float miditof[];

static float32_t sig, f;

static sin_oscillator sin1;

static uint32_t new_note;

static float32_t amp;

static line framp;

static sadsr amp_env;

static bl_saw saw;
static bl_square square;

static float32_t cents = 0.f;

static vcf_filter filter;



void mode_simple_sin_init(void){
	f = 50.0;
	amp = 0;
	bl_square_init(&square);
	sadsr_init(&amp_env);
}

float32_t mode_simple_sin_sample_process (void) {

	f = c_to_f(line_process(&framp));

	sin_set(&sin1, f * ((pp6_get_knob_3()) + 1), .9f);

	bl_saw_set(&saw, f * ((pp6_get_knob_3()) + 1));
	bl_square_set(&square, f * ((pp6_get_knob_3()) + 1));


	//sin_set(&sin1, f * ((pp6_get_knob_3() * 4.f) + 1), .9f);

	//sig = sin_process(&sin1) * .3f + bl_square_process(&square) *.3f + bl_saw_process(&saw) * .3f;

		sig = sin_process(&sin1) * .6f;



  // distortion ?
	//a = 75; //this is the cool tone
	//sig = sig*(ABS(sig) + a)/((sig * sig) + (a-1)*ABS(sig) + 1);



	amp = sadsr_process(&amp_env);

	// remove artifacts
	vcf_filter_set(&filter, 2000.f,  1.f);
	sig = vcf_filter_process(&filter, sig);

	return sig * amp * amp;
}

void mode_simple_sin_control_process (void) {

	static uint32_t note_dur;

	//release = 1.54f;
	//f = miditof[pp6_get_note()] * .6f;
	//f = c_to_f(cents);

	if (pp6_get_synth_note_start() ){
		cents = ((float32_t)pp6_get_synth_note() * 100.f);
		//f = c_to_f(cents);


		//line_set(&framp, f * pp6_get_knob_2()  * 10.f );
		line_set(&framp, cents + (pp6_get_knob_2()  * 2400.f));   // 2400 cents sharp
		line_go(&framp, cents, (pp6_get_knob_1() + .001f) * 1500.f);
		sadsr_set(&amp_env, .01f, .2f, 1.54f, .6f);
		sadsr_go(&amp_env);
		new_note = 1;
		//sin_reset(&sin1);
		note_dur = 0;
	}
	if (pp6_get_synth_note_stop()){
		if (note_dur < 100)
			sadsr_set(&amp_env, .01f, .1f, .15, .6f);
		sadsr_release(&amp_env);
	}
	note_dur++;
}
