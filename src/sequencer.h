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

void seq_log_first_note(uint8_t note);

void seq_log_note_start(uint8_t note);
void seq_log_note_stop(uint8_t note);

void seq_stop_recording(void) ;
void seq_rewind(void) ;

void seq_play_tick (void);
void seq_tick(void);

uint32_t seq_get_time(void);

uint32_t seq_get_length(void);

void seq_log_knobs(float32_t * knob);
float32_t * seq_play_knobs(void) ;

uint8_t seq_knob_playback_enabled(void);
void seq_disable_knob_playback(void);
void seq_enable_knob_playback(void);

#endif /* SEQUENCER_H_ */
