/*
 * sequencer.h
 *
 *  Created on: Aug 4, 2012
 *      Author: owen
 */

#ifndef SEQUENCER_H_
#define SEQUENCER_H_

#define SEQ_STOPPED 0
#define SEQ_RECORD_ENABLE 1
#define SEQ_RECORDING 2
#define SEQ_PLAYING 3


uint8_t seq_ready_for_recording(void);


void seq_set_status(uint8_t stat) ;

uint8_t seq_get_status(void);


#endif /* SEQUENCER_H_ */
