/* playback.h
**
** Functions associated with recording
** buffering and playing stored data
** about notes, timing and waveform
*/

#ifndef PLAYBACK_H
#define PLAYBACK_H

/* Global variables for the buffers */
volatile uint8_t note_buffer[24];
volatile uint8_t bytes_in_notebuffer;
volatile uint8_t time_buffer[24];
//value of 1 = a 10ms step

/* Global variables for playback */
volatile uint8_t playback_counter;
volatile uint8_t playback_noteSpace;
volatile uint8_t tuneWait;

/* Global variables for note recording */
volatile uint8_t rec_waveform;
volatile uint16_t rec_timecounter;
volatile uint16_t rec_beatset;
volatile uint8_t recording;
volatile uint8_t rec_octave;
volatile uint8_t tmp_octave;

/* Public functions */
void playBuffer(void);
	//enable recording playback
void record_start(void);
	//clear buffers, enable note storage
void record_note(uint8_t n, uint8_t t);
	//conditionally write note to buffer
void record_stop(void);
	//disable note storage
void demoTuneStart(void);
	//write demo tune to buffers
void playbackStep(void);
	//timer interrupt handler for playback

#endif
