/* serial2.h
**
** Functions to handle serial port communications:
** setup, send, receive, parse.
** It is assumed that the RS232 PMOD is connected to
** Connector JC top, i.e. PORTE 0..3
**
** setup_serial() must be called before serial communication
** can take place.
*/

#ifndef SERIAL_H
#define SERIAL_H

#define BUFFER_SIZE 64
volatile char buffer[BUFFER_SIZE];
volatile unsigned char insert_pos;
volatile unsigned char bytes_in_buffer;
volatile unsigned char noteLines;

/* Enable serial send/rcv */
void setup_serial(void);

/* Add string to outgoing buffer */
void output_string(char* str);

/* Abstraction to provide output_string text corresonding
 ** to the current note.
 */
void output_note(void);

#endif
