/*----------------
File: Delay.h
Description: Header file for Delay Module
--------------------*/

// Function Prototypes
void initDelay(void);
void delayms(int);
void setCounter(volatile int *);
#define ONETENTH_MS 75
