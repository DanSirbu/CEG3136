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

//2^10 - 1 = 1023 max value

//Min: 0.025 V
//Max 1 V

//5V = 500 degrees

//0-5V = 0-1023

//1023 / 500 = 2.2 seomething

//Value read / 2.2something = degrees

//Multiply by 0.4887585532746823

// 1 / (1023 / 500)

//trigger voltage 0.27


#define THERMISTER_INTERVAL_1_TENTH 75000

void initThermistor(void) {
    int x = 0;
    
    /****THERMISTER INITIALIZATION****/
    
    ATD0CTL2 = 0;
    ATD0CTL3 = 0;
    ATD0CTL4 = 0;

    ATD0CTL2_AFFC = 1; //Enable fast clear, access to result register resets interrupt
    ATD0CTL2_ASCIE = 1;//Enable interrupts
    

    ATD0CTL3_S1C = 1; //Single conversion
    ATD0CTL4 = 0b00010111; //500khz, 10 bits resolution

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

        ATD0CTL5 = 5|0x80; //Start conversion on Channel 5
    }

    TC3 = TCNT + THERMISTER_INTERVAL_1_TENTH;
    TFLG1 &= 0b11110111; //Clear interrupt      
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
    return raw_temperature * 4.8875855327468230694037145650049;
}
