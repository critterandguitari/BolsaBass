/*
 * comb.c
 *
 *  Created on: Apr 20, 2012
 *      Author: owen
 */

#include "comb.h"



float32_t delay_buffer[4096];
uint32_t delay_time = 406;
float32_t delay_fb = .9f;
uint32_t delay_read_index = 0;
uint32_t delay_write_index = 0;


void comb_init(void) {
	uint32_t i;

	for (i=0; i<4096; i++) delay_buffer[i] = 0.f;

}

float32_t comb_process(float32_t in){


	  float32_t delay_out, delay_in;



	  delay_out = delay_buffer[(delay_write_index - delay_time) & 0xfff];

	//  delay_out = 0.f;
	//  delay_fb = 0.f;

	  //delay_in = (delay_out * delay_fb) + in;

	  //delay_in = (delay_out) + in;

	  	 delay_in = in + delay_out * .6f;

	    delay_buffer[delay_write_index] = delay_in;
	    delay_write_index++;
	    delay_write_index &= 0xfff;


	    return delay_in;
	//return in * .25f;
}
