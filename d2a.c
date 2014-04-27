/* d2a.c
**
** Functions to handle the digital to analog conversion.
** It is assumed that the D2A PMOD is connected to port B pins
** 0 to 3 (i.e. connector JD, upper row). NOTE THAT THE JTAG-SPI
** PROGRAMMER MUST BE DISCONNECTED FOR THE D2A TO WORK.
**
** The setup_d2a() function must be called before the
** D2A can be used.
** Output can be sent to the digital to analog converter
** using the d2a_output() function - which accepts an 
** unsigned 8 bit integer (i.e. in the range of 0 to 255
** inclusive).
*/

#include <avr/io.h>

/* Configure the SPI device so that we can output data to
** the Digital-to-analog converter DA2 PMOD.
*/
void setup_d2a(void)
{
	/* Make port B, bit 0 (Sync signal) be an output, along with
	** MOSI and SCK pins (PB2 and PB1) */
	DDRB |= 0x07;
	/* Setup SPCR - SPI Control Register.
	** Enable SPI peripheral, make the AVR the
	** master and set the clock phase appropriately.
	*/
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<CPHA);
	/* Set the double speed bit in SPSR */
	SPSR = (1<<SPI2X);
	/* Take the sync signal (PB0) high */
	PORTB |= 0x01;
}

/* Output data to the Digilent DA2 PMOD - assumed to be 
** connected to port B (bits 0 to 3), i.e. Cerebot connector
** JD (upper row).
** We assume that the SPI peripheral is ready to accept new
** data.
*/
void d2a_output(uint8_t data)
{
	/* Take Sync signal low - i.e. PB0 to 0 */
	PORTB &= 0xFE;
	/* Output first byte (contains 0000 + high nibble of data) */
	SPDR = data >> 4;
	/* Wait for transfer to be complete */
	while((SPSR & 0x80) == 0) {
		/* Wait for SPIF flag in SPSR to be set (see pp 169 of
		** datasheet). The final read of SPSR (when the value is
		** 1 will cause the bit to be set back to 0).
		*/
		;
	}
	/* SPDR register should be ready to accept next data byte.
	** Data is 4 lower bits of value and 0000 - i.e. we
	** don't take advantage of the full 12 bits of range
	** on the D2A.
	*/
	SPDR = (data << 4);
	/* Wait for transfer to be complete */
	while((SPSR & 0x80) == 0) {
		;
	}
	/* Take the sync signal high again */
	PORTB |= 0x01;
}
