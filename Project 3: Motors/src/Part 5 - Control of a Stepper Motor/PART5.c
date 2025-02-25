/* 
 * File:   PART5.c
 * Author: adiazroq
 *
 * Created on May 1, 2024, 12:21 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "Board.h"
#include <Stepper.h>

#include <xc.h>
#include <sys/attribs.h>

/*
 * 
 */

int main(void) {

    uint16_t setRate = 600; // 707 is bad, 705 is good
                        // half step: 1375 bad
                        // wave: 681 bad
    
    int32_t i, numSteps = 400; // 100 - Full & Wave, 200 - Half

#define NOPCOUNT 1500000
#define DELAY() for(i=0; i< NOPCOUNT; i++) __asm("nop")

    BOARD_Init();



    Stepper_Init();
    Stepper_SetRate(setRate);

    printf("\n\r\n\rNow setting stepper to turn forwards and backwards %d steps at %d Hz", numSteps, Stepper_GetRate());
    Stepper_SetSteps(FORWARD, numSteps);
    Stepper_StartSteps();

    DELAY();

    Stepper_SetSteps(REVERSE, numSteps);
    Stepper_StartSteps();

    printf("\r\nStepping Forwards");

    return 0;
}

