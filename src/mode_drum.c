/*
 * mode_drum.c
 *
 *  Created on: Jul 21, 2012
 *      Author: owen
 */


#include "stm32f4xx.h"
#include "arm_math.h"
#include "oscillator.h"
#include "pp6.h"
#include "line.h"
#include "vcf.h"
#include "sadsr.h"
#include "mode_drum.h"

extern float miditof[];


static float32_t sig, f, cutoff;
static vcf_filter filter;
static float32_t amp = 0.f;
static sadsr amp_env, filter_env, sin_amp_env;


static sin_oscillator sin1;
static line framp;


void mode_drum_init(void){

	f = 0;
	vcf_filter_init(&filter);
	sadsr_init(&filter_env);
	sadsr_init(&amp_env);
	sadsr_init(&sin_amp_env);

}


float32_t mode_drum_sample_process (void) {

	float32_t sin, sin_amp;

	// sin wave
	sin_set(&sin1, line_process(&framp) * ((pp6_get_knob_3()) + 1), .9f);
	sin = sin_process(&sin1);
	sin_amp = sadsr_process(&sin_amp_env);
	sin *= sin_amp * sin_amp * .5f;


	cutoff = sadsr_process(&filter_env) ;//(( sadsr_process(&filter_env) * 6000.f ) + 200.f) / 6000.f;//1000.f;// ;
		vcf_filter_set(&filter, ((cutoff * cutoff) * 6000.f) + 200.f, pp6_get_knob_2() * 3.9f );


	//bl_saw_set(&saw, f);
	sig = ((float32_t) RNG_GetRandomNumber() / 2147483648.f) * .6f;// / 2147483648.0f;;
	amp = sadsr_process(&amp_env);

	sig *= amp * amp;

		//if (mode)
	sig = vcf_filter_process(&filter, sig);


	return sig + sin;
}

void mode_drum_control_process (void) {


	//f = miditof[pp6_get_note()] * .6f * (pp6_get_knob_3() +  1.f);
	f = c_to_f(((float32_t)pp6_get_synth_note() * 100.f)) * (pp6_get_knob_3() +  1.f);



	sadsr_set(&amp_env, .01f, .2f, .2f, 0.f);
	sadsr_set(&filter_env, .001f, (pp6_get_knob_1() * 2.f) + .01f, 1.f, .1f);
	if (pp6_get_synth_note_start()){
	/*count++;
	all_clock++;
	aux = pp6_get_aux();
	if (aux != aux_last) {
		aux_last = aux;
		machine_gun_period = (all_clock - last_hit) / 4;
		last_hit = all_clock;

	}*/

	//if ((count > machine_gun_period) && pp6_get_num_keys_down() ) {
	//	count = 0;
		sadsr_go(&amp_env);
		sadsr_go(&filter_env);

		// sin wave
		line_set(&framp, f * pp6_get_knob_2()  * 10.f );
		line_go(&framp, f, pp6_get_knob_1() * 500.f);
		sadsr_set(&sin_amp_env, .01f, .1f, .2f, 0.f);
		sadsr_go(&sin_amp_env);

	}
	if (pp6_get_synth_note_stop()){
		sadsr_release(&amp_env);
		sadsr_release(&filter_env);
		sadsr_release(&sin_amp_env);
	}
	vcf_filter_set(&filter, (pp6_get_knob_1() * 6000.f) + 100.f, pp6_get_knob_2() * 3.9f );
	//vcf_filter_set(&filter, f * 2.f, pp6_get_knob_2() * 3.9f );
}
