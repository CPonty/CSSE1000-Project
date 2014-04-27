/* notes.h
**
** Functions associated with playing musical notes.
*/

#ifndef NOTES_H
#define NOTES_H

/* Globally accessible variables managing the current
** note and waveform
*/
volatile uint8_t waveform;
volatile uint8_t note;
volatile uint8_t tunePlaying;
volatile uint8_t triWaveSteps;
volatile uint8_t octave;

/* Setup the AVR timer that we will use to time our 
** notes. See the interrupt handler in notes.c for
** what happens on each interrupt.
*/
void setup_note_timer(void);

/* Change the current sound wave type
 */
void set_waveform(uint8_t wavetype);

/* Write the timer compare length for the selected note
*/
void start_note(void);

/* Stop the note timer - this will stop all sound 
*/
void quiet(void);

/* Playing twinkle, twinkle little star
 */
void demoTuneStart(void);
void demoTuneStep(void);

#endif
