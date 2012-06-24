/*
 * vcf.h
 *
 *  Created on: Apr 1, 2012
 *      Author: owen
 */

#ifndef VCF_H_
#define VCF_H_

#include "arm_math.h"
#include "audio.h"

typedef struct {
	float32_t fc;   // cutoff
	float32_t res;  // resonance

	float32_t f;
	float32_t fb;

	float32_t input, in1, in2, in3, in4;
	float32_t out1, out2, out3, out4;

} vcf_filter;

void vcf_filter_init(vcf_filter * vcf);

void vcf_filter_set(vcf_filter * vcf, float32_t cutoff, float32_t resonance);

float32_t vcf_filter_process(vcf_filter * vcf, float32_t in);




#endif /* VCF_H_ */
