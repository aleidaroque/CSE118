/* 
 * File:   sensors.c
 * Author: adiazroq
 *
 * Created on May 14, 2024, 5:36 PM
 */

#include "BOARD.h"
#include "IO_Ports.h"
#include "AD.h"
#include "sensors.h"
#include <stdio.h>
#include <xc.h>
#include <stdint.h>
#include "RC_servo.h"

void sensors_Init() {

    /// TRACK WIRE SENSORS -----------------------------------------------------
    AD_AddPins(AD_PORTV3); // Right Track - V3
    AD_AddPins(AD_PORTV4); // Left Track - V4

    // BEACON DETECTOR ---------------------------------------------------------
    AD_AddPins(AD_PORTW8); // Beacon Distance - W8
    PORTW07_TRIS = 1; // Beacon Found - W7

    // TAPE SENSORS ------------------------------------------------------------
    PORTX04_TRIS = 1; // TapeFR - X4
    PORTX05_TRIS = 1; // TapeFL - X5
    PORTX06_TRIS = 1; // TapeRR - X6
    PORTX03_TRIS = 1; // TapeRL - X3
    PORTW05_TRIS = 1; // WallTape - W5
    PORTW06_TRIS = 1; // WallTape2 - W6

    // BUMPER SENSORS ----------------------------------------------------------
    PORTV05_TRIS = 1; // FR - V5
    PORTV06_TRIS = 1; // FL - V6
    PORTV07_TRIS = 1; // RR - V7
    PORTV08_TRIS = 1; // RL - V8
    PORTW03_TRIS = 1; // TR - W3
    PORTW04_TRIS = 1; // TL - W4
}

int trackWireR() {
    AD_ReadADPin(AD_PORTV3);
}

int trackWireL() {
    AD_ReadADPin(AD_PORTV4);
}

int beaconVal() {
    AD_ReadADPin(AD_PORTW8);
}

int beaconFound() {
    return PORTW07_BIT;
}

unsigned char tapeFR() {
    return PORTX04_BIT;
}

unsigned char tapeFL() {
    return PORTX05_BIT;
}

unsigned char tapeRR() {
    return PORTX06_BIT;
}

unsigned char tapeRL() {
    return PORTX03_BIT;
}



// 4-bit value representing all four bumpers in following order: front left,front right, rear left, rear right
unsigned char botReadTape() {
    //printf("\r\n    L: %d, B: %d, R: %d         reading: %d", (((PORTW04_BIT << 2) | (PORTW05_BIT << 1)) | PORTW03_BIT))
    //return (((tapeL()<<2) | (tapeB()<<1)) | tapeR());
    //return (((!PORTW04_BIT << 2) | (!PORTW05_BIT << 1)) | !PORTW03_BIT);
    
    //return (!PORTW04_BIT + ((! PORTW03_BIT) << 1)+((!PORTW06_BIT) << 2)+((!PORTW05_BIT) << 3));
    return ((PORTX05_BIT << 3) | ((PORTX04_BIT << 2) | ((PORTX03_BIT << 1) | (PORTX06_BIT))));
    
}

int frontRightBumper() {
    return PORTV05_BIT;
}

int frontLeftBumper() {
    return PORTV06_BIT;
}

int rearRightBumper() {
    return PORTV07_BIT;
}

int rearLeftBumper() {
    return PORTV08_BIT;
}

int topRightBumper() {
    return PORTW03_BIT;
}

int topLeftBumper() {
    return PORTW04_BIT;
}

// 4-bit value representing all four bumpers in following order: front left,front right, rear left, rear right
unsigned char botReadBumpers() {
    //return (!PORTV06_BIT + ((!PORTV05_BIT) << 1)+((!PORTV08_BIT) << 2)+((!PORTV07_BIT) << 3));
    return ((PORTV06_BIT << 3) | ((PORTV05_BIT << 2) | ((PORTV08_BIT << 1) | (PORTV07_BIT))));
}

// 4-bit value representing two top bumpers in following order: top left, top right
unsigned char botReadTopBumpers() {
    return ((PORTW04_BIT << 1) | (PORTW03_BIT));
}

unsigned char wallTape() {
    return PORTW05_BIT;
}

unsigned char otherWallTape() {
    return PORTW06_BIT; 
}



/// TEST HARNESS ---------------------------------------------------------------
//#define TRACK_WIRES
//#define BEACON
//#define TAPE
//#define BUMPERS


//#define SENSOR_TEST_HARNESS
#ifdef SENSOR_TEST_HARNESS

#define DELAY(x)                      \
    for (int wait = 0; wait <= x; wait++) \
    {                                 \
        asm("nop");                   \
    }

#define A_BIT 18300
#define A_BIT_MORE 36600
#define YET_A_BIT_LONGER (A_BIT_MORE << 2)
#define A_LOT 1890000

int main() {

    BOARD_Init();
    AD_Init();
    sensors_Init();
    RC_Init();

    int trackValR = 0;
    int trackValL = 0;

#ifdef TRACK_WIRES
    while (1) {
        trackValR = trackWireR();
        trackValL = trackWireL();
        printf("\r\nTrack Wire R: %d", trackValR); // max ~700 ish
        printf("\r\nTrack Wire L: %d", trackValL);
        printf("\r\n diff: %d", trackValR - trackValL);
        DELAY(YET_A_BIT_LONGER);
    }

#endif

#ifdef BEACON  
    while (1) {
        printf("\r\n%d", beaconFound());
        DELAY(YET_A_BIT_LONGER);
    }
#endif

#ifdef TAPE  
    while (1) {
        //printf("\r\n%d", tapeFR());
        //printf("\r\n%d", tapeFL());
        //printf("\r\n%d", tapeFR());
        //printf("\r\n%d", tapeFL());
        //printf("\r\n%d", tapeB());
        //printf("\r\n%d", botReadTape());
        DELAY(YET_A_BIT_LONGER);
    }
#endif

#ifdef BUMPERS  
    while (1) {
        //printf("\r\n%d", frontRightBumper());
        //printf("\r\n%d", frontLeftBumper());
        //printf("\r\n%d", rearRightBumper());
        //printf("%d", rearLeftBumper());
        DELAY(YET_A_BIT_LONGER);
    }
#endif

    return 0;
}

#endif

