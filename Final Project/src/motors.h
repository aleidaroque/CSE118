/* 
 * File:   motors.h
 * Author: aleidadiaz-roque
 *
 * Created on May 13, 2024, 9:37 AM
 */

#ifndef MOTORS_H
#define	MOTORS_H


#define DELAY(x)                      \
    for (int wait = 0; wait <= x; wait++) \
    {                                 \
        asm("nop");                   \
    }

#define A_BIT 18300
#define A_BIT_MORE 36600
#define YET_A_BIT_LONGER (A_BIT_MORE << 2)
#define SOMEWHAT_LONG 189000
#define A_LOT 1890000


/// @brief Initializes the all motor pwm/dir pins only
void motors_Init();

/// @brief Sets the direction of motion for the given motor
/// @param motor
/// @param DIR
void moveMotor(int motor, int speed);

/// @brief Sets the speed for the given motor
/// @param motor
/// @param speed 
void motorSpeed(int motor, int speed);

/// @brief Sets the speed for both wheel motors to move bot
/// @param speed 
void slugSpeed(int speed);


/// @brief  Sets both wheels to spin forward
void moveSlug(int speed);


/// @brief Right wheel turns faster for a LEFT turn
void turnSlugLeft(int speed);


/// @brief Left wheel turns faster for a RIGHT turn
void turnSlugRight(int speed);

void turnSlugSharpRight(int speed);


void turnSlugSharpLeft(int speed);

/// @brief Both wheels spin opposite, bot spins in place
void spinSlug(int dir, int speed);

/// @bried Swings the ball back and forth 5 times
void swingWall(int flings);

void dragSlug(int speedL, int speedR);

#endif	/* MOTORS_H */


