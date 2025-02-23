/*
 * File:   HelloWorldMain.c
 * Author: Vinh
 *
 * Created on April 6, 2024, 12:15 PM
 */


#include "xc.h"
#include <BOARD.h>
#include <stdio.h>
#include <roach.h>

int main(void) {
    BOARD_Init();
    Roach_Init();

    printf("Hello World\n");
    printf("Hello World\n");
    while (1) {
        ;
    }
    // return 0;
}
