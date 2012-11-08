/*
 * mode_nazareth.c
 *
 *  Created on: Jul 13, 2012
 *      Author: owen
 */

#include "arm_math.h"
#include "oscillator.h"
#include "pp6.h"
#include "line.h"
#include "vcf.h"
#include "sadsr.h"
#include "mode_nazareth.h"

extern float miditof[];

extern float32_t midi_cc[];

static float32_t sig, f;

static sin_oscillator sins[8];
static sin_oscillator amp_sins[8];
static sin_oscillator sin_for_lfo;

static uint32_t new_note, i;

static float32_t amp;

static float32_t harmonic_levels[8] = {0,0,0,0, 0,0,0,0};

//static bl_saw saws[8];

static sadsr amp_env;

static sadsr h[8];


void mode_nazareth_init(void){
	f = 50.0;
	amp = 0;
	sadsr_init(&amp_env);

	sadsr_init(&h[0]);
	sadsr_init(&h[1]);
	sadsr_init(&h[2]);
	sadsr_init(&h[3]);
	sadsr_init(&h[4]);
	sadsr_init(&h[5]);
	sadsr_init(&h[6]);
	sadsr_init(&h[7]);
}

float32_t mode_nazareth_sample_process (void) {

	/*harmonic_levels[0] = (arm_sin_f32(pp6_get_knob_1() * TWO_PI) + 1) * .5f;
	harmonic_levels[1] = (arm_sin_f32(pp6_get_knob_1() * TWO_PI * 2) + 1) * .5f;
	harmonic_levels[2] = (arm_sin_f32(pp6_get_knob_1() * TWO_PI * 4) + 1) * .5f;
	harmonic_levels[3] = (arm_sin_f32(pp6_get_knob_1() * TWO_PI * 8) + 1) * .5f;
	harmonic_levels[4] = (arm_sin_f32(pp6_get_knob_1() * TWO_PI * 16) + 1) * .5f;
	harmonic_levels[5] = (arm_sin_f32(pp6_get_knob_1() * TWO_PI * 32) + 1) * .5f;
	harmonic_levels[6] = (arm_sin_f32(pp6_get_knob_1() * TWO_PI * 64) + 1) * .5f;
	harmonic_levels[7] = (arm_sin_f32(pp6_get_knob_1() * TWO_PI * 128) + 1) * .5f;*/

	//LFO Stuff
	float32_t lfo;
	sin_set(&sin_for_lfo, 10.f * pp6_get_knob_2() + 1.5f, 0.5f);
	lfo = sin_process(&sin_for_lfo);

	harmonic_levels[0] = (arm_sin_f32(pp6_get_knob_1() * TWO_PI) + 1) * .5f;
	harmonic_levels[1] = (arm_sin_f32(pp6_get_knob_1() * TWO_PI * 2) + 1) * .5f;
	harmonic_levels[2] = (arm_sin_f32(pp6_get_knob_1() * TWO_PI * 4) + 1) * .5f;
	harmonic_levels[3] = (arm_sin_f32(pp6_get_knob_1() * TWO_PI * 8) + 1) * .5f;
	harmonic_levels[4] = (arm_sin_f32(pp6_get_knob_1() * TWO_PI * 16) + 1) * .5f;
	harmonic_levels[5] = (arm_sin_f32(pp6_get_knob_1() * TWO_PI * 32) + 1) * .5f;
	harmonic_levels[6] = (arm_sin_f32(pp6_get_knob_1() * TWO_PI * 64) + 1) * .5f;
	harmonic_levels[7] = (arm_sin_f32(pp6_get_knob_1() * TWO_PI * 128) + 1) * .5f;

	//f = f + (lfo*10.f); this sounds funny here

	for (i=0; i<8; i++){
		f = f + lfo; //LFO stuff
		sin_set(&sins[i], f * (pp6_get_knob_3() + 1) * (i + 1), .9f);

		//harmonic_levels[i] = (sin_process(&amp_sins[i]) + 1 ) * .5f;
		//harmonic_levels[i] = sadsr_process(&h[i]);
	}



	sig = 0;
	for (i=0; i<8; i++){
		amp = harmonic_levels[i];
		sig += sin_process(&sins[i]) * amp * amp * .125f;
	}
	amp = sadsr_process(&amp_env);

	return sig * amp * amp;
}

void mode_nazareth_control_process (void) {

	sin_set(&amp_sins[0], .0625f, 1);
	sin_set(&amp_sins[1], .0125f, 1);
	sin_set(&amp_sins[2], .025f, 1);
	sin_set(&amp_sins[3], .05f, 1);
	sin_set(&amp_sins[4], .1f, 1);
	sin_set(&amp_sins[5], .2f, 1);
	sin_set(&amp_sins[6], .4f, 1);
	sin_set(&amp_sins[7], .8f, 1);


	f = miditof[pp6_get_synth_note()] * .6f;
	if (pp6_get_synth_note_start() ){
		sadsr_set(&amp_env, .01f, 1.f, .2f, .6f);
		sadsr_go(&amp_env);


		/*sadsr_set(&h[0], .01f, 4.f * pp6_get_knob_1(), .2f, 0);
		sadsr_set(&h[1], .01f, 3.5f * pp6_get_knob_1(), .2f, 0);
		sadsr_set(&h[2], .01f, 3.f * pp6_get_knob_1(), .2f, 0);
		sadsr_set(&h[3], .01f, 2.5f * pp6_get_knob_1(), .2f, 0);
		sadsr_set(&h[4], .01f, 2.f * pp6_get_knob_1(), .2f, 0);
		sadsr_set(&h[5], .01f, 1.5f * pp6_get_knob_1(), .2f, 0);
		sadsr_set(&h[6], .01f, 1.f * pp6_get_knob_1(), .2f, 0);
		sadsr_set(&h[7], .01f, 1.f * pp6_get_knob_1(), .2f, 0);*/
	/*	sadsr_go(&h[0]);
		sadsr_go(&h[1]);
		sadsr_go(&h[2]);
		sadsr_go(&h[3]);
		sadsr_go(&h[4]);
		sadsr_go(&h[5]);
		sadsr_go(&h[6]);
		sadsr_go(&h[7]);*/

		new_note = 1;
	}
	if (pp6_get_synth_note_stop()){
		sadsr_release(&amp_env);
	}

	// set harmonic levels
	//t = 1.f;
	//for(i = 0; i < 8; i++){
		//harmonic_levels[i] = (float32_t)(((uint32_t)(pp6_get_knob_1() * 256)  * (i + 1)) & 0xff) / 256.f;
		//t *= 2.f;
		//bl_saw_set(&saws[i], (float32_t)(i + 1) * pp6_get_knob_1() * 10.f);
	//}






		/*harmonic_levels[0] = midi_cc[0];
		harmonic_levels[1] = midi_cc[1];
		harmonic_levels[2] = midi_cc[2];
		harmonic_levels[3] = midi_cc[3];
		harmonic_levels[4] = midi_cc[4];
		harmonic_levels[5] = midi_cc[5];
		harmonic_levels[6] = midi_cc[6];
		harmonic_levels[7] = midi_cc[7];*/

}
