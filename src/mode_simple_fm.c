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

extern float miditof[];

static float32_t sig, f;

static uint32_t new_note;

static float32_t amp;

static line framp;

static sadsr amp_env, index_env;




void mode_simple_fm_init(void){
	amp = 0;
	sadsr_init(&amp_env);
	sadsr_init(&index_env);
}

float32_t mode_simple_fm_sample_process (void) {

//	sig = simple_FM(f, (pp6_get_knob_1() * 2.f) + .5f, pp6_get_knob_2() * 5.f);
	//sig = simple_FM(line_process(&framp), (pp6_get_knob_1() * 2.f) + .5f, (float32_t)pp6_get_aux());
	sig = simple_FM(line_process(&framp), (float32_t)pp6_get_aux(), sadsr_process(&index_env) * 6.f);


	amp = sadsr_process(&amp_env);

	return sig * amp * amp;
}

void mode_simple_fm_control_process (void) {

	f = miditof[pp6_get_note()] * ((pp6_get_knob_3()) + 1.f);

	if (pp6_get_note_start() ){
		line_set(&framp, f * 2.f );
		line_go(&framp, f, 10.f);
		sadsr_set(&amp_env, .01f, 1.f, 1.f, .6f);
		sadsr_go(&amp_env);

		sadsr_set(&index_env, .1f, pp6_get_knob_1() * 2.f, 1.f, .1f);
		sadsr_go(&index_env);


		new_note = 1;
	}
	if (pp6_get_note_stop()){
		sadsr_release(&amp_env);
	}
}

