/* playback.c
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "led.h"
#include "notes.h"

/* Global variables for the buffers */
#define NOTE_BUFSIZE 24
volatile uint8_t note_buffer[24] = {111};
volatile uint8_t bytes_in_notebuffer = 0;
volatile uint8_t time_buffer[24];
	//time buffers count with 10ms groupings

/* Global variables for playback */
volatile uint8_t playback_counter = 0;
	//counts time between notes
volatile uint8_t playback_noteSpace = 0;
	//stores the current note's length
volatile uint8_t tuneWait = 255;
	//0=play,1=waitForBeat,255=idle

/* Global variables for note recording */
volatile uint8_t rec_waveform = 0;
volatile uint16_t rec_timecounter = 0;
volatile uint16_t rec_beatset = 0;
volatile uint8_t recording = 0;
volatile uint8_t rec_octave = 0;
volatile uint8_t tmp_octave = 0;
volatile uint8_t tmp_waveform = 0;

/* Playback */
void playBuffer(void) {
	
	/* Set state */
	playback_counter = 0;
	playback_noteSpace = time_buffer[0];
	tuneWait = 1;
	
	tmp_octave = octave;
	octave = rec_octave;
	
	tmp_waveform = waveform;
	waveform = rec_waveform;
	
}


void buffer_note(uint8_t n, uint8_t t) {
	/* Procedure to output a note/time (adding it to the outgoing buffers)
	 ** (The values will get consumed one at a time on the 1ms timer)
	 */
	
	if(bytes_in_notebuffer < NOTE_BUFSIZE) {
		/* We have room to add this byte */
		note_buffer[bytes_in_notebuffer] = n;
		time_buffer[bytes_in_notebuffer] = t;
		bytes_in_notebuffer++;
	} 
}


void buffer_song(uint8_t* note_array, uint8_t* time_array) {
	/* Procedure to output a song (by adding it to the outgoing buffer
	 ** note by note). We iterate over all notes in the
	 ** array. IMPORTANT: (Terminates with 111)
	 */
	uint8_t i;
	
	/* Write to buffer */
	for(i=0; note_array[i] != 111; i++) {
		buffer_note(note_array[i], time_array[i]);
	}
	
	/* Playback */
	playBuffer();
	
}


void record_start(void) {
	/* Procedure to refresh the recorded data
	 ** and activate recording of button presses
	 */
	notebuffer_clear();
	rec_waveform = waveform;
	rec_beatset = beatCount;
	rec_octave = octave;
	recording = 1;
}

void record_stop(void) {
	/* Switch off note recording*/
	recording = 0;
	//buffer_note(111,1);
}

void record_note(uint8_t n, uint8_t t) {
	/* Attempt to write the given values to the recording
	** buffer. End recording if buffer full.
	*/
	buffer_note(n,t);
	if (bytes_in_notebuffer == NOTE_BUFSIZE) {
		record_stop();
	}
}



/* Play the demo tune  (to be run on LED activation)
 **
 ** Set a global variable to prevent other actions while the demo
 ** tune plays. Loop through a series of notes, leaving a beat
 ** between each and playing on the beat.
 ** 2beats/sec, so 500ms delay
 */
void demoTuneStart(void) {
	
	if (recording==0) {
		/* Note/time buffers */
		const uint8_t twinkleSong[17] = {0,0,4,4,5,5,4,255,3,3,2,2,1,1,0,255,111};
		const uint8_t twinkleTime[17] = {50};
			//C:0 D:1 E:2 F:3 G:4 A:5 B:6 C:7 xx:255 EoF:111
		
		/* Settings */
		rec_beatset = 0;
		rec_waveform = waveform;
		rec_octave = 0;
	
		/* Buffer write */
		buffer_song(twinkleSong,twinkleTime);
	}

}



/* Define the timer handler for playing notes from the buffer
*/
void playbackStep(void) {
	
	/* 0. Check recording */
	if ((recording==1) && (tuneWait==255)) {
		rec_timecounter++;
		return;
	}
	
	/* 1. Time start of playback to beat LED */
	if ((tuneWait==1) && (beatCount==rec_beatset)) {
		tuneWait = 0;
	}
	
	/* 2. Check if we have notes to play,
	** and aren't still waiting for the beat
	*/
	if ((bytes_in_notebuffer > 0) && (tuneWait==0)) {
		
		/* Check note timing match */
		if (playback_counter++ == playback_noteSpace) {

			uint8_t n, t;
		
			/* Read from buffers */
			n = note_buffer[bytes_in_notebuffer -1];
			t = time_buffer[bytes_in_notebuffer -1];
			bytes_in_notebuffer--;
			
			/* Play the note */
			note = n;
			pressNote();

			if (bytes_in_notebuffer > 0) {
				/* Prepare for the next note */
				playback_noteSpace = t*10;
				playback_counter = 0;
			} else {
				/* Turn off playback */
				tuneWait = 255;
				octave = tmp_octave;
				waveform = tmp_waveform;
			}

		}

	}
		
}

