#include <BOARD.h>
#include <xc.h>
#include <stdio.h>
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "sensors.h"
#include "motors.h"
#include "pwm.h"
#include "LED.h"

//#define MAIN_TEST

#ifdef MAIN_TEST 

//commented this out

void main(void) {
    ES_Return_t ErrorType;

    BOARD_Init();
    PWM_Init();
    motors_Init(); 
    sensors_Init();
    LED_Init();
    printf("Starting ES Framework Template\r\n");
    printf("using the 2nd Generation Events & Services Framework\r\n");


    // Your hardware initialization function calls go here

    
    LED_AddBanks(LED_BANK1 | LED_BANK2 | LED_BANK3);
    LED_OnBank(LED_BANK1, 0xF);
    LED_OnBank(LED_BANK2, 0xF);
    LED_OnBank(LED_BANK3, 0xF);


    // now initialize the Events and Services Framework and start it running
    ErrorType = ES_Initialize();
    if (ErrorType == Success) {
        ErrorType = ES_Run();

    }
    //if we got to here, there was an error
    switch (ErrorType) {
        case FailedPointer:
            printf("Failed on NULL pointer");
            break;
        case FailedInit:
            printf("Failed Initialization");
            break;
        default:
            printf("Other Failure: %d", ErrorType);
            break;
    }
    for (;;)
        ;

};

#endif
/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/
