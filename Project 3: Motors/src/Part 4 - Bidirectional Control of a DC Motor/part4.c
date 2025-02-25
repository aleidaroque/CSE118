/* 
 * File:   part4.c
 * Author: adiazroq
 *
 * Created on April 29, 2024, 7:31 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "BOARD.h"
#include "LED.h"
#include "AD.h"
#include <xc.h>
#include <serial.h>
#include "pwm.h"
#include "IO_Ports.h"

#ifdef DEBUG_VERBOSE
#include <stdio.h>
#define dbprintf(...) printf(__VA_ARGS__)
#else
#define dbprintf(...)
#endif

#define NUMLEDBANKS 3
#define NUMLEDSPERBANK 4
#define BANK2OFFSET 4
#define BANK3OFFSET 8
#define FULLBANK 0x0F
#define ALL_LED_BANKS (LED_BANK1|LED_BANK2|LED_BANK3)

#define LED_SetPinOutput(i) *LED_TRISCLR[i] = LED_bitsMap[i]
#define LED_SetPinInput(i) *LED_TRISSET[i] = LED_bitsMap[i];
#define LED_On(i) *LED_LATCLR[i] = LED_bitsMap[i];
#define LED_Off(i) *LED_LATSET[i] = LED_bitsMap[i];
#define LED_Toggle(i) *LED_LATINV[i] = LED_bitsMap[i];
#define LED_Get(i) (*LED_LAT[i]&LED_bitsMap[i])

#define NOPCOUNT 220000
#define DELAY() for(int i=0; i< NOPCOUNT; i++) __asm("nop")
#define ALPHA 0.09
#define SCALEDIVIDE 1024.0
#define OPEN 0
#define CLOSE 1

/*
 * 
 */
int main(void) {

    BOARD_Init();
    LED_Init();
    PWM_Init();
    AD_Init();

    LED_AddBanks(LED_BANK1 | LED_BANK2 | LED_BANK3);
    AD_AddPins(AD_PORTV3);
    AD_AddPins(AD_PORTV5);
    PWM_AddPins(PWM_PORTZ06);
    PWM_SetFrequency(PWM_DEFAULT_FREQUENCY);

    IO_PortsSetPortDirection(PORTY, 0);

    int prevPot = 0;
    int currPot = 0;
    int reading = 0;
    int LED = 0;
    int switchval = 0;
    int connection = 0;
    unsigned char pattern;
    unsigned char speedPattern;
    unsigned char inversePattern;
    unsigned char invspeedPattern;
    unsigned int Duty;
    unsigned short int pulse = 0;

    LED_OffBank(LED_BANK1, 0x0F);
    LED_OffBank(LED_BANK2, 0x0F);
    LED_OffBank(LED_BANK3, 0x0F);

    while (1) {

        // EMA filtering
        currPot = AD_ReadADPin(AD_PORTV5);
        reading = ((ALPHA) * currPot) + ((1 - ALPHA) * prevPot);
        prevPot = reading;


        // Scaling POT reading to 12 LEDs    
        LED = (reading / SCALEDIVIDE) * 13.0;
        // printf("LED value: %d \r\n", LED);

        // Scaling LED to 4 values
        pattern = (LED - 1) % 4 + 1;
        speedPattern = (LED / 3);

        // If a new value is detected make the value 1-4
        if (currPot != prevPot) {
            pattern = (1 << pattern) - 1;
            speedPattern = (1 << speedPattern) - 1;
        }

        // inverting pattern for decreasing POT values
        inversePattern = pattern ^ 0x0F;
        invspeedPattern = speedPattern ^ 0x0F;
        /*
        //        if (LED == 0) {
        //            LED_OffBank(LED_BANK1, 0x0F);
        //            LED_OffBank(LED_BANK2, 0x0F);
        //            LED_OffBank(LED_BANK3, 0x0F);
        //        }
        //        if (LED <= 4) {
        //            LED_OnBank(LED_BANK1, pattern);
        //
        //            LED_OffBank(LED_BANK1, inversePattern);
        //            LED_OffBank(LED_BANK2, 0x0F);
        //            LED_OffBank(LED_BANK3, 0x0F);
        //        }
        //        if ((LED >= 5) && (LED <= 8)) {
        //            LED_OnBank(LED_BANK2, pattern);
        //
        //            LED_OffBank(LED_BANK2, inversePattern);
        //            LED_OffBank(LED_BANK3, 0x0F);
        //        }
        //        if ((LED >= 9) && (LED <= 12)) {
        //            LED_OnBank(LED_BANK3, pattern);
        //            LED_OffBank(LED_BANK3, inversePattern);
        //        }
         */
        // Motor and DS36

        Duty = (reading / SCALEDIVIDE) * 1000.0;
        PWM_SetDutyCycle(PWM_PORTZ06, Duty);
        // printf("Reading Value: %d \r\n", reading);
        // printf("Duty Value: %d \r\n", Duty);
        // printf("\r\n");

        // External LED switch
        switchval = AD_ReadADPin(AD_PORTV3);
        if (switchval > 0) {
            connection = CLOSE;
        } else {
            connection = OPEN;
        }
        // printf("Switch Value: %d \r\n", connection);

        if (connection) {
            LED_OnBank(LED_BANK1, speedPattern);
            LED_OffBank(LED_BANK1, invspeedPattern);
            LED_OffBank(LED_BANK2, 0x0F);
            LED_OffBank(LED_BANK3, 0x0F);
            IO_PortsSetPortBits(PORTY, PIN10);
            IO_PortsClearPortBits(PORTY, PIN12);
        } else {
            LED_OnBank(LED_BANK3, speedPattern);
            LED_OffBank(LED_BANK3, invspeedPattern);
            LED_OffBank((LED_BANK1 | LED_BANK2), 0x0F);
            IO_PortsSetPortBits(PORTY, PIN12);
            IO_PortsClearPortBits(PORTY, PIN10);
        }

    }

    return (0);
}

