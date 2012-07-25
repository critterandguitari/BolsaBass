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
#include "mode_simple_sin.h"

extern float miditof[];

static float32_t sig, f;

static sin_oscillator sin1;

static uint32_t new_note;

static float32_t amp;

static line framp;

static sadsr amp_env;




void mode_simple_sin_init(void){
	f = 50.0;
	amp = 0;
	sadsr_init(&amp_env);
}

float32_t mode_simple_sin_sample_process (void) {


	sin_set(&sin1, line_process(&framp) * ((pp6_get_knob_3()) + 1), .9f);

	//sin_set(&sin1, f * ((pp6_get_knob_3() * 4.f) + 1), .9f);

	sig = sin_process(&sin1);

	/*if ((sig > 0) && (sig_last < 0) && (new_note)) {
		new_note = 0;
		line_set(&framp, f * pp6_get_knob_2()  * 10.f );
		line_go(&framp, f, pp6_get_knob_1() * 500.f);
		sadsr_set(&amp_env, .01f, 1.f, .2f);
		sadsr_go(&amp_env);
	}
	sig_last = sig;*/
/*
	amp -= .00005f;
	if (amp < 0) amp = 0.f;

	sig *= amp * amp;
	*/

	amp = sadsr_process(&amp_env);

	return sig * amp * amp;
}

void mode_simple_sin_control_process (void) {
	f = miditof[pp6_get_note()] * .6f;
	if (pp6_get_note_start() ){
		line_set(&framp, f * pp6_get_knob_2()  * 10.f );
		line_go(&framp, f, pp6_get_knob_1() * 500.f);
		sadsr_set(&amp_env, .01f, 1.f, .2f, .6f);
		sadsr_go(&amp_env);
		new_note = 1;
		//sin_reset(&sin1);
	}
	if (pp6_get_note_stop()){
		sadsr_release(&amp_env);
	}
}
