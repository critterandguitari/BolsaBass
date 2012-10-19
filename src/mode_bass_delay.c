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

static bl_saw saw;
static bl_square square;

static float32_t cents = 0.f;



void mode_bass_delay_init(void){
	f = 50.0;
	amp = 0;
	sadsr_init(&amp_env);
}

float32_t mode_bass_delay_sample_process (void) {

	f = c_to_f(line_process(&framp));
	f = f * ((pp6_get_knob_3()) + 1);

	//comb_set_dtime(pp6_get_knob_1() + (1 / f));
	comb_set_dtime((pp6_get_knob_1() * .1f) + (2 / f));


	comb_set_fb((pp6_get_knob_2() * .2f) + .8f);

	sin_set(&sin1, f * 4, .25f);

	sig = sin_process(&sin1);


	amp = sadsr_process(&amp_env);

	sig = comb_process(sig * amp * amp);


	return sig;
}

void mode_bass_delay_control_process (void) {


	static uint32_t note_dur;

	//release = 1.54f;
	//f = miditof[pp6_get_note()] * .6f;
	//f = c_to_f(cents);

	if (pp6_get_note_start() ){
		cents = ((float32_t)pp6_get_note() * 100.f);
		//f = c_to_f(cents);


		//line_set(&framp, f * pp6_get_knob_2()  * 10.f );
		line_set(&framp, cents +2400.f);   // 2400 cents sharp
		line_go(&framp, cents, .01f);
		sadsr_set(&amp_env, .01f, .001f + (pp6_get_knob_1() * .5f), .001f,  .1f);
		sadsr_go(&amp_env);
		new_note = 1;
		//sin_reset(&sin1);
		note_dur = 0;
	}
	if (pp6_get_note_stop()){
		if (note_dur < 100)
			sadsr_set(&amp_env, .001f, .001f, .015, .6f);
		sadsr_release(&amp_env);
	}
	note_dur++;
}
