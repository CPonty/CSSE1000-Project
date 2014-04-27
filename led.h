/* led.h
**
** Handles operations for the blinking onboard LD0
** on PORTE, bit4, synchronized to a 120bpmbeat.
** Sets up port values and toggles LED for 20% of the
** millisecond-stepped interval.
*/

#ifndef LED_H
#define LED_H

/* LED State */
volatile int beatCount;
volatile uint8_t ledOn;

/* Configure the LED port for In/Out */
void setup_led(void);

/* Incrementing the on/off timer */
void beatStep(void);

/* Writing to the port */
void ledWrite(uint8_t on);

#endif
