/*
 * delay.c
 *
 *  Created on: Oct 19, 2012
 *      Author: owen
 */



#include "delay.h"
#include <math.h>

static  float32_t    buffer[MAX_WG_DELAY];
static   int        counter;
static float32_t feedback;
static float32_t delay;


void delay_init(void) {
	int s;
	counter=0;
	for(s=0;s<MAX_WG_DELAY;s++)
		buffer[s]=0;
}

void delay_set_fb (float32_t fb) {
	feedback = fb;
}

void delay_set_dtime (float32_t dtime) {

	delay = (float32_t) (dtime * SR);

}

float32_t delay_process(float32_t in){

        // calculate delay offset
        float32_t back=(float32_t)counter-delay;

        // clip lookback buffer-bound
        if(back<0.0)
            back=MAX_WG_DELAY+back;

        // compute interpolation left-floor
        int const index0=(int)floorf(back);

        // compute interpolation right-floor
        int index_1=index0-1;
        int index1=index0+1;
        int index2=index0+2;

        // clip interp. buffer-bound
        if(index_1<0)index_1=MAX_WG_DELAY-1;
        if(index1>=MAX_WG_DELAY)index1=0;
        if(index2>=MAX_WG_DELAY)index2=0;

        // get neighbourgh samples
        float32_t const y_1= buffer [index_1];
        float32_t const y0 = buffer [index0];
        float32_t const y1 = buffer [index1];
        float32_t const y2 = buffer [index2];

        // compute interpolation x
        float32_t const x=(float32_t)back-(float32_t)index0;

        // calculate
        float32_t const c0 = y0;
        float32_t const c1 = 0.5f*(y1-y_1);
        float32_t const c2 = y_1 - 2.5f*y0 + 2.0f*y1 - 0.5f*y2;
        float32_t const c3 = 0.5f*(y2-y_1) + 1.5f*(y0-y1);

        float32_t const output=((c3*x+c2)*x+c1)*x+c0;

        // add to delay buffer
        buffer[counter]=in+output*feedback;

        // increment delay counter
        counter++;

        // clip delay counter
        if(counter>=MAX_WG_DELAY)
            counter=0;

        // return output
        return output;
}

