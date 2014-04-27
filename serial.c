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

#include <avr/io.h>
#include <avr/interrupt.h>
#include "notes.h"
#include "led.h"
#include "playback.h"

/* Global variables */
/* 
 ** Circular buffer to hold outgoing characters. The insert_pos variable
 ** keeps track of the position (0 to BUFFER_SIZE-1) that the next
 ** outgoing character should be written to. bytes_in_buffer keeps
 ** count of the number of characters currently stored in the buffer 
 ** (ranging from 0 to BUFFER_SIZE). This number of bytes immediately
 ** prior to the current insert_pos are the bytes waiting to be output.
 ** If the insert_pos reaches the end of the buffer it will wrap around
 ** to the beginning (assuming those bytes have been output).
 */
#define BUFFER_SIZE 64
volatile char buffer[64];
volatile unsigned char insert_pos = 0;
volatile unsigned char bytes_in_buffer = 0;
volatile unsigned char noteLines = 0;

void setup_serial(void) {
	/* Set the baud rate to 9600 */
	UBRR0H = 0;
	UBRR0L = 51;
	/* This value is from table 84 (page 196) of the datasheet */
	/* NOTE - this is one example of a value split across
	** more than one 8-bit I/O register - however you can NOT
	** just say UBRR0 = 25; - you MUST address each half
	** separately. This is because UBRR0H and UBRR0L are not
	** next to each other in I/O register space.
	*/
	
	/*
	 ** Enable transmission and receiving via UART and also 
	 ** enable the Receive Complete Interrupt and the Data Register
	 ** Empty interrupt. This ensures that we get an interrupt
	 ** when the UART receives a character and when it is ready
	 ** to accept a new character for transmission.
	 ** (See page 190 of the datasheet)
	 */
	UCSR0B = (1<<RXCIE0) | (1<<UDRIE0) | (1<<RXEN0) | (1<<TXEN0);
}


/* output_char
 **
 ** Procedure to output a character (by adding it to the outgoing buffer)
 ** (The characters will get consumed by an interrupt handler (see below).)
 */
void output_char(char c) {
	/* Add the character to the buffer for transmission if there
	 ** is space to do so. We advance the insert_pos to the next
	 ** character position. If this is beyond the end of the buffer
	 ** we wrap around back to the beginning of the buffer */
	/* NOTE: this only gets executed within an interrupt handler
	 ** so we can be guaranteed uninterrupted access to the buffer.
	 */
	if(bytes_in_buffer < BUFFER_SIZE) {
		/* We have room to add this byte */
		buffer[insert_pos++] = c;
		bytes_in_buffer++;
		if(insert_pos == BUFFER_SIZE) {
			/* Wrap around buffer pointer if necessary */
			insert_pos = 0;
		}
	} 
	/* else, we have no room to add the byte - just discard it */
}

/* output_string
 **
 ** Procedure to output a string (by adding it to the outgoing buffer
 ** character by character). We iterate over all characters in the
 ** string. (Remember, strings are null-terminated.)
 */
void output_string(char* str) {
	
	/* Activate the output buffer check bit */
	UCSR0B |= (1<<UDRIE0);
	
	unsigned char i;	/* index into the string */
	for(i=0; str[i] != 0; i++) {
		output_char(str[i]);
	}
}


/* noteStringTimeDetect
 **
 ** Returns a lowercase/uppercase version of the note string
 ** depending on whether we are within 10% of the beat.
 ** As the LED is on for 100ms, this occurs from 0..100ms.
 */
void noteStringTimeDetect(char* str1, char* str2) {
	
	/* Check LED state */
	if ((beatCount>=450) || (beatCount<=50)) {
		/* uppercase if LED on */
		output_string(str1);
		return;
	}
	output_string(str2);

}

/* output_note
 **
 ** Abstraction of output_string matching note index to string.
 ** Also accounts for scrolling every 20 lines.
 ** New note read from the "notes.c" global value
 */
void output_note(void) {
	
	/* output a string matching the note */
	
	if (octave==0) switch (note) {
		case 0: noteStringTimeDetect(" C4"," c4"); break;
		case 1: noteStringTimeDetect(" D4"," d4"); break;
		case 2: noteStringTimeDetect(" E4"," e4"); break;
		case 3: noteStringTimeDetect(" F4"," f4"); break;
		case 4: noteStringTimeDetect(" G4"," g4"); break;
		case 5: noteStringTimeDetect(" A4"," a4"); break;
		case 6: noteStringTimeDetect(" B4"," b4"); break;
		case 7: noteStringTimeDetect(" C5"," c5"); break;
		default: break;
	} else
	if (octave==1) switch (note) {
		case 0: noteStringTimeDetect(" C5"," c5"); break;
		case 1: noteStringTimeDetect(" D5"," d5"); break;
		case 2: noteStringTimeDetect(" E5"," e5"); break;
		case 3: noteStringTimeDetect(" F5"," f5"); break;
		case 4: noteStringTimeDetect(" G5"," g5"); break;
		case 5: noteStringTimeDetect(" A5"," a5"); break;
		case 6: noteStringTimeDetect(" B5"," b5"); break;
		case 7: noteStringTimeDetect(" C6"," c6"); break;
		default: break;
	}

	
	/* Increment note counter & check for newline */
	noteLines++;
	if (noteLines>=20) {
		noteLines = 0;
		output_string("\r\n");
	}
	
}

/*
 * Define the interrupt handler for UART Data Register Empty (i.e. 
 * another character can be taken from our buffer and written out)
 */
ISR(USART0_UDRE_vect)
{
	/* Check if we have data in our buffer */
	if(bytes_in_buffer > 0) {
		/* Yes we do - remove the pending byte and output it
		 ** via the UART. The pending byte (character) is the
		 ** one which is "bytes_in_buffer" characters before the 
		 ** insert_pos (taking into account that we may 
		 ** need to wrap around to the end of the buffer).
		 */
		char c;
		if(insert_pos - bytes_in_buffer < 0) {
			/* Need to wrap around */
			c = buffer[insert_pos - bytes_in_buffer
					   + BUFFER_SIZE];
		} else {
			c = buffer[insert_pos - bytes_in_buffer];
		}
		/* Decrement our count of the number of bytes in the 
		 ** buffer 
		 */
		bytes_in_buffer--;
		
		/* Output the character via the UART */
		UDR0 = c;
	} else {
		/* no data in buffer - deactivate the buffer read interrupt.
		 */		
		UCSR0B ^= (1<<UDRIE0);
	}
	
}


/*
 * Define the interrupt handler for UART Receive Complete - i.e. a new
 * character has arrived in the UART Data Register (UDR).
 */
 /* This name comes from the AVR libC documentation - see avr/interrupt.h */
ISR(USART0_RX_vect)
{
	/* A character has been received - we will read it. If
	** it corresponds to a key command we will execute that action.
	*/
	char input;

	/* Extract character from UART Data register and place in input
	** variable
	*/
	input = UDR0;

	/* Convert character to upper case if it is lower case */
	if(input >= 'a' && input <= 'z') {
		/* Subtract 32 to change lower case character ASCII code
		** into upper case. */
		input -= 32;	
	}
	
	/* 'T' handler: toggle triangle waveform */
	else if ((input == 'T') && (recording==0)) {
		set_waveform(1);
	}
	
	/* 'S' handler: toggle sine waveform */
	else if ((input == 'S') && (recording==0)) {
		set_waveform(2);
	}	
	
	/* 'D' handler: demo tune */
	else if ((input == 'D') && (tuneWait==0)) {
		demoTuneStart();
		output_string("\r\n-DemoTune- ");
	}
	
	/* '<' handler: dec triangular waveform */
	else if ((input == '<') && (triWaveSteps>4)) {
		triWaveSteps--;
				//output_string(" Triangle_Wave_Steps_dec");
	}
	
	/* '>' handler: inc triangular waveform */
	else if ((input == '>') && (triWaveSteps<16)) {
		triWaveSteps++;
				//output_string(" Triangle_Wave_Steps_inc");
	}
	
	/* 'U' handler: toggle double wavelength */
	else if ((input=='U') && (recording==0)) {
		octave ^= 1;
		output_string("\r\n-OctaveToggle- ");
	}
	
	/* 'R' handler: toggle recording if available */
	else if ((input=='R') && (tuneWait==255)) {
		if (recording) {
			record_stop();
			output_string(" -RecordingStop-");
		} else {
			record_start();
			output_string(" -RecordingStart-");
		}
	}
	
	/* 'P' handler: play recording if available */
	else if ((input=='P')) {
		if ((recording==0) && (tuneWait==255) && (bytes_in_notebuffer>0)) {
			playBuffer();
			output_string(" -playbackStart-");
		}
		
	}
	
}