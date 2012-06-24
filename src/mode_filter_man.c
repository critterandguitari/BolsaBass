/*
 * mode_filter_man.c
 *
 *  Created on: Jun 23, 2012
 *      Author: owen
 */


#include "arm_math.h"
#include "oscillator.h"
#include "pp6.h"
#include "line.h"
#include "vcf.h"
#include "mode_filter_man.h"

extern float miditof[];

extern pocket_piano pp6;

float32_t sig, f;
q15_t  wave;
sin_oscillator sin1;
bl_square sq1, sq2;
bl_saw saw;

uint32_t k, k_last, kdown, note, note_last, i;

square_oscillator oldsquare;
uint8_t mode =0;
float32_t k1;
vcf_filter filter;

float32_t amp = 0.f;

void mode_filter_man_init(void){
	bl_square_init(&sq1);
	bl_square_init(&sq2);
	bl_square_set(&sq1, 440);
	bl_square_set(&sq2, 460);

	f = 50.0;

	k = note = note_last = 0;
	vcf_filter_init(&filter);

}


float32_t mode_filter_man_sample_process (void) {


	sin_set(&sin1, f, .9f);
	bl_square_set(&sq1, f * (k1 + 1.f));
	bl_saw_set(&saw, f * (k1 + 1.f));
	squarewave_set(&oldsquare, f, .9f);


	sig = sin_process(&sin1);
	if (mode) sig = bl_square_process(&sq1) * .75f;
	else sig = bl_saw_process(&saw) * .75;

	amp -= .00001f;
	if (amp < 0) amp = 0.f;

	sig *= amp * amp;

		//if (mode)
			sig = vcf_filter_process(&filter, sig);

			return sig;
}

void mode_filter_man_control_process (void) {
	f = miditof[pp6_get_note()] * .6f;
	k1 =  pp6.knob_3 * 4.f;
	if (pp6_get_note_start()){
		amp = 1.f;
	}
	vcf_filter_set(&filter, (pp6.knob_1 * 6000.f) + 100.f, pp6.knob_2 * 3.9f );
	//vcf_filter_set(&filter, ((uint8_t)(pp6.knob_1 * 20) + 1) * f, pp6.knob_2 * 3.9f );
}
