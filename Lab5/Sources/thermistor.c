#include <stdtypes.h>
#include "mc9s12dg256.h"
#include "thermistor.h"


static volatile int raw_temperature;
static volatile int thermister_counter = 0;

#define FIVE_HUNDRED_KHZ_PRESCALER 0b10111
#define MAX_SAMPLE_TIME (0b11 << 5)
#define TEN_BITS_RESOLUTION 0

#define RIGHT_JUSTIFIED 0x80

//2^10 - 1 = 1023 max value

//Min: 0.025 V
//Max 1 V
//5V = 500 degrees

//0-5V = 0-1023
//So each change in the input is 500 / 1023 = +0.48875 degrees

#define THERMISTER_INTERVAL_1_TENTH 75000

void initThermistor(void) {
    int x = 0;
    
    /****THERMISTER INITIALIZATION****/
    
    ATD0CTL2 = 0;
    ATD0CTL3 = 0;
    ATD0CTL4 = 0;

    ATD0CTL2_AFFC = 1; //Enable fast clear, access to result register resets interrupt
    ATD0CTL2_ASCIE = 1; //Enable interrupts
    

    ATD0CTL3_S1C = 1; //Single conversion
    ATD0CTL4 = FIVE_HUNDRED_KHZ_PRESCALER | MAX_SAMPLE_TIME | TEN_BITS_RESOLUTION;

    //Right shift the output
    

    ATD0CTL2_ADPU = 1; //Power up the ADPU


    //Wait 20 microseconds
    //180 instructions for 1 microsecond
    //20 * 180 = 3600
    for(x = 0; x < 3600; x++);

    
    /****INTERRUPT INITIALIZATION****/
    
    TIOS_IOS3 = 1; //Enable output mode
    TC3 = TCNT + THERMISTER_INTERVAL_1_TENTH; //Set for every 10 ms
    TIE_C3I = 1; //Enable interrupt 3
}
 
void interrupt VectorNumber_Vtimch3 tc3_isr(void) {
    thermister_counter++;
    if(thermister_counter >= 10) { //Every 100 ms
        thermister_counter = 0;

        ATD0CTL5 = RIGHT_JUSTIFIED | 5; //Start conversion on Channel 5
    }

    TC3 = TC3 + THERMISTER_INTERVAL_1_TENTH;
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
    return raw_temperature * 4.8876;
}
