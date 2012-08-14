/*
 * sequencer.c
 *
 *  Created on: Aug 4, 2012
 *      Author: owen
 */
#include "arm_math.h"
#include "sequencer.h"

#define SEQ_NOTE_STOP 2   // these should be standard midi
#define SEQ_NOTE_START 1


uint32_t sequencer_record_enable = 0;
uint32_t seq_deltas[255];
uint32_t seq_notes[255];
uint32_t seq_events[255];
uint32_t seq_index = 0;
uint32_t seq_time = 0;
uint32_t seq_recording = 0;
uint32_t seq_playing = 0;
uint32_t seq_length = 0;
uint32_t seq_last_note_start = 0;
uint32_t seq_last_note_start_index = 0;
uint8_t seq_status = SEQ_STOPPED;


uint8_t seq_ready_for_recording(void){
	if ((seq_status == SEQ_STOPPED) || (seq_status == SEQ_PLAYING))
		return 1;
	else
		return 0;
}

void seq_set_status(uint8_t stat) {
	seq_status = stat;
}

uint8_t seq_get_status(void) {
	return seq_status;
}
