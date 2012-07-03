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
#include "mode_filter_man.h"

extern float miditof[];

float32_t sig, f;

sin_oscillator sin1;


uint32_t note, note_last;

float32_t k1;

float32_t amp;

line framp;


void mode_simple_sin_init(void){
	f = 50.0;
	amp = 0;
	note = note_last = 0;

}


float32_t mode_simple_sin_sample_process (void) {


	sin_set(&sin1, line_process(&framp) * ((pp6_get_knob_3() * 4.f) + 1), .9f);


	sig = sin_process(&sin1);


	amp -= .00005f;
	if (amp < 0) amp = 0.f;

	sig *= amp * amp;

			return sig;
}

void mode_simple_sin_control_process (void) {
	f = miditof[pp6_get_note()] * .6f;
	if (pp6_get_note_start()){
		amp = 1.f;
		line_set(&framp, f * (pp6_get_knob_2() * 10.f) );
		line_go(&framp, f, (uint32_t) (pp6_get_knob_1() * 5000.f));
	}
}
