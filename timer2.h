/* timer2.h
**
** We set up timer 2 to give us an interrupt
** every 1 millisecond. Tasks that have to occur
** regularly (every ms) can be added to the
** interrupt handler (in timer2.c).
*/

#ifndef TIMER2_H
#define TIMER2_H

void setup_timer2(void);

#endif
