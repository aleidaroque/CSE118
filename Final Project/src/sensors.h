/* 
 * File:   sensors.h
 * Author: adiazroq
 *
 * Created on May 14, 2024, 5:36 PM
 */

#ifndef SENSORS_H
#define	SENSORS_H


void sensors_Init();

// analog
int trackWireR();
// analog
int trackWireL();


// analog
int beaconVal();

// digital
int beaconFound();

// digital
unsigned char tapeFR();

// digital
unsigned char tapeFL();

// digital
unsigned char tapeRR();

// digital
unsigned char tapeRL();

// digital
unsigned char tapeB();

unsigned char botReadTape();

// digital
int frontRightBumper();


// digital
int frontLeftBumper();


// digital
int rearRightBumper();


// digital
int rearLeftBumper();

int topRightBumper();

int topLeftBumper();

unsigned char botReadBumpers();

unsigned char botReadTopBumpers();

unsigned char wallTape();

unsigned char otherWallTape();





#endif	/* SENSORS_H */

