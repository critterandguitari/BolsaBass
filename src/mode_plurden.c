/*
 * mode_plurden.c
 *
 *  Created on: Oct 16, 2012
 *      Author: owen
 */

#include "arm_math.h"
#include "oscillator.h"
#include "pp6.h"
#include "line.h"
#include "vcf.h"
#include "sadsr.h"
#include "audio.h"
#include "mode_plurden.h"


extern float miditof[];

static float32_t sig, f;

static sin_oscillator sin1;
static sin_oscillator sin2;

static uint32_t new_note;

static float32_t amp;

static line framp;

static sadsr amp_env;

static bl_saw saw;

static float32_t cents = 0.f;



void mode_plurden_init(void){
	f = 50.0;
	amp = 0;
	//bl_saw_init(&saw);
	sadsr_init(&amp_env);
}

float32_t mode_plurden_sample_process (void) {

	float32_t a;
	float32_t lfo;

	f = c_to_f(line_process(&framp));

	//sin_set(&sin3, 10.f * pp6_get_knob_2() + 1.5f, 0.5f);
	bl_saw_set(&saw, 100.f * pp6_get_knob_2());




	//lfo = sin_process(&sin3);
	lfo = bl_saw_process(&saw) * .5f;

	f = f + (lfo*1000.f * pp6_get_knob_1() );

	sin_set(&sin1, f * ((pp6_get_knob_3()) + 1), .9f);
	sin_set(&sin2, f*2 * ((pp6_get_knob_3()) + 1), .5f);


		sig = sin_process(&sin1) * .5f + sin_process(&sin2);

		//sig = sig * ((lfo + 1) * .5f);

	amp = sadsr_process(&amp_env);

	//a = pp6_get_knob_2() * 500.f;
	a = 0.15f * 500.f; //this is the cool tone
	sig = sig*(ABS(sig) + a)/((sig * sig) + (a-1)*ABS(sig) + 1);

	return sig * amp * amp * .15f;
}

void mode_plurden_control_process (void) {

	static uint32_t note_dur;
	static uint8_t last_note = 60;

	if (pp6_get_synth_note_start() ){
		cents = ((float32_t)pp6_get_synth_note() * 100.f);
		//f = c_to_f(cents);


		//line_set(&framp, f * pp6_get_knob_2()  * 10.f );
		line_set(&framp, last_note * 100.f);   // 2400 cents sharp
		//line_go(&framp, cents, (pp6_get_knob_1() + .001f) * 1500.f);  //glide
		line_go(&framp, cents, 10);  //glide

		sadsr_set(&amp_env, .01f, .2f, 2.f, .6f);
		sadsr_go(&amp_env);
		new_note = 1;
		//sin_reset(&sin1);
		note_dur = 0;

		last_note = pp6_get_synth_note();
	}
	if (pp6_get_synth_note_stop()){
		if (note_dur < 100)
			sadsr_set(&amp_env, .01f, .1f, .15, .6f);
		sadsr_release(&amp_env);
	}
	note_dur++;
}
