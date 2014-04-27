/* notes.c
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include "d2a.h"
#include "led.h"
#include "serial.h"

void quiet(void);

/* Global variables for the waveform generator */
volatile uint8_t waveform = 0;
volatile uint8_t upWave = 1;//for triangle wave
volatile uint8_t sinWaveStep = 0;
volatile float amplitude = 0;
volatile uint8_t note = 255;//no note
volatile uint8_t triWaveSteps = 8;
volatile uint8_t octave = 0;


/* Setup timer 1 to generate an interrupt when output compare 
** match A happens. Global interrupts will have to be 
** enabled also.
*/
void setup_note_timer(void) {
	TIMSK |= (1<<OCIE1A);
}

/* Play a note.
**
** Setup timer 1 to generate an interrupt at twice the frequency
** of the desired note for square waves, and twice*no. of steps
** for triangular waves. We then change the value being sent to 
** the D2A. Timer 1 is configured to count the system clock
** and to reset on output compare match.
*/
void start_note(void) 
{
	uint16_t clockVal = 0;
	const uint16_t noteClocks[] = {15287,13620,12133,11452,10203,9089,8098,7643};
	
	/* Select precalculated clock times matching a note
	** length for the compare register. 
	** period=1/(frequency)
	** clocksteps=8MHz *period
	**/
	//c4 261.30hz	//d4 293.66hz	//e4 329.63hz	//f4 349.23hz
	//g4 392.00hz	//a4 440.00hz	//b4 493.88hz	//c5 523.25hz
	
	if (note<=7) {
		clockVal = noteClocks[note];
	} else {
		quiet();
		return;
	}
	
	/* Allow for waveforms: divide by no. steps needed
	** then subtract 1, as clk starts at 0 */
	if (waveform==1) clockVal = clockVal/(triWaveSteps);
	if (waveform==2) clockVal = clockVal/(16);
	if (octave==1) clockVal = clockVal/2;
		//if (waveform==0) clockVal = clockVal;
	clockVal--;
	
	/* Write to the timer compare register */
	OCR1A = clockVal;
	
	/* Set up timer so that it resets on output compare match
	** and is clocked by the system clock. This turns the timer
	** on - so the interrupt handler will fire when it reaches
	** the output compare value - and sound will be generated.
	*/
	TCCR1B = (1<<WGM12)|(1<<CS10);
}


/* Set the currently used note waveform 
 */
void set_waveform(uint8_t wavetype) {
	//0==square, 1==triangle, 2=sin

	/* Stop the current note */
	quiet();
	
	/* Toggle the waveform */
	if (waveform != wavetype) {
		waveform = wavetype;
	} else {
		waveform = 0;
	}
	
	/* Continue the current note */
	start_note();
}

/* Turn off timer 1 - so no interrupts will fire
** and no sound will be generated. */
void quiet(void)
{
	TCCR1B = 0;
	/* reset amplitude in case of waveform changes before
	** next tone */
	amplitude = 0;
	upWave = 1;
	sinWaveStep = 0;
}


/* Interrupt service routine for our note timer.
*/
ISR(TIMER1_COMPA_vect)
{
	/* Implement wave output. This interrupt
	** handler is called at twice the frequency
	** of the sound being output for square waves,
	** and 32x for triangle/sin waves.
	**
	** Timing is handled by start_note.
	*/
	
	/* Precalculated amplitudes for 32-step sin waves,
	** equivalent to 16 step triangular waves.
	** Precalculation saves processing time.
	*/
	const uint8_t sinAmplitude[32] = {128,176,218,246,255,246,218,176,128,79,37,9,0,9,37,79,127,176,218,246,255,246,218,176,128,79,37,9,0,9,37,79};
	
	/* Square waveform processing */
	if (waveform==0) {
		amplitude = 255 - amplitude;
	}
	
	/* Triangle waveform processing */
	if (waveform==1) {
		
		/* Increase/Decrease amplitude */
		if (upWave==1) {
			amplitude += 256.0/triWaveSteps;
		} else {
			amplitude -= 256.0/triWaveSteps;
		}
		
		/* Check for upper/lower amplitude */
		if (amplitude>254) {
			amplitude = 255;
			upWave = 0;
		}
		if (amplitude<1) {
			amplitude = 0;
			upWave = 1;
		}
	}
	
	/* Sine waveform processing */
	if (waveform==2) {
		
		/* Increase the wave position counter */
		sinWaveStep++;
		if (sinWaveStep>=32) {
			sinWaveStep = 0;
		}
		
		/* Read the amplitude */
		amplitude = sinAmplitude[sinWaveStep];
	}
	
	
	/* Play tone */
	d2a_output((uint8_t)round(amplitude));
}
