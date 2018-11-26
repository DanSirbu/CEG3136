#include <stdtypes.h>
#include "mc9s12dg256.h"
#include "thermistor.h"


static volatile int raw_temperature;
//-20 to 120 degrees = 0 to 5 volts

//Turn led on at 27 degrees

//We want 10-bit precision

//Vout of LM45 is connected to PAD05
#define THERMISTER_INTERVAL 750

void initThermistor(void) {
    TIOS_IOS6 = 1; //Enable output mode
    TIE_C6I = 1; //Enable interrupt 6
    TC6 = TCNT + THERMISTER_INTERVAL; //Set for every 10 ms
}
//Every 100 ms
void interrupt VectorNumber_Vtimch6 thermister_isr(void) {
    //TODO read ADC
    
    TC6 = TCNT + THERMISTER_INTERVAL;
}

/*
 * Returns the temperature in tenth of degree
 * Ex. 27 degrees = 270
*/
int getTemp(void) {
    return raw_temperature;
}
