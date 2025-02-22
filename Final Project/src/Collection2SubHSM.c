/*
 * File: TemplateSubHSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * There is for a substate machine. Make sure it has a unique name
 *
 * This is provided as an example and a good place to start.
 *
 * History
 * When           Who     What/Why
 * -------------- ---     --------
 * 09/13/13 15:17 ghe      added tattletail functionality and recursive calls
 * 01/15/12 11:12 jec      revisions for Gen2 framework
 * 11/07/11 11:26 jec      made the queue static
 * 10/30/11 17:59 jec      fixed references to CurrentEvent in RunTemplateSM()
 * 10/23/11 18:20 jec      began conversion from SMTemplate.c (02/20/07 rev)
 */


/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "ES_Configure.h"
#include "ES_Framework.h"
#include "BOARD.h"
#include "TopHSM.h"
#include "Collection2SubHSM.h"
#include "motors.h"
#include "LED.h"
#include <stdio.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    InitPSubState,
    DriveForward,
    AlignReverse,
    Reverse,
    Turn90Right,
    Turn90Left,
    Turn45Left,
    Turn180,
    AdjustingRight,
    AdjustingLeft,
    Stop,
    TapeFollowRight,
    RightAlign,
    FollowReverse,



} Collection2SubHSMState_t;

static const char *StateNames[] = {
	"InitPSubState",
	"DriveForward",
	"AlignReverse",
	"Reverse",
	"Turn90Right",
	"Turn90Left",
	"Turn45Left",
	"Turn180",
	"AdjustingRight",
	"AdjustingLeft",
	"Stop",
	"TapeFollowRight",
	"RightAlign",
	"FollowReverse",
};

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */


/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/
/* You will need MyPriority and the state variable; you may need others as well.
 * The type of state variable should match that of enum in header file. */

static Collection2SubHSMState_t CurrentState = InitPSubState; // <- change name to match ENUM
static uint8_t MyPriority;

static int collisionFrom = START;
static int alignCounter = 0;
static int bumperCounter = 0;
static int rightBumped = 0;
static int leftBumped = 0;


#define REVERSE_TIMER_TICKS 600
#define TURN_90_TIMER_TICKS 650

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateSubHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitCollection2SubHSM(void) {
    ES_Event returnEvent;

    CurrentState = InitPSubState;
    returnEvent = RunCollection2SubHSM(INIT_EVENT);
    if (returnEvent.EventType == ES_NO_EVENT) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @Function RunTemplateSubHSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the heirarchical state
 *        machine, as this is called any time a new event is passed to the event
 *        queue. This function will be called recursively to implement the correct
 *        order for a state transition to be: exit current state -> enter next state
 *        using the ES_EXIT and ES_ENTRY events.
 * @note Remember to rename to something appropriate.
 *       The lower level state machines are run first, to see if the event is dealt
 *       with there rather than at the current level. ES_EXIT and ES_ENTRY events are
 *       not consumed as these need to pass pack to the higher level state machine.
 * @author J. Edward Carryer, 2011.10.23 19:25
 * @author Gabriel H Elkaim, 2011.10.23 19:25 */
ES_Event RunCollection2SubHSM(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE; // use to flag transition
    Collection2SubHSMState_t nextState; // <- change type to correct enum

    ES_Tattle(); // trace call stack

    switch (CurrentState) {
        case InitPSubState: // If current state is initial Psedudo State
            if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
            {
                // this is where you would put any actions associated with the
                // transition from the initial pseudo-state into the actual
                // initial state

                // now put the machine into the actual initial state
                collisionFrom = START;
                alignCounter = 0;
                nextState = DriveForward;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case DriveForward: // in the first state, replace this with correct names

            switch (ThisEvent.EventType) {

                case ES_ENTRY:
                    ES_Timer_StopTimer(SPIN_TIMER); // from RightAlign
                    ES_Timer_StopTimer(COLLISION_TIMER); // from Turn90Right/Left & Turn180
                    ES_Timer_StopTimer(REVERSE_TIMER); // from Reverse
                    printf("\r\nCollection2: In Drive Forward");
                    moveSlug(DRIVE_SPEED);
                    break;

                case ES_EXIT:
                    break;

                case TAPE_SENSED:
                    printf("\r\n    TAPE SENSED: %d", ThisEvent.EventParam);
                    if (ThisEvent.EventParam == FRONT_RIGHT) {
                        printf("\r\n    Tape: RIGHT");
                        collisionFrom = FRONT_RIGHT;

                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;


                    } else if (ThisEvent.EventParam == FRONT_LEFT) {
                        printf("\r\n    Tape: LEFT");
                        collisionFrom = FRONT_LEFT;

                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;


                    } else if ((ThisEvent.EventParam == REAR_LEFT) || (ThisEvent.EventParam == REAR_RIGHT)) {
                        printf("\r\n    Tape: LEFT");
                        collisionFrom = REAR_BOTH;

                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }

                    if ((ThisEvent.EventParam == FRONT_BOTH) || (alignCounter == 3)) {
                        printf("\r\n    Tape: FRONT");
                        collisionFrom = TAPE;
                        alignCounter = 0;

                        nextState = Reverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;

                    }
                    break;


                case BUMPER_CHANGED:
                    // change parameter if statement later
                    if (ThisEvent.EventParam == FRONT_LEFT) {
                        printf("\r\n    Bump: LEFT");
                        collisionFrom = FRONT_LEFT_BUMP;

                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;

                    } else if (ThisEvent.EventParam == FRONT_RIGHT) {
                        printf("\r\n    Bump: RIGHT");
                        collisionFrom = FRONT_RIGHT_BUMP;

                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;

                    } else if (ThisEvent.EventParam == FRONT_BOTH) {
                        printf("\r\n    Bump: FRONT");
                        collisionFrom = WALL;
                        nextState = Reverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;

                case TOP_BUMPER_CHANGED:
                    collisionFrom = DEAD_BOT_MIDDLE;
                    nextState = Reverse;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case TRACK_WIRE_FOUND:

                    break;


                case ES_NO_EVENT:
                    break;

                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case TapeFollowRight:

            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    ES_Timer_StopTimer(SPIN_TIMER); // from RightAlign
                    ES_Timer_StopTimer(COLLISION_TIMER); // from Turn90Right/Left & Turn45Left
                    turnSlugRight(DRIVE_SPEED);
                    printf("\r\n COlleciton2: Tape Follow Right");

                    break;

                case TAPE_SENSED:
                    if (ThisEvent.EventParam == FRONT_RIGHT) {
                        nextState = RightAlign;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;

                    } else if (ThisEvent.EventParam == FRONT_BOTH || ThisEvent.EventParam == FRONT_LEFT || ThisEvent.EventParam == 0b1101) {
                        collisionFrom = TAPE;
                        nextState = FollowReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;

                case TOP_BUMPER_CHANGED:
                    collisionFrom = DEAD_BOT;
                    nextState = Reverse;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;

                    break;

                case BUMPER_CHANGED:
                    if (ThisEvent.EventParam == FRONT_LEFT) {
                        printf("\r\n    Bump: LEFT");
                        collisionFrom = FRONT_LEFT_BUMP;

                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;

                    } else if (ThisEvent.EventParam == FRONT_RIGHT) {
                        printf("\r\n    Bump: RIGHT");
                        collisionFrom = FRONT_RIGHT_BUMP;

                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;

                    } else if (ThisEvent.EventParam == FRONT_BOTH) {
                        printf("\r\n    Bump: FRONT");
                        collisionFrom = WALL;
                        nextState = Reverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;


                case TRACK_WIRE_FOUND:
                    moveSlug(NO_SPEED);
                    moveMotor(WALL, 700);
                    ThisEvent.EventType = READY_TO_DEPOSIT;
                    return ThisEvent;
                    break;

                default:
                    break;
            }

            break;

        case RightAlign:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    spinSlug(LEFT, DRIVE_SPEED);
                    //turnSlugSharpRight(-DRIVE_SPEED);
                    if (collisionFrom == DEAD_BOT) {
                        ES_Timer_InitTimer(SPIN_TIMER, TURN_90_TIMER_TICKS * 2);
                    }
                    break;

                case TAPE_NOT_SENSED:
                    nextState = TapeFollowRight;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case TAPE_SENSED:
                    if (ThisEvent.EventParam == FRONT_BOTH || ThisEvent.EventParam == FRONT_LEFT || ThisEvent.EventParam == 0b1101) {
                        collisionFrom = TAPE;
                        nextState = FollowReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    if (ThisEvent.EventParam == 0b0001) {
                        nextState = TapeFollowRight;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }

                    break;
                    //case TRACK_WIRE_FOUND:
                    //ThisEvent.EventType = ES_NO_EVENT;
                    //break;

                case TRACK_WIRE_FOUND:
                    moveSlug(NO_SPEED);
                    ThisEvent.EventType = READY_TO_DEPOSIT;
                    return ThisEvent;
                    break;

                case TOP_BUMPER_CHANGED:
                    collisionFrom = DEAD_BOT;
                    nextState = Reverse;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;

                    break;

                case ES_TIMEOUT:
                    nextState = DriveForward;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;

                case ES_EXIT:
                    ES_Timer_StopTimer(SPIN_TIMER);

                default:
                    break;
            }

            break;

        case FollowReverse:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    ES_Timer_StopTimer(SPIN_TIMER); // from RightAlign
                    moveSlug(-DRIVE_SPEED);
                    ES_Timer_InitTimer(FOLLOW_TIMER, REVERSE_TIMER_TICKS - 200);
                    break;

                case ES_TIMEOUT:
                    nextState = Turn90Left;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case TRACK_WIRE_FOUND:
                    //ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case TAPE_SENSED:
                    if ((ThisEvent.EventParam == REAR_RIGHT) || (ThisEvent.EventParam == REAR_LEFT) || (ThisEvent.EventParam == REAR_BOTH)) {
                        nextState = Turn90Left;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;

                case ES_EXIT:
                    ES_Timer_StopTimer(FOLLOW_TIMER);

                default:
                    break;
            }

            break;

        case Stop:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    // the timer will depend on ig coming from a tape or wall or track wire detection
                    moveSlug(NO_SPEED);
                    printf("\r\nCollection2: Stop");
                    ThisEvent.EventType = READY_TO_DEPOSIT;
                    return ThisEvent;
                    break;

                case TRACK_WIRE_NOT_FOUND:
                    break;

                case TRACK_WIRE_FOUND:
                    //ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case ES_EXIT:
                    break;

                case ES_NO_EVENT:
                    break;
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case AlignReverse:

            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    // the timer will depend on ig coming from a tape or wall or track wire detection
                    moveSlug(-DRIVE_SPEED);

                    alignCounter++;
                    if ((leftBumped == 1) && (rightBumped == 1)) {
                        bumperCounter++;
                    }

                    printf("\r\nCollection2: In Align Reverse");

                    //// bumpers
                    if ((collisionFrom == FRONT_RIGHT_BUMP) || (collisionFrom == FRONT_LEFT_BUMP)) {
                        ES_Timer_InitTimer(REVERSE_TIMER, 100);
                        printf("\r\n timer started");
                    }

                    /////// 

                    break;

                case TAPE_NOT_SENSED:
                    printf("\r\n tape not sensed");
                    if (collisionFrom == FRONT_RIGHT) {
                        nextState = AdjustingRight;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                        printf("\r\n alignreverseright");

                    } else if (collisionFrom == FRONT_LEFT) {
                        nextState = AdjustingLeft;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                        printf("\r\n alignreverseleft");
                    }
                    break;

                case TAPE_SENSED:
                    if ((ThisEvent.EventParam == 0b0001) || (ThisEvent.EventParam == 0b0010) || (ThisEvent.EventParam == 0b0011)) {
                        collisionFrom = REAR_BOTH;
                        nextState = Reverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;

                case ES_TIMEOUT:
                    if (collisionFrom == FRONT_LEFT_BUMP) {
                        nextState = AdjustingLeft;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                        printf("\r\nalignreverseleft");
                    } else if (collisionFrom == FRONT_RIGHT_BUMP) {
                        nextState = AdjustingRight;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                        printf("\r\nalignreverseleft");
                    }
                    break;

                case ES_EXIT:
                    //moveSlug(DRIVE_SPEED);
                    ES_Timer_StopTimer(REVERSE_TIMER);
                    break;

                case ES_NO_EVENT:
                    break;
                case TRACK_WIRE_FOUND:
                    //ThisEvent.EventType = ES_NO_EVENT;
                    // consume track wire event here
                    break;
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case Reverse: // in the first state, replace this with correct names

            moveSlug(-DRIVE_SPEED);

            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    //ES_Timer_StopTimer(REVERSE_TIMER); // from AlignReverse

                    // the timer will depend on ig coming from a tape or wall or track wire detection
                    if (collisionFrom == DEAD_BOT || collisionFrom == DEAD_BOT_MIDDLE) {
                        ES_Timer_InitTimer(REVERSE_TIMER, REVERSE_TIMER_TICKS);
                    } else {
                        ES_Timer_InitTimer(REVERSE_TIMER, 200);
                    }

                    printf("\r\nCollection2: In Reverse");
                    break;

                case ES_TIMEOUT:

                    if (collisionFrom == WALL) {
                        nextState = Turn90Left;
                    } else if (collisionFrom == TAPE) {
                        nextState = Turn90Left;
                    } else if (collisionFrom == FRONT_RIGHT) {
                        nextState = Turn90Left;
                    } else if (collisionFrom == FRONT_LEFT) {
                        nextState = Turn90Right;
                    } else if (collisionFrom == REAR_BOTH) {
                        nextState = DriveForward;
                    } else if (collisionFrom == DEAD_BOT) {
                        nextState = Turn45Left;
                    } else if (collisionFrom == DEAD_BOT_MIDDLE) {
                        nextState = Turn180;
                    }

                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    ES_Timer_StopTimer(REVERSE_TIMER);
                    break;

                case ES_EXIT:
                    ES_Timer_StopTimer(REVERSE_TIMER);
                    //moveSlug(DRIVE_SPEED);
                    break;


                case TAPE_SENSED:
                    if ((ThisEvent.EventParam == REAR_RIGHT) || (ThisEvent.EventParam == REAR_LEFT) || (ThisEvent.EventParam == REAR_BOTH)) {
                        collisionFrom = REAR_BOTH;
                        nextState = Turn90Left;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                        break;
                    }

                case ES_NO_EVENT:
                    break;
                case TRACK_WIRE_FOUND:
                    //ThisEvent.EventType = ES_NO_EVENT;
                    break;
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case Turn90Right: // in the first state, replace this with correct names

            spinSlug(RIGHT, DRIVE_SPEED);

            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    ES_Timer_StopTimer(REVERSE_TIMER); // from Reverse
                    ES_Timer_StopTimer(COLLISION_TIMER);
                    ES_Timer_InitTimer(COLLISION_TIMER, TURN_90_TIMER_TICKS);
                    printf("\r\nCollection2: In Turn90 Right");
                    break;

                case ES_TIMEOUT:
                    if ((collisionFrom == FRONT_RIGHT_BUMP) || (collisionFrom == FRONT_LEFT_BUMP)) {
                        nextState = DriveForward;
                    } else {
                        nextState = DriveForward;
                    }
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case ES_EXIT:
                    ES_Timer_StopTimer(COLLISION_TIMER);
                    //moveSlug(DRIVE_SPEED);
                    break;

                case ES_NO_EVENT:
                    break;
                case TRACK_WIRE_FOUND:
                    //ThisEvent.EventType = ES_NO_EVENT;
                    break;
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case Turn90Left: // in the first state, replace this with correct names

            spinSlug(LEFT, DRIVE_SPEED);

            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    ES_Timer_StopTimer(REVERSE_TIMER); // from Reverse
                    ES_Timer_StopTimer(FOLLOW_TIMER); // from FollowReverse
                    //ES_Timer_StopTimer(COLLISION_TIMER);
                    ES_Timer_InitTimer(COLLISION_TIMER, TURN_90_TIMER_TICKS);
                    printf("\r\nCollection2: In Turn90 Left");
                    break;

                case ES_TIMEOUT:
                    if ((collisionFrom == FRONT_RIGHT_BUMP) || (collisionFrom == FRONT_LEFT_BUMP) || (collisionFrom == WALL)) {
                        nextState = DriveForward;
                    } else {
                        nextState = TapeFollowRight;
                    }
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case ES_EXIT:
                    ES_Timer_StopTimer(COLLISION_TIMER);
                    //moveSlug(DRIVE_SPEED);
                    break;



                case ES_NO_EVENT:
                    break;
                case TRACK_WIRE_FOUND:
                    //ThisEvent.EventType = ES_NO_EVENT;
                    break;
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case Turn45Left: // in the first state, replace this with correct names

            spinSlug(LEFT, DRIVE_SPEED);

            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    ES_Timer_StopTimer(REVERSE_TIMER); // from Reverse
                    ES_Timer_StopTimer(COLLISION_TIMER);
                    ES_Timer_InitTimer(COLLISION_TIMER, TURN_90_TIMER_TICKS / 2);
                    printf("\r\nCollection2: In Turn45 Left");
                    break;

                case ES_TIMEOUT:
                    nextState = TapeFollowRight;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case ES_EXIT:
                    ES_Timer_StopTimer(COLLISION_TIMER);
                    //moveSlug(DRIVE_SPEED);
                    break;

                case ES_NO_EVENT:
                    break;
                case TRACK_WIRE_FOUND:
                    //ThisEvent.EventType = ES_NO_EVENT;
                    break;
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case Turn180:
            spinSlug(RIGHT, DRIVE_SPEED);

            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    ES_Timer_StopTimer(REVERSE_TIMER); // from Reverse
                    ES_Timer_StopTimer(COLLISION_TIMER);
                    ES_Timer_InitTimer(COLLISION_TIMER, TURN_90_TIMER_TICKS * 2);
                    printf("\r\nCollection2: In Turn180");
                    break;

                case ES_TIMEOUT:
                    nextState = DriveForward;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;
                case ES_EXIT:
                    ES_Timer_StopTimer(COLLISION_TIMER);
                    //moveSlug(DRIVE_SPEED);
                    break;

                case ES_NO_EVENT:
                    break;
                case TRACK_WIRE_FOUND:
                    //ThisEvent.EventType = ES_NO_EVENT;
                    break;
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;


        case AdjustingRight: // in the first state, replace this with correct names
            printf("\r\nCollection2: In Adjusting Right");
            turnSlugSharpRight(DRIVE_SPEED - 75);

            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    ES_Timer_StopTimer(REVERSE_TIMER); // from AlignReverse
                    break;

                case BUMPER_CHANGED:
                    printf("\r\n    BUMP SENSED: %d", ThisEvent.EventParam);

                    if (ThisEvent.EventParam == 0b1000) {
                        // LEFT tape hit
                        printf("\r\n    Bump: LEFT");
                        collisionFrom = FRONT_LEFT_BUMP;
                        leftBumped = 1;
                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }

                    if (ThisEvent.EventParam == 0b0100) {
                        // LEFT tape hit
                        printf("\r\n    Bump: RIGHT");
                        collisionFrom = FRONT_RIGHT_BUMP;
                        rightBumped = 1;
                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }

                    if ((ThisEvent.EventParam == 0b1100) || (bumperCounter > 3)) {
                        printf("\r\n    Bump: FRONT");
                        // BOTH FRONT tape hit
                        bumperCounter = 0;
                        leftBumped = 0;
                        rightBumped = 0;
                        collisionFrom = WALL;

                        nextState = Reverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;

                    }
                    break;

                case TAPE_SENSED:
                    printf("\r\n    TAPE SENSED: %d", ThisEvent.EventParam);

                    if (ThisEvent.EventParam == 0b1000) {
                        // LEFT tape hit
                        printf("\r\n    Tape: LEFT");
                        collisionFrom = FRONT_LEFT;

                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;


                    }

                    if ((ThisEvent.EventParam == 0b1100) || (alignCounter > 2)) {
                        printf("\r\n    Tape: FRONT");
                        // BOTH FRONT tape hit
                        alignCounter = 0;
                        collisionFrom = TAPE;

                        nextState = Reverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;

                    }
                    break;

                case ES_EXIT:
                    //moveSlug(DRIVE_SPEED);
                    break;

                case ES_NO_EVENT:
                    break;
                case TRACK_WIRE_FOUND:
                    //ThisEvent.EventType = ES_NO_EVENT;
                    break;
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;

        case AdjustingLeft: // in the first state, replace this with correct names
            printf("\r\nCollection2: In Adjusting Left");
            turnSlugSharpLeft(DRIVE_SPEED - 75);

            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    ES_Timer_StopTimer(REVERSE_TIMER); // from AlignReverse
                    //turnSlugSharpLeft(DRIVE_SPEED - 75);
                    break;

                case BUMPER_CHANGED:
                    printf("\r\n    TAPE_SENSED: %d", ThisEvent.EventParam);

                    if (ThisEvent.EventParam == 0b0100) {
                        printf("\r\n    Bump: RIGHT");
                        // RIGHT tape hit
                        collisionFrom = FRONT_RIGHT_BUMP;
                        rightBumped = 1;

                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;


                    }

                    if (ThisEvent.EventParam == 0b1000) {
                        printf("\r\n    Bump: RIGHT");
                        // RIGHT tape hit
                        collisionFrom = FRONT_LEFT_BUMP;
                        leftBumped = 1;

                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;


                    }
                    // at a sharp angle it will hit the side six times before actually being alligned
                    // 

                    if ((ThisEvent.EventParam == 0b1100) || (bumperCounter > 3)) {
                        printf("\r\n    Bump: FRONT");
                        // BOTH FRONT tape hit
                        bumperCounter = 0;
                        leftBumped = 0;
                        rightBumped = 0;
                        collisionFrom = WALL;

                        nextState = Reverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;

                    }
                    break;

                case TAPE_SENSED:
                    printf("\r\n    TAPE_SENSED: %d", ThisEvent.EventParam);

                    if (ThisEvent.EventParam == 0b0100) {
                        printf("\r\n    Tape: RIGHT");
                        // RIGHT tape hit
                        collisionFrom = FRONT_RIGHT;

                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;


                    }

                    if ((ThisEvent.EventParam == 0b1100) || (alignCounter > 2)) {
                        printf("\r\n    Tape: FRONT");
                        // BOTH FRONT tape hit
                        alignCounter = 0;
                        collisionFrom = TAPE;

                        nextState = Reverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;

                    }
                    break;

                case ES_EXIT:

                    //moveSlug(DRIVE_SPEED);
                    break;

                case ES_NO_EVENT:
                    break;
                case TRACK_WIRE_FOUND:
                    //ThisEvent.EventType = ES_NO_EVENT;
                    break;
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;


        default: // all unhandled states fall into here
            break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunCollection2SubHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunCollection2SubHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

