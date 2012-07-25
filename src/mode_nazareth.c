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

static float32_t sig, f, t;

static sin_oscillator sins[8];

static uint32_t new_note, i;

static float32_t amp;

static float32_t harmonic_levels[8] = {1.f, .9f, .8f, .7f, .7f, .5f, .4f, .3f};

static bl_saw saws[8];

static sadsr amp_env;


void mode_nazareth_init(void){
	f = 50.0;
	amp = 0;
	sadsr_init(&amp_env);
}

float32_t mode_nazareth_sample_process (void) {


	for (i=0; i<8; i++){
		sin_set(&sins[i], f * (pp6_get_knob_3() + 1) * (i + 1), .9f);
		//harmonic_levels[i] = 1.f - ((bl_saw_process(&saws[i]) + 1.f) * .5f) ;
	}
	sig = 0;
	for (i=0; i<8; i++){
		sig += sin_process(&sins[i]) * harmonic_levels[i] * .125f;
	}

	amp = sadsr_process(&amp_env);

	return sig * amp * amp;
}

void mode_nazareth_control_process (void) {

	f = miditof[pp6_get_note()] * 6.f;
	if (pp6_get_note_start() ){
		sadsr_set(&amp_env, .01f, 1.f, .2f, .6f);
		sadsr_go(&amp_env);
		new_note = 1;
	}
	if (pp6_get_note_stop()){
		sadsr_release(&amp_env);
	}

	// set harmonic levels
	//t = 1.f;
	//for(i = 0; i < 8; i++){
		//harmonic_levels[i] = (float32_t)(((uint32_t)(pp6_get_knob_1() * 256)  * (i + 1)) & 0xff) / 256.f;
		//t *= 2.f;
		//bl_saw_set(&saws[i], (float32_t)(i + 1) * pp6_get_knob_1() * 10.f);
	//}


		harmonic_levels[0] = .7f;
		harmonic_levels[1] = 1.f;
		harmonic_levels[2] = .3f;
		harmonic_levels[3] = 1.f;
		harmonic_levels[4] = .5f;
		harmonic_levels[5] = .3f;
		harmonic_levels[6] = .2f;
		harmonic_levels[7] = 1.f;

		harmonic_levels[0] = midi_cc[0];
		harmonic_levels[1] = midi_cc[1];
		harmonic_levels[2] = midi_cc[2];
		harmonic_levels[3] = midi_cc[3];
		harmonic_levels[4] = midi_cc[4];
		harmonic_levels[5] = midi_cc[5];
		harmonic_levels[6] = midi_cc[6];
		harmonic_levels[7] = midi_cc[7];

}
