/*
 * File:   main.c
 * Author: Vinh
 *
 * Created on April 6, 2024, 2:46 PM
 */


#include "xc.h"
#include <BOARD.h>
#include <stdio.h>
#include <roach.h>

#define FLEFT_BUMP_MASK (1)
#define FRIGHT_BUMP_MASK (1<<1)
#define RLEFT_BUMP_MASK (1<<2)
#define RRIGHT_BUMP_MASK (1<<3)

int main(void) {
    BOARD_Init();
    Roach_Init();
    
    int motorSpeed = 100;
    int lightLevel = 0;

    while (1) {
        // read roach bumpers/motor control
        switch (Roach_ReadBumpers()) {
            case FLEFT_BUMP_MASK:
                printf("Front Left!\r\n");
                Roach_LeftMtrSpeed(motorSpeed);
                break;
                
            case FRIGHT_BUMP_MASK:
                printf("Front Right!\r\n");
                Roach_RightMtrSpeed(motorSpeed);
                break;
                
            case RLEFT_BUMP_MASK:
                printf("Rear Left!\r\n");
                Roach_LeftMtrSpeed(-motorSpeed);
                break;
                
            case RRIGHT_BUMP_MASK:
                printf("Rear Right!\r\n");
                Roach_RightMtrSpeed(-motorSpeed);
                break;
                
            default:
                Roach_LeftMtrSpeed(0);
                Roach_RightMtrSpeed(0);
                break;
        }
        
        // light level to LED bar
        lightLevel = Roach_LightLevel();
        Roach_BarGraph(lightLevel * 12/1023);
        motorSpeed = lightLevel * 100/1023 + 20;
    }
    // return 0;
}
