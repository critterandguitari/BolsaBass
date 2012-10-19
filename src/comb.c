/*
 * comb.c
 *
 *  Created on: Apr 20, 2012
 *      Author: owen
 */

#include "comb.h"
//#include "vcf.h"


static float32_t delay_buffer[MAX_DELAY];
static uint32_t delay_time = 3000;
static float32_t delay_time_frac;

static float32_t delay_fb = .9f;
static uint32_t delay_read_index = 0;
static uint32_t delay_write_index = 0;

//static vcf_filter filter;





void comb_init(void) {
	uint32_t i;

	//vcf_filter_init(&filter);


	for (i=0; i<4096; i++) delay_buffer[i] = 0.f;

}

void comb_set_fb (float32_t fb) {
	delay_fb = fb;
}

void comb_set_dtime (float32_t dtime) {

	delay_time = (uint32_t) (dtime * SR);
	delay_time_frac = (dtime * SR) - (float32_t)delay_time;

	if (delay_time > (MAX_DELAY - 1)) {
		delay_time = MAX_DELAY - 1;
		delay_time_frac = 0;
	}

}

float32_t comb_process(float32_t in){


	  float32_t delay_out, delay_in, delay_out_p1;



	  delay_out = delay_buffer[(delay_write_index - delay_time) & 0xfff];
	  delay_out_p1 = delay_buffer[(delay_write_index - (delay_time + 1)) & 0xfff];

	  delay_out = delay_out + delay_time_frac * (delay_out_p1 - delay_out);


	  	 delay_in = in + delay_out * delay_fb;

	 	//vcf_filter_set(&filter, 2000.f,  2.f);

	 	//delay_in = vcf_filter_process(&filter, delay_in);

	    delay_buffer[delay_write_index] = delay_in;
	    delay_write_index++;
	    delay_write_index &= 0xfff;


	    return delay_in;
	//return in * .25f;
}
