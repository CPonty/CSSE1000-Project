/* led.c
**
** Handles operations for the blinking onboard LD0
** on PORTE, bit4, synchronized to a 120bpmbeat.
** Sets up port values and toggles LED for 20% of the
** millisecond-stepped interval.
*/

#include <avr/io.h>
#include <avr/interrupt.h>

void ledWrite(uint8_t on);

/* Remember the current beat cycle/LED state */
volatile int beatCount = 0;
volatile uint8_t ledOn = 0;



/* Configure the LED port for In/Out */
void setup_led(void) {
	
	/* Setup the data direction register for portE (LED) */
	DDRE = 0xFF;
}


/* Incrementing the on/off timer */
void beatStep(void) {
	
	/* Beat Handler
	** This interrupt firing 1000times/sec
	** 60,000times/minute
	** 120bpm = 2bps
	** 500clocks delay
	** but the light is on for 20% of a beat
	** 500*0.2 = 100clocks */
	
	/* Count beats */
	beatCount++;
	if (beatCount==499) {
		beatCount = 0;
	}
	
	/* LED on/off */
	if (beatCount<100) {
		ledWrite(1);
	}
	else {
		ledWrite(0);
	}
}


/* Writing to the port */
void ledWrite(uint8_t on) {
	
	/* Toggle portE, bit4 without changing other bits */
	if (on==1) {
		PORTE |= (1<<4);
		ledOn = 1;
	}
	else {
		PORTE &= 0xEF; //andi 11101111=239, bit mask
		ledOn = 0;
	}

}
