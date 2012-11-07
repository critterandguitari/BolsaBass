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

	// adjust dtime smoothly
	static float32_t dt = 0;
	dtime *= SR;  // convert to samples, then smooth
	float32_t kFilteringFactor = .05f;   // the smoothing factor 1 is no smoothing
	dt = (dtime * kFilteringFactor) + (dt * (1.0 - kFilteringFactor));
	dtime = dt;


	delay_time = (uint32_t) dtime;
	delay_time_frac = dtime - (float32_t)delay_time;

	if (delay_time > (MAX_DELAY - 1)) {
		delay_time = MAX_DELAY - 1;
		delay_time_frac = 0;
	}

}

float32_t comb_process(float32_t in){


	  float32_t delay_out, delay_in, delay_out_p1;


	  // with linear interp
	  delay_out = delay_buffer[(delay_write_index - delay_time) & 0xfff];
	  delay_out_p1 = delay_buffer[(delay_write_index - (delay_time + 1)) & 0xfff];
	  // 2 point
	  delay_out = delay_out + delay_time_frac * (delay_out_p1 - delay_out);

	  //4 point interp
	  // this works, but doesn't sound much better, so linear is fine
      // get neighbourgh samples
    /*  float32_t const y_1= delay_buffer [(delay_write_index - (delay_time - 1)) & 0xfff];
      float32_t const y0 = delay_buffer [(delay_write_index - delay_time) & 0xfff];
      float32_t const y1 = delay_buffer [(delay_write_index - (delay_time + 1)) & 0xfff];
      float32_t const y2 = delay_buffer [(delay_write_index - (delay_time + 2)) & 0xfff];

      // compute interpolation x
      float32_t const x=delay_time_frac;

      // calculate
      float32_t const c0 = y0;
      float32_t const c1 = 0.5f*(y1-y_1);
      float32_t const c2 = y_1 - 2.5f*y0 + 2.0f*y1 - 0.5f*y2;
      float32_t const c3 = 0.5f*(y2-y_1) + 1.5f*(y0-y1);

      float32_t const output=((c3*x+c2)*x+c1)*x+c0;

      delay_out = output;*/




	  	 delay_in = in + delay_out * delay_fb;

	 	//vcf_filter_set(&filter, 2000.f,  2.f);
	 	//delay_in = vcf_filter_process(&filter, delay_in);



	    delay_buffer[delay_write_index] = delay_in;
	    delay_write_index++;
	    delay_write_index &= 0xfff;


	    return delay_in;
	//return in * .25f;
}
