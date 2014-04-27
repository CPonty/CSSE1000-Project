/*
** CSSE1000 Project - Main File
*/

#include <avr/io.h>
#include <avr/interrupt.h>

#include "d2a.h"
#include "timer2.h"
#include "notes.h"
#include "serial.h"
#include "segment.h"
#include "led.h"
#include "playback.h"

int main(void) 
{
	/* Setup timer 2 to generate an interrupt every 1ms
	** for push button checks and other timer actions
	*/
	setup_timer2();
	
	/* Setup the note timer so that we are ready to 
	** generate sounds
	*/
	setup_note_timer();
	
	/* Configure the D2A so we're ready to output 
	** data to the speaker
	*/
	setup_d2a();
	
	/* Configure serial port ready for In/Out
	*/
	setup_serial();
	
	/* Print the splash screen message
	*/
	output_string("\r\nReady 42345493 Chris Ponticello ");
	
	/* Configure the 7SEG port for In/Out
	*/
	setup_segmentDisplay();
	
	/* Configure the onboard LED
	*/
	setup_led();

	//------------------------------------------------------------
	
	/* Enable global interrupts */
	sei();

	for(;;) {
		/* Do nothing - the interrupt handlers take care of it. */
	}
}

