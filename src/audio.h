/*
 * audio.h
 *
 *  Created on: Apr 1, 2012
 *      Author: owen
 */

#ifndef AUDIO_H_
#define AUDIO_H_

//#define BASE_FREQ 32.70f   // C1
#define BASE_FREQ 65.40f   // C1

#define SR 22520.0f
//#define SR 11025.0f
//#define KR 1378.125f // sr / 16
#define KR 22520.0f // sr / 16
#define TWO_PI 6.283185307f

float32_t c_to_f(float32_t c);

#endif /* AUDIO_H_ */
