/*
 * vcf.c
 *
 *  Created on: Apr 1, 2012
 *      Author: owen
 */

#include "vcf.h"

void vcf_filter_init(vcf_filter * vcf) {

	// zero everything out
	vcf->input = vcf->in1 = vcf->in2 = vcf->in3 = vcf->in4 = 0;
	vcf->out1 = vcf->out2 = vcf->out3 = vcf->out4 = 0;
}

// this needs to be called at audio rate (or control rate)  because of the smoothing function
void vcf_filter_set(vcf_filter * vcf, float32_t cutoff, float32_t resonance) {
	// smooth cutoff
	static float32_t co = 0;
	float32_t kFilteringFactor = .05f;   // the smoothing factor 1 is no smoothing
	co = (cutoff * kFilteringFactor) + (co * (1.0 - kFilteringFactor));
	cutoff = co;


	vcf->res = resonance;
	vcf->fc = cutoff / (SR / 2.f);
	// calculate coeffs
	vcf->f = vcf->fc * 1.16f;
	vcf->fb = vcf->res * (1.0f - 0.15f * vcf->f * vcf->f);
}

float32_t vcf_filter_process(vcf_filter * vcf, float32_t in){
	vcf->input = in;
	vcf->input -= vcf->out4 * vcf->fb;
	vcf->input *= 0.35013f * (vcf->f * vcf->f)*(vcf->f * vcf->f);
	vcf->out1 = vcf->input + 0.3f * vcf->in1 + (1 - vcf->f) * vcf->out1; // Pole 1
	vcf->in1 = vcf->input;
	vcf->out2 = vcf->out1 + 0.3f * vcf->in2 + (1 - vcf->f) * vcf->out2; // Pole 2
	vcf->in2 = vcf->out1;
	vcf->out3 = vcf->out2 + 0.3f * vcf->in3 + (1 - vcf->f) * vcf->out3; // Pole 3
	vcf->in3 = vcf->out2;
	vcf->out4 = vcf->out3 + 0.3f * vcf->in4 + (1 - vcf->f) * vcf->out4; // Pole 4
	vcf->in4 = vcf->out3;
	return (float32_t)vcf->out4;
}
