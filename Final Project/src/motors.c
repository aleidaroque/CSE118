/*
 * File:   motors.c
 * Author: aleidadiaz-roque
 *
 * Created on May 13, 2024, 9:37 AM
 */

#include "BOARD.h"
#include "IO_Ports.h"
#include "pwm.h"
#include "AD.h"
#include "motors.h"
#include "RC_Servo.h"
#include "serial.h"
#include <stdio.h>
#include <xc.h>

// MOTORS
#define LEFT_MOTOR PWM_PORTZ06
#define RIGHT_MOTOR PWM_PORTY04 
#define WALL_ACT PWM_PORTX11
#define ROLLER_MOTOR PWM_PORTY10
#define SERVO_MOTOR RC_PORTW07

#define SCALE 92/100

// MOTOR LIBRARY
#define FORWARD 1
#define BACKWARD 0

#define LEFT 0
#define RIGHT 100
#define WALL 200
#define ROLLER 300
#define ALL 400

void motors_Init() {

    /// WHEEL H-BRIDGE ---------------------------------------------------------
    // LEFT MOTOR
    PWM_AddPins(LEFT_MOTOR); // ENA - Z6
    PORTZ07_TRIS = 0; // IN1 - Z7
    PORTZ08_TRIS = 0; // IN2 - Z8

    PORTZ07_LAT = 0; // FORWARD
    PORTZ08_LAT = 0; // BACKWARD

    // WALL ACTUATOR
    PORTX08_TRIS = 0; // IN3 - X8
    PORTX10_TRIS = 0; // IN4 - X10

    PORTX08_LAT = 0; // BACKWARD ?
    PORTX10_LAT = 0; // FORWARD ?
    PWM_AddPins(WALL_ACT); // ENB - X11

    /// WALL & ROLLER H-BRIDGE -------------------------------------------------

    // RIGHT MOTOR
    PWM_AddPins(RIGHT_MOTOR); // ENA - Y4
    PORTY03_TRIS = 0; // IN1 - Y3
    PORTY05_TRIS = 0; // IN2 - Y5

    PORTY03_LAT = 0; // FORWARD ?
    PORTY05_LAT = 0; // BACKWARD ?

    // ROLLER MOTOR
    PORTY11_TRIS = 0; // IN3 - Y11
    PORTY12_TRIS = 0; // IN4 - Y12

    PORTY11_LAT = 0; // BACKWARD
    PORTY12_LAT = 0; // FORWARD
    PWM_AddPins(ROLLER_MOTOR); // ENB - Y10

    // SWING WALL RC SERVO
    RC_Init();
    RC_AddPins(RC_PORTW07); // Sig - w7

}

void moveMotor(int motor, int speed) {
    
    if (motor == WALL) {
        if (speed == 0) {
            PORTX08_LAT = 0;
            PORTX10_LAT = 0;
        } else if (speed > 0) {
            PORTX08_LAT = 0; // BACKWARD ?
            PORTX10_LAT = 1; // FORWARD ?
        } else {
            PORTX08_LAT = 1; // BACKWARD ?
            PORTX10_LAT = 0; // FORWARD ?
            speed = speed * -1;
        }
        PWM_SetDutyCycle(WALL_ACT, speed);

    } else if (motor == LEFT) {
        if (speed == 0) {
            PORTZ07_LAT = 0;
            PORTZ08_LAT = 0;
        } else if (speed > 0) {
            PORTZ07_LAT = 1; // FORWARD
            PORTZ08_LAT = 0; // BACKWARD
        } else {
            PORTZ07_LAT = 0; // FORWARD
            PORTZ08_LAT = 1; // BACKWARD
            speed = speed * -1;
        }
        PWM_SetDutyCycle(LEFT_MOTOR, speed);

    } else if (motor == RIGHT) {
        if (speed == 0) {
            PORTY03_LAT = 0;
            PORTY05_LAT = 0;
        } else if (speed > 0) {
            PORTY03_LAT = 1; // FORWARD ?
            PORTY05_LAT = 0; // BACKWARD ?
        } else {
            PORTY03_LAT = 0; // FORWARD ?
            PORTY05_LAT = 1; // BACKWARD ?
            speed = speed * -1;
        }
        PWM_SetDutyCycle(RIGHT_MOTOR, speed * SCALE);

    } else if (motor == ROLLER) {
        if (speed == 0) {
            PORTY11_LAT = 0;
            PORTY12_LAT = 0;
        } else if (speed > 0) {
            PORTY11_LAT = 0; // BACKWARD
            PORTY12_LAT = 1; // FORWARD
        } else {
            PORTY11_LAT = 1; // BACKWARD
            PORTY12_LAT = 0; // FORWARD
            speed = speed * -1;
        }
        PWM_SetDutyCycle(ROLLER_MOTOR, speed);

    } else if (motor == ALL) {
        if (speed == 0) {
            PORTX08_LAT = 0;
            PORTX10_LAT = 0;
            //------------------
            PORTZ07_LAT = 0;
            PORTZ08_LAT = 0;
            //------------------
            PORTY03_LAT = 0;
            PORTY05_LAT = 0;
            //------------------
            PORTY11_LAT = 0;
            PORTY12_LAT = 0;
            //------------------
            PWM_SetDutyCycle(RIGHT_MOTOR, speed);
            PWM_SetDutyCycle(LEFT_MOTOR, speed);
            PWM_SetDutyCycle(WALL_ACT, speed);
            PWM_SetDutyCycle(ROLLER_MOTOR, speed);
        }
    }

}

void motorSpeed(int motor, int speed) {
    if (motor == RIGHT) {
        PWM_SetDutyCycle(PWM_PORTX11, speed * SCALE);
    } else if (motor == LEFT) {
        PWM_SetDutyCycle(PWM_PORTZ06, speed);
    } else if (motor == WALL) {
        PWM_SetDutyCycle(PWM_PORTY04, speed);
    } else if (motor == ROLLER) {
        PWM_SetDutyCycle(PWM_PORTY10, speed);
    } else if (motor == ALL) {
        PWM_SetDutyCycle(RIGHT_MOTOR, speed * SCALE);
        PWM_SetDutyCycle(LEFT_MOTOR, speed);
        PWM_SetDutyCycle(WALL_ACT, speed);
        PWM_SetDutyCycle(ROLLER_MOTOR, speed);
    }
}

void slugSpeed(int speed) {
    motorSpeed(RIGHT, speed);
    motorSpeed(LEFT, speed);
}

void moveSlug(int speed) {
    moveMotor(RIGHT, speed);
    moveMotor(LEFT, speed);
}

void dragSlug(int speedL, int speedR) {
    moveMotor(RIGHT, speedR);
    moveMotor(LEFT, speedL);
}

void turnSlugRight(int speed) {
    moveMotor(RIGHT, speed - 300);
    moveMotor(LEFT, speed);
}

void turnSlugLeft(int speed) {
    moveMotor(RIGHT, speed);
    moveMotor(LEFT, speed - 300);
}

void turnSlugSharpRight(int speed) {
    //moveMotor(RIGHT, FORWARD);
    moveMotor(LEFT, speed);
    moveMotor(RIGHT, 0);
}

void turnSlugSharpLeft(int speed) {
    moveMotor(RIGHT, speed);
    moveMotor(LEFT, 0);
}

void spinSlug(int dir, int speed) {
    if (dir == RIGHT) {
        moveMotor(RIGHT, -speed);
        moveMotor(LEFT, speed);
    } else if (dir == LEFT) {
        moveMotor(RIGHT, speed);
        moveMotor(LEFT, -speed);
    }

}

void swingWall(int flings) {

    for (int i = 0; i < flings; i++) {
        RC_SetPulseTime(SERVO_MOTOR, 1000);
        DELAY(YET_A_BIT_LONGER);
        RC_SetPulseTime(SERVO_MOTOR, 1600);
        DELAY(YET_A_BIT_LONGER);
    }
}

/// TEST HARNESS
// 1. SLUG_MOTORS_TEST: for testing motors attached to wheels
// 2. WR_MOTORS_TEST: for testing motors attached to wall/roller
// 3. SERVO: for testing the servo attached to the swinging wall
//#define SLUG_MOTORS_TEST
//#define WR_MOTORS_TEST
//#define SERVO
//#define SCALE_TEST

#ifdef SCALE_TEST
void run(int speed) {
    slugSpeed(speed);
    DELAY(A_LOT);
    DELAY(A_LOT);
    DELAY(A_LOT);
    slugSpeed(0);
    DELAY(A_LOT);
}

int main(void) {
    BOARD_Init();
    PWM_Init();
    motors_Init();
    
    int motorSpeed = 800;
    printf("\r\n Driving BOTH motors FORWARD");
    moveSlug(motorSpeed);
    run(motorSpeed);



    return 0;
}

#endif


#ifdef SLUG_MOTORS_TEST

#include <stdio.h>
#include <motors.h>

void run(int speed) {
    slugSpeed(speed);
    DELAY(A_LOT);
    DELAY(A_LOT);
    slugSpeed(0);
    DELAY(A_LOT);
}

int main(void) {
    BOARD_Init();
    PWM_Init();
    motors_Init();

    int motorSpeed = 1000;

    printf("\r\nWelcome To slugOlympics Motor Test Harness");
    slugSpeed(motorSpeed);
    DELAY(A_LOT);
    //DELAY(A_LOT);

    // DRIVING RIGHT MOTOR ONLY
    printf("\r\n Driving forward right motor");
    moveMotor(RIGHT, motorSpeed);
    run(motorSpeed);
    //
    printf("\r\n Driving backward right motor");
    moveMotor(RIGHT, -motorSpeed);
    run(-motorSpeed);

    /// DRIVING LEFT MOTOR ONLY
    printf("\r\n Driving forward left motor");
    moveMotor(LEFT, motorSpeed);
    run(motorSpeed);

    printf("\r\n Driving backward left motor");
    moveMotor(LEFT, -motorSpeed);
    run(-motorSpeed);

    // DRIVING BOTH MOTORS
    printf("\r\n Driving BOTH motors FORWARD");
    moveSlug(motorSpeed);
    run(motorSpeed);

    printf("\r\n Driving BOTH motors BACKWARD");
    moveSlug(-motorSpeed);
    run(-motorSpeed);

    /// TURNS
    printf("\r\n Turning Gradual Right");
    turnSlugRight(motorSpeed);
    DELAY(A_LOT);
    slugSpeed(0);
    DELAY(A_LOT);

    printf("\r\n Turning Gradual Left");
    turnSlugLeft(motorSpeed);
    DELAY(A_LOT);
    slugSpeed(0);
    DELAY(A_LOT);

    printf("\r\n Turning Sharp Right");
    turnSlugSharpRight(motorSpeed);
    DELAY(A_LOT);
    slugSpeed(0);
    DELAY(A_LOT);

    printf("\r\n Turning Sharp Left");
    turnSlugSharpLeft(motorSpeed);
    DELAY(A_LOT);
    slugSpeed(0);
    DELAY(A_LOT);

    printf("\r\n Spinning in place CW");
    spinSlug(RIGHT, motorSpeed);
    run(motorSpeed);

    printf("\r\n Spinning in place CCW");
    spinSlug(LEFT, motorSpeed);
    run(motorSpeed);

    return;
}

#endif


#ifdef WR_MOTORS_TEST
#include <stdio.h>
#include <motors.h>

void run(int motor, int speed) {
    motorSpeed(motor, speed);
    DELAY(A_LOT);
    DELAY(A_LOT);
    motorSpeed(motor, 0);
    DELAY(A_LOT);
}

int main(void) {
    BOARD_Init();
    PWM_Init();
    motors_Init();

    printf("\r\nWelcome to slugOlympics motor Test harness");
    DELAY(A_LOT);

    printf("\r\nTesting Wall Motor FORWARD");
    moveMotor(WALL, 600);
    run(WALL, 600);
    //
    printf("\r\nTesting Wall Motor BACKWARD");
    moveMotor(WALL, -600);
    run(WALL, -600);

    printf("\r\nTesting Roller Motor FORWARD");
    moveMotor(ROLLER, 350);
    run(ROLLER, 350);

    printf("\r\nTesting Roller Motor BACKWARD");
    moveMotor(ROLLER, -350);
    run(ROLLER, -350);
}

#endif


#ifdef SERVO    

int main(void) {
//    BOARD_Init();
//    RC_Init();
//    motors_Init();
//    PWM_Init();
    
    BOARD_Init();
    //PWM_Init();
    RC_Init();
    RC_AddPins(RC_PORTW07);
    
    
    printf("\r\nWelcome to slugOlympics Servo Test harness");

        while (1) {
            RC_SetPulseTime(RC_PORTW07, 1000);
            //printf("\r\nin while");

            DELAY(SOMEWHAT_LONG);
            RC_SetPulseTime(RC_PORTW07, 1600);
            DELAY(SOMEWHAT_LONG);
        }

    //swingWall(5);
}
#endif