/* timer2.c
** We setup timer2 to sample our push button regularly
** and act on that.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "notes.h"
#include "d2a.h"
#include "segment.h"
#include "led.h"
#include "serial.h"
#include "playback.h"

void quiet(void);

/* Remember the last status of the push button
** and 7SEG display
*/
volatile uint8_t prevButtonStatus = 0;
volatile uint8_t cat = 0;

/* Set up timer 2 to generate an interrupt every 1ms. 
** We will divide the clock by 64 and count up to 124.
** We will therefore get an interrupt every 64 x 125
** clock cycles, i.e. every millisecond with an 8MHz
** clock. 
** The counter will be reset to 0 when it reaches it's
** output compare value.
*/
void setup_timer2(void)
{
	/* Set the output compare value to be 124 */
	OCR2 = 124;

	/* Enable an interrupt on output compare match. 
	** Note that interrupts have to be enabled globally
	** before the interrupts will fire.
	*/
	TIMSK |= (1<<OCIE2);

	/* Set the timer to clear on compare match (CTC mode)
	** and to divide the clock by 64. This starts the timer
	** running.
	*/
	TCCR2 = (1<<WGM21)|(0<<WGM20)|(0<<CS22)|(1<<CS21)|(1<<CS20);
}


/*Abstraction of press note button actions */
void pressNote(uint8_t n) {

	/* set current note */
	note = n;
	/* set frequency (notes.h) */
	start_note();
	/* print out note (serial.h) */
	output_note();	
}


/* Abstraction of record note action */
void recNote(uint8_t n) {
	if (recording==1) {
		buffer_note(n, rec_timecounter);
		rec_timecounter = 0;
	}
	
}


ISR(TIMER2_COMP_vect) 
{
	/* Sample push button connected to port A pin 0.
	** (We ignore other push buttons.)
	** If has changed since previous sample, we act on that,
	** i.e. other stop note playing or start note playing
	*/
	uint8_t currentButtonStatus = PINA;
	uint8_t i;
	
	/* Push Buttons
	 ** Ensure the button state has changed,
	 ** and the demo tune is inactive (notes.h) 
	 */
	if ((currentButtonStatus != prevButtonStatus) && (tuneWait == 0)) {
		
		/* Check no buttons held */
		if (currentButtonStatus==0) {	
			/* Disable the speaker */
			quiet();
			/* Set no note being played */
			note = ~0;
		}
		/* A button was released */
		else if (prevButtonStatus > currentButtonStatus) {
			/* Play any held-down button */
			for (i=0; i<=7; i++) {
				if (currentButtonStatus & (1<<i)) {
					pressNote(i);
					recNote(i);
					break;
				}
			}
		}
		/* A button was pressed */
		else if (prevButtonStatus < currentButtonStatus) {
			/* Play the new button */
			for (i=0; i<=7; i++) {
				if ((currentButtonStatus^prevButtonStatus) & (1<<i)) {
					pressNote(i);
					recNote(i);
					break;
				}
			}
		}
	}
	/* Remember the status of the push button*/
	prevButtonStatus = PINA;
	
	/* Print to the 7Seg Display */
	segmentPrint(note,cat,octave);
	cat ^= 1;

	/* Incrememnt the beat timer */
	beatStep();
	
	/* Run the playback tune handler */
	playbackStep();
}
