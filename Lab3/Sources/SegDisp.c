/*--------------------------------------------
File: SegDisp.c
Description:  Segment Display Module
---------------------------------------------*/

#include <stdtypes.h>
#include "mc9s12dg256.h"
#include "SegDisp.h"
#include "Delay_asm.h"

// Prototypes for internal functions
#define NUM_7_SEG_DISPLAYS 4
//Convert character to 7 segment display version
#define CHAR_TO_NUM(a) (a - 0x30)

static unsigned char dispChars[NUM_7_SEG_DISPLAYS];
static unsigned char NUM_TO_7_SEG_TBL[10] = { 0x3F, 0x6, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67 }; //TODO
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

}

/*---------------------------------------------
Function: clearDisp
Description: Clears all displays.
-----------------------------------------------*/
void clearDisp(void) 
{
	// Complete this function
    PTP = 0x0F;
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

/*---------------------------------------------
Function: segDisp
Description: Displays the codes in the code display table 
             (contains four character codes) on the 4 displays 
             for a period of 100 milliseconds by displaying 
             the characters on the displays for 5 millisecond 
             periods.
-----------------------------------------------*/
void segDisp(void) 
{
	// Complete this function
	int x;
    for(x = 0; x < 20; x++) {
        int i;
        for(i=0; i<4; i++) {
            PTP = 0xF0 | ~(1 << i);
            PORTB = NUM_TO_7_SEG_TBL[CHAR_TO_NUM(dispChars[i])];
            delayms_c(5);
        }
    }
}
