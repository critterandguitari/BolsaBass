/*
 * mode_wave_adder.c
 *
 *  Created on: Aug 17, 2012
 *      Author: owen
 */



#include "arm_math.h"
#include "oscillator.h"
#include "pp6.h"
#include "line.h"
#include "vcf.h"
#include "sadsr.h"
#include "audio.h"
#include "mode_wave_adder.h"


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



void mode_wave_adder_init(void){
	f = 50.0;
	amp = 0;
	bl_square_init(&square);
	sadsr_init(&amp_env);
}

float32_t mode_wave_adder_sample_process (void) {

	f = c_to_f(line_process(&framp));

	sin_set(&sin1, f * ((pp6_get_knob_3()) + 1) * 2.f, .9f);

	bl_saw_set(&saw, f * ((pp6_get_knob_3()) + 1));
	bl_square_set(&square, f * ((pp6_get_knob_3()) + 1));


	//sin_set(&sin1, f * ((pp6_get_knob_3() * 4.f) + 1), .9f);

	//sig = sin_process(&sin1) * .3f + bl_square_process(&square) *.3f + bl_saw_process(&saw) * .3f;

//	if (pp6_get_aux() == 0)
	//sig = bl_saw_process(&saw) * (1.f - pp6_get_knob_1()) + sin_process(&sin1) * (pp6_get_knob_1());
	sig = bl_saw_process(&saw) * .6f;

/*	else if (pp6_get_aux() == 1)
		sig = bl_square_process(&square);
	else if (pp6_get_aux() == 2)
		sig = bl_saw_process(&saw);
	else if (pp6_get_aux() == 3)
		sig = sin_process(&sin1);
	else if (pp6_get_aux() == 4)
		sig = bl_square_process(&square);
	else if (pp6_get_aux() == 5)
		sig = bl_saw_process(&saw);
*/


	amp = sadsr_process(&amp_env);

	return sig * amp * amp;
}

void mode_wave_adder_control_process (void) {

	static uint32_t note_dur;

	//release = 1.54f;
	//f = miditof[pp6_get_note()] * .6f;
	//f = c_to_f(cents);

	if (pp6_get_note_start() ){
		cents = ((float32_t)pp6_get_note() * 100.f);
		//f = c_to_f(cents);


		//line_set(&framp, f * pp6_get_knob_2()  * 10.f );
		line_set(&framp, cents + (pp6_get_knob_2()  * 1200.f));   // 2400 cents sharp
		line_go(&framp, cents, (pp6_get_knob_1() + .001f) * 500.f);
		sadsr_set(&amp_env, .01f, .2f, 1.54f, .6f);
		sadsr_go(&amp_env);
		new_note = 1;
		//sin_reset(&sin1);
		note_dur = 0;
	}
	if (pp6_get_note_stop()){
		if (note_dur < 100)
			sadsr_set(&amp_env, .01f, .1f, .15, .6f);
		sadsr_release(&amp_env);
	}
	note_dur++;
}
