/* 
 * File:   part6.c
 * Author: adiazroq
 *
 * Created on May 1, 2024, 3:17 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "Board.h"
#include <Stepper.h>
#include <xc.h>
#include <sys/attribs.h>

#define NOPCOUNT 200000
#define DELAY() for(i=0; i< NOPCOUNT; i++) __asm("nop")

/*
 * 
 */
int main(void) {

    uint16_t setRate = 1755; // Full: 1730 is bad 0.5 A // 1755 bad 2 A
    // half step: 3764 bad 0.5 A // 3769 bad 2 A


    int32_t i, numSteps = 800; // 100 - Full & Wave, 200 - Half

    BOARD_Init();
    Stepper_Init();
    Stepper_SetRate(setRate);

    //printf("\n\r\n\rNow setting stepper to turn forwards and backwards %d steps at %d Hz", numSteps, Stepper_GetRate());

    Stepper_SetSteps(REVERSE, numSteps);
    Stepper_StartSteps();

//    DELAY();
//
//    Stepper_SetSteps(REVERSE, numSteps);
//    Stepper_StartSteps();

      printf("\r\nStepping Forwards");

    return 0;
}

