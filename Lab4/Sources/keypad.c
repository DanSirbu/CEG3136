#include <stdtypes.h>
#include "mc9s12dg256.h"
#include "keyPad.h"
#include "delay.h"


typedef struct {
    char keycode;
    char value;
} keycode_key_pair_t;

#define NUM_KEYS 16
keycode_key_pair_t convertTbl[NUM_KEYS] = {
    { 0b11101110,'1' },
    { 0b11101101,'2' },
    { 0b11101011,'3' },
    { 0b11100111,'a' },
    { 0b11011110,'4' },
    { 0b11011101,'5' },
    { 0b11011011,'6' },
    { 0b11010111,'b' },
    { 0b10111110,'7' },
    { 0b10111101,'8' },
    { 0b10111011,'9' },
    { 0b10110111,'c' },
    { 0b01111110,'*' },
    { 0b01111101,'0' },
    { 0b01111011,'#' },
    { 0b01110111,'d' }
};

static char key_debounced = 0;
static char key_release_debounced = 0;

static char key_wait_release = 0;

static char volatile key_pressed;

#define DEBOUNCE_TIME (ONETENTH_MS * 10)

void initKeyPad(void) {
    DDRA = 0b11110000; // Data Direction Register
	PUCR |= 0b00000001; // Enable pullups

    TIOS_IOS2 = 1; // set to output-compare
	TC2 = TCNT + DEBOUNCE_TIME; //Set for every 10 ms
    TIE_C2I = 1; // enable interrupt channel
}

void interrupt VectorNumber_Vtimch2 timer2_isr(void) {
    PORTA = 0x0F;
    char keyPressed = PORTA != 0x0F;

    if(keyPressed) {
        if(!key_debounced) { //Wait for 1 cycle to debounce key
            key_debounced = 1;
        } else {
            key_wait_release = 1;
        }
        key_release_debounced = 0;
    } else {
        //Key not pressed
        if(key_wait_release && key_release_debounced) {
            key_wait_release = 0;

            int x;
            for(int x = 0; x < NUM_KEYS; x++) {
                PORTA = convertTbl[x].keycode;

                //it means that key is pressed
                if(PORTA == convertTbl[x].keycode) {
                    key_pressed = convertTbl[x].value;
                    break;
                }
            }
        } else {
            key_release_debounced = 1;
        }
        key_debounced = 0;
    }

    TC2 = TCNT + DEBOUNCE_TIME; //Update interrupt time
}

char pollReadKey(void) {
    if(key_pressed != NOKEY) {
        char temp = key_pressed;
        key_pressed = NOKEY;
        return temp;
    }
    return NOKEY;
}

char readKey(void) {
    while(key_pressed == NOKEY); //Wait

    char temp = key_pressed;
    key_pressed = NOKEY;
    return temp;
}