/* segment.h
 **
 ** Handles operations for the 7-segment display.
 ** Matches note index to a segment value, and
 ** Outputs to a PMOD on PORTC (Connector JB)
 */

#ifndef SEGMENT_H
#define SEGMENT_H


/* Configure the 7SEG port for In/Out */
void setup_segmentDisplay(void);

/* Matching the note index to 7-segment display port values */
uint8_t noteToSegVal(uint8_t noteIndex, uint8_t isLeftChar, uint8_t isUpOctave);

/* Printing to the display */
void segmentPrint(uint8_t noteIndex, uint8_t isLeftChar, uint8_t isUpOctave);

#endif
