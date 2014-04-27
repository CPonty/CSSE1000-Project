/* segment.c
**
** Handles operations for the 7-segment display.
** Matches note index to a segment value, and
** Outputs to a PMOD on PORTC (Connector JB)
*/

#include <avr/io.h>
#include <avr/interrupt.h>


void setup_segmentDisplay(void) {
	
	/* Setup the data direction register for PORTC (7SEG) */
	DDRC = 0xFF;
}

uint8_t noteToSegVal(uint8_t noteIndex, uint8_t isLeftChar, uint8_t isUpOctave) {
	
	/* Stored, precalculated values to write
	** to the port for each number/letter
	*/
	const uint8_t segments[11] = {0xB9,0xDE,0xF9,0xF1,0xBD,0xF7,0xFC,0xB9,0x66,0x6D,0x7D};
	//values: CDEFGABC456 = 0x{B9,DE,F9,F1,BD,F7,FC,B9,66,6D,7D}
	
	/* Blank for no note */
	if (noteIndex==255) {
		return 0;
	}
	
	/* CAT=1, left side - alphabetic */
	if (isLeftChar) {
		if (noteIndex<=7) {
			return segments[noteIndex];
		}
	}
	/* CAT=0, right side - numeric */
	else {
		if (noteIndex<7)//octave 4
			return segments[8+isUpOctave];
		if (noteIndex==7)//octave 5
			return segments[9+isUpOctave];

	}
	/* Backup return value: no display */
	return 0;
	
}

void segmentPrint(uint8_t noteIndex, uint8_t isLeftChar, uint8_t isUpOctave) {
	
	/* Print to the port */
	PORTC = noteToSegVal(noteIndex, isLeftChar, isUpOctave);
}