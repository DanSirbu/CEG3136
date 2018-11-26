/*--------------------------------------------
File: SegDisp.c
Description:  Segment Display Module
---------------------------------------------*/

#include <stdtypes.h>
#include "mc9s12dg256.h"
#include "delay.h"
#include "SegDisp.h"

// Prototypes for internal functions
#define NUM_7_SEG_DISPLAYS 4
//Convert character to 7 segment display version
#define CHAR_TO_NUM(a) (a - 0x30)

#define SEG_UPDATE_INTERVAL (ONETENTH_MS * 10) // Every 1 ms

static unsigned char dispChars[NUM_7_SEG_DISPLAYS];
static unsigned display_decimal_bitmask = 0x00;

static unsigned char NUM_TO_7_SEG_TBL[10] = { 0x3F, 0x6, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67 }; //TODO
volatile int i = 0;

/*---------------------------------------------
Function: initDisp
Description: initializes hardware for the 
             7-segment displays.
-----------------------------------------------*/
void initDisp(void) 
{
	// Complete this function
    //Pg. 24 on dragon 12 manual
    //PORT B connects to the Anode
    //PB0-PB7
    //PP0-PP3 controls common cathode
    DDRB = 0xFF;
    DDRP = 0xFF;
    DDRJ = 0xFF;
    clearDisp();

    TIOS_IOS1 = 1; // set TC1 to output-compare
	TIE_C1I = 1; // enable interrupt channel 1
	TC1 = TCNT + SEG_UPDATE_INTERVAL; //Set for every 10 ms
}

/*---------------------------------------------
Function: clearDisp
Description: Clears all displays.
-----------------------------------------------*/
void clearDisp(void) 
{
    dispChars[0] = ' ';
    dispChars[1] = ' ';
    dispChars[2] = ' ';
    dispChars[3] = ' ';
}


/*---------------------------------------------
Function: setCharDisplay
Description: Receives an ASCII character (ch)
             and translates
             it to the corresponding code to 
             display on 7-segment display.  Code
             is stored in appropriate element of
             codes for identified display (dispNum).
-----------------------------------------------*/
void setCharDisplay(char ch, byte dispNum) 
{
	// Complete this function
    dispChars[dispNum] = ch;
}

void interrupt VectorNumber_Vtimch1 timer1_isr(void) {
   PTP = 0xF0 | ~(1 << i);
      if(dispChars[i] == ' ') {
              PORTB = 0x00;
      } else if (dispChars[i] == 'A') {
              PORTB = 0x77;
      } else {
        PORTB = NUM_TO_7_SEG_TBL[CHAR_TO_NUM(dispChars[i])];
      }
      //Enable dot in display by setting bit 7
      if(display_decimal_bitmask & 1 << i) {
        PORTB |= 1 << 7;
      }

      i++;
      i %= 4;
    TC1 = (TC1 + SEG_UPDATE_INTERVAL); //Update interrupt time
}

/*
 * Enables the dot at that position
*/
void turnOnDP(int pos) 
{
    display_decimal_bitmask |= 1 << pos;
}

/*
 * Enables the dot at that position
*/
void turnOffDP(int pos)
{
    display_decimal_bitmask &= ~(1 << pos);
}