/*
 * sequencer.c
 *
 *  Created on: Aug 4, 2012
 *      Author: owen
 */
#include "arm_math.h"
#include "pp6.h"
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
uint8_t seq_playback_knobs_enabled = 1;

float32_t knob_log [4096][3];


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

void seq_log_first_note(uint8_t note){
	seq_deltas[0] = 0;
	seq_notes[0] = note;
	seq_events[0] = SEQ_NOTE_START;
	seq_time = 0;
	seq_length = 0;
	seq_index = 0;
	seq_index++;
}

void seq_log_note_start(uint8_t note){
	seq_deltas[seq_index] = seq_time;
	seq_notes[seq_index] = note;
	seq_events[seq_index] = SEQ_NOTE_START;
	seq_last_note_start = seq_time;
	seq_last_note_start_index = seq_index;
	seq_index++;
}

void seq_log_note_stop(uint8_t note){
	seq_deltas[seq_index] = seq_time;
	seq_notes[seq_index] = note;
	seq_events[seq_index] = SEQ_NOTE_STOP ;
	seq_index++;
}

void seq_stop_recording(void) {
	seq_deltas[seq_index] = seq_time;
	seq_notes[seq_index] = seq_notes[0];
	seq_events[seq_index] = SEQ_NOTE_STOP; // just make it a note stop
	seq_length = seq_index;
	seq_index = 0; // go back to the begining
	seq_time = 0;
	seq_playback_knobs_enabled = 1;  // enable playback of knobs
	pp6_set_note_stop();  // always stop the note
}

void seq_rewind(void) {
	seq_index = 0; // go back to the begining
	seq_time = 0;
}


// TODO don't call pp6 functions from here !!!
void seq_play_tick (void){

	if (seq_time >= seq_deltas[seq_index]){
		if (seq_events[seq_index] == SEQ_NOTE_START){
			pp6_set_note(seq_notes[seq_index]);
			pp6_set_note_start();
		}
		if (seq_events[seq_index] == SEQ_NOTE_STOP){
			pp6_set_note_stop();
		}
		seq_index++;
		if (seq_index > seq_length){
			seq_index = 0;
			seq_time = 0;
		}
	}

}

void seq_tick(void){
	seq_time++;
}

uint32_t seq_get_time(void){
	return seq_time;
}

uint32_t seq_get_length(void) {
	return seq_length;
}


/// todo :  these should only be called every 16 ticks  (don't store multiples as you are doing below)
void seq_log_knobs(float32_t * knob){
	uint32_t knob_log_time;

	knob_log_time = (seq_time >> 4) & 0xFFF;

	knob_log[knob_log_time][0] = knob[0];
	knob_log[knob_log_time][1] = knob[1];
	knob_log[knob_log_time][2] = knob[2];
}

float32_t * seq_play_knobs(void) {
	uint32_t knob_log_time;

	knob_log_time = (seq_time >> 4) & 0xFFF;

	return &knob_log[knob_log_time][0];
}

uint8_t seq_knob_playback_enabled(void){
	return seq_playback_knobs_enabled;
}

void seq_disable_knob_playback(void){
	seq_playback_knobs_enabled = 0;
}

void seq_enable_knob_playback(void){
	seq_playback_knobs_enabled = 1;
}
