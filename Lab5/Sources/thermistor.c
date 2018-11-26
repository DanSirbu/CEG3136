#include <stdtypes.h>
#include "mc9s12dg256.h"
#include "thermistor.h"


static volatile int raw_temperature;
static volatile int thermister_counter = 0;
//-20 to 120 degrees = 0 to 5 volts

//Turn led on at 27 degrees

//We want 10-bit precision

//Vout of LM45 is connected to PAD05
//750 * 1.3333 microseconds = 1 millisecond
//7500 = 10 milliseconds

//2^16 - 1 = 65 535, max value we can set the comparator to


#define THERMISTER_INTERVAL_1_TENTH 7500

void initThermistor(void) {
    
    /****THERMISTER INITIALIZATION****/
    ATD0CTL2 = 0;
    ATD0CTL3 = 0;
    ATD0CTL4 = 0;

    ATD0CTL2_AFFC = 1; //Enable fast clear, access to result register resets interrupt
     //Enable interrupts

    ATD0CTL3 = //Single conversion, no FIFO
    ATD0CTL4 = //500khz, 10 bits resolution

    //Right shift the output

    ATD0CTL2_ADPU = 1; //Power up the ADPU
    //TODO wait 20 microseconds

    /****INTERRUPT INITIALIZATION****/
    TIOS_IOS6 = 1; //Enable output mode
    TC6 = TCNT + THERMISTER_INTERVAL; //Set for every 10 ms
    TIE_C6I = 1; //Enable interrupt 6
}

void interrupt VectorNumber_Vtimch6 thermister_isr(void) {
    thermister_counter++;
    if(thermister_counter >= 10) { //Every 100 ms
        thermister_counter = 0;

        ATD0CTL5 = //Start conversion on PIN5
        TFLG1 //Clear flag and CH3
    }

    TC6 = TCNT + THERMISTER_INTERVAL_1_TENTH;
}

//Happens when conversion is complete
void interrupt VectorNumber_Vatd0 atd_isr(void) {
    raw_temperature = ATD0DR0;
}

/*
 * Returns the temperature in tenth of degree
 * Ex. 27 degrees = 270
*/
int getTemp(void) {
    return raw_temperature;
}
