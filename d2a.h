/* d2a.h
**
** Functions to handle the digital to analog conversion.
** It is assumed that the D2A PMOD is connected to port B pins
** 0 to 3 (i.e. connector JD, upper row). NOTE THAT THE JTAG-SPI
** PROGRAMMER MUST BE DISCONNECTED FOR THE D2A TO WORK.
**
** The d2a_configure() function must be called before the
** D2A can be used.
** Output can be sent to the digital to analog converter
** using the d2a_output() function - which accepts an 
** unsigned 8 bit integer (i.e. in the range of 0 to 255
** inclusive).
*/

#ifndef D2A_H
#define D2A_H

void setup_d2a(void);
void d2a_output(uint8_t data);

#endif
