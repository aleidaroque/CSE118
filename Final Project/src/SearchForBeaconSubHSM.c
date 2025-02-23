/*
 * File: SearchForBeaconSubHSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 * Author of Current Version: Aleida Diaz-Roque
 *
 * File to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * History
 * When           Who     What/Why
 * -------------- ---     --------
 * 05/15/24 10:00 adr      Modified template for ECE 118 Spring 2024 project
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
#include "SearchForBeaconSubHSM.h"
#include "sensors.h"
#include "motors.h"
#include "stdio.h"
#include "LED.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    InitPSubState,
    RotateSearch,
    InfinitySearchRight,
    InfinitySearchLeft,
    DriveToBeacon,
    Park,
    Reverse,
    Turning,
    ShortDrive,
} SearchForBeaconSubHSMState_t;

static const char *StateNames[] = {
	"InitPSubState",
	"RotateSearch",
	"InfinitySearchRight",
	"InfinitySearchLeft",
	"DriveToBeacon",
	"Park",
	"Reverse",
	"Turning",
	"ShortDrive",
};

// CONSUME TRACK WIRE EVENTS

// collision values


#define SPIN_TIMER_TICKS 3000
#define REVERSE_TIMER_TICKS 750
#define TURN_TIMER_TICKS 1000
#define SHORT_DRIVE_TIMER_TICKS 1000
#define TURN_90_TIMER_TICKS 1000
#define INFINITY_TIMER_TICKS 4500
#define PARK_TIMER_TICKS 4000


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

static SearchForBeaconSubHSMState_t CurrentState = InitPSubState; // <- change name to match ENUM
static uint8_t MyPriority;
static int collisionFrom = START;


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitSearchForBeaconSubHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitSearchForBeaconSubHSM(void) {
    ES_Event returnEvent;

    CurrentState = InitPSubState;
    returnEvent = RunSearchForBeaconSubHSM(INIT_EVENT);
    if (returnEvent.EventType == ES_NO_EVENT) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @Function RunSearchForBeaconSubHSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the heirarchical state
 *        machine, as this is called any time a new event is passed to the event
 *        queue. This function will be called recursively to implement the correct
 *        order for a state transition to be: exit current state -> enter next state
 *        using the ES_EXIT and ES_ENTRY events.
 * @note The lower level state machines are run first, to see if the event is dealt
 *       with there rather than at the current level. ES_EXIT and ES_ENTRY events are
 *       not consumed as these need to pass pack to the higher level state machine.
 * @author J. Edward Carryer, 2011.10.23 19:25
 * @author Gabriel H Elkaim, 2011.10.23 19:25 
 * @author Aleida Diaz-Roque */
ES_Event RunSearchForBeaconSubHSM(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE; // use to flag transition
    SearchForBeaconSubHSMState_t nextState; // <- change type to correct enum

    ES_Tattle(); // trace call stack

    switch (CurrentState) {
        case InitPSubState: // If current state is initial Psedudo State
            if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
            {
                // this is where you would put any actions associated with the
                // transition from the initial pseudo-state into the actual
                // initial state
                collisionFrom = START;

                // now put the machine into the actual initial state
                nextState = InfinitySearchRight;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
	    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
	    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

        case RotateSearch: // in the first state, replace this with correct names

            spinSlug(RIGHT, SPIN_SPEED);

            switch (ThisEvent.EventType) {

                case ES_ENTRY:
                    ES_Timer_InitTimer(SPIN_TIMER, SPIN_TIMER_TICKS);
                    printf("\r\n SearchForBeacon: In rotate search");
                    break;

                case ES_TIMEOUT:
                    nextState = InfinitySearchRight;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case BEACON_FOUND:
                    nextState = DriveToBeacon;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case ES_EXIT:
                    ES_Timer_StopTimer(SPIN_TIMER);
                    break;

                case TRACK_WIRE_FOUND:
                    break;

                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;
	    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
	    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

        case InfinitySearchRight: // in the first state, replace this with correct names

            turnSlugSharpRight(DRIVE_SPEED);

            switch (ThisEvent.EventType) {

                case ES_ENTRY:
                    //ES_Timer_StopTimer(COLLISION_TIMER);
                    ES_Timer_InitTimer(INFINITY_TIMER, INFINITY_TIMER_TICKS);
                    printf("\r\n SearchForBeacon: In Infinity Search Right");
                    collisionFrom = START;
                    ES_Timer_InitTimer(FINISH_TIMER, 30000);
                    break;

                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == INFINITY_TIMER) {
                        nextState = InfinitySearchLeft;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    } else if (ThisEvent.EventParam == FINISH_TIMER) {
                        ES_Timer_StopTimer(INFINITY_TIMER);
                        ThisEvent.EventType = AT_BEACON_TOWER;
                        ThisEvent.EventParam = 0;
                        return ThisEvent;
                    }
                    break;
                case TRACK_WIRE_FOUND:
                    break;

                case TAPE_SENSED:
                    printf("\r\n    TAPE SENSED: %d", ThisEvent.EventParam);

                    if (ThisEvent.EventParam == FRONT_RIGHT) {
                        collisionFrom = FRONT_RIGHT;
                        printf("\r\n    Tape: RIGHT");
                    } else if (ThisEvent.EventParam == FRONT_LEFT) {
                        collisionFrom = FRONT_LEFT;
                        printf("\r\n    Tape: LEFT");
                    } else if (ThisEvent.EventParam == FRONT_BOTH) {
                        collisionFrom = FRONT_BOTH;
                        printf("\r\n    Tape: FRONT");
                    }

                    nextState = Reverse;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case BUMPER_CHANGED:
                    printf("\r\n    BUMPER CHANGED: %d", ThisEvent.EventParam);

                    if (ThisEvent.EventParam == FRONT_RIGHT) {
                        collisionFrom = FRONT_RIGHT;
                        printf("\r\n    Bump: RIGHT");
                    } else if (ThisEvent.EventParam == FRONT_LEFT) {
                        collisionFrom = FRONT_LEFT;
                        printf("\r\n    Bump: LEFT");
                    } else if (ThisEvent.EventParam == FRONT_BOTH) {
                        collisionFrom = FRONT_BOTH;
                        printf("\r\n    Bump: FRONT");
                    } else {
                        ThisEvent.EventType = ES_NO_EVENT;
                    }

                    nextState = Reverse;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case TOP_BUMPER_CHANGED:
                    collisionFrom = DEAD_BOT;
                    nextState = Reverse;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case BEACON_FOUND:
                    nextState = DriveToBeacon;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case ES_EXIT:
                    ES_Timer_StopTimer(INFINITY_TIMER);

                    break;

                case ES_NO_EVENT:
                    break;
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;
	    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
	    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

        case InfinitySearchLeft: // in the first state, replace this with correct names

            turnSlugSharpLeft(DRIVE_SPEED);

            switch (ThisEvent.EventType) {

                case ES_ENTRY:
                    //ES_Timer_StopTimer(INFINITY_TIMER);
                    ES_Timer_InitTimer(SPIN_TIMER, INFINITY_TIMER_TICKS);
                    printf("\r\n SearchForBeacon: In Infinity Search Left");
                    collisionFrom = START;
                    break;

                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == SPIN_TIMER) {

                        ThisEvent.EventType = AT_BEACON_TOWER;
                        ThisEvent.EventParam = 0;
                        return ThisEvent;
                    } else if (ThisEvent.EventParam == FINISH_TIMER) {
                        ES_Timer_StopTimer(SPIN_TIMER);
                        ThisEvent.EventType = AT_BEACON_TOWER;
                        ThisEvent.EventParam = 0;
                        return ThisEvent;
                    }
                    break;

                case TAPE_SENSED:
                    printf("\r\n    TAPE SENSED: %d", ThisEvent.EventParam);

                    if (ThisEvent.EventParam == FRONT_RIGHT) {

                        collisionFrom = FRONT_RIGHT;
                        printf("\r\n    Tape: RIGHT");
                    } else if (ThisEvent.EventParam == FRONT_LEFT) {
                        collisionFrom = FRONT_LEFT;
                        printf("\r\n    Tape: LEFT");
                    } else if (ThisEvent.EventParam == FRONT_BOTH) {
                        collisionFrom = FRONT_BOTH;
                        printf("\r\n    Tape: FRONT");
                    }

                    nextState = Reverse;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case BUMPER_CHANGED:
                    printf("\r\n    BUMPER CHANGED: %d", ThisEvent.EventParam);

                    if (ThisEvent.EventParam == FRONT_RIGHT) {
                        collisionFrom = FRONT_RIGHT;
                        printf("\r\n    Bump: RIGHT");
                    } else if (ThisEvent.EventParam == FRONT_LEFT) {
                        collisionFrom = FRONT_LEFT;
                        printf("\r\n    Bump: LEFT");
                    } else if (ThisEvent.EventParam == FRONT_BOTH) {
                        collisionFrom = FRONT_BOTH;
                        printf("\r\n    Bump: FRONT");
                    } else {
                        ThisEvent.EventType = ES_NO_EVENT;
                    }

                    // maybe restart timer here to continue infinity search if we want that behavior
                    // currently: remaning time is used for search after coming back to this state
                    nextState = Reverse;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;


                case TOP_BUMPER_CHANGED:
                    collisionFrom = DEAD_BOT;
                    nextState = Reverse;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;



                case BEACON_FOUND:
                    nextState = DriveToBeacon;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case ES_EXIT:
                    ES_Timer_StopTimer(SPIN_TIMER);
                    break;

                case TRACK_WIRE_FOUND:
                    break;

                case ES_NO_EVENT:
                    break;
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;
	    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
	    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

        case DriveToBeacon: // in the first state, replace this with correct names
            moveSlug(DRIVE_SPEED);
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    // start driving forward
                    collisionFrom = START;
                    printf("\r\n SearchForBeacon: In DriveToBeacon");
                    moveSlug(DRIVE_SPEED);
                    break;

                case BUMPER_CHANGED:

                    if (ThisEvent.EventParam == FRONT_RIGHT) { // FR Bumper
                        printf("\r\n    Bump: RIGHT");
                        collisionFrom = FRONT_RIGHT;

                    } else if (ThisEvent.EventParam == FRONT_LEFT) { // FL Bumper
                        printf("\r\n    Bump: LEFT");
                        collisionFrom = FRONT_LEFT;

                    } else if (ThisEvent.EventParam == FRONT_BOTH) { // Both front bumpers
                        printf("\r\n    Bump: FRONT");
                        collisionFrom = FRONT_BOTH;

                    } else {
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    nextState = Reverse;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;

                case TAPE_SENSED:
                    nextState = Park;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case ES_EXIT:
                    moveSlug(NO_SPEED);
                    break;

                case TRACK_WIRE_FOUND:
                    break;

                case ES_NO_EVENT:
                    break;

                default:
                    break;
            }
            break;
	    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
	    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

        case Park:
            switch (ThisEvent.EventType) {

                case ES_ENTRY:
                    printf("\r\n SearchForBeacon: In Park");
                    ES_Timer_InitTimer(INFINITY_TIMER, PARK_TIMER_TICKS);
                    moveSlug(NO_SPEED);
                    break;

                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == INFINITY_TIMER) {
                        //Ready to Collect
                        ThisEvent.EventType = READY_TO_DEPOSIT;
                        ThisEvent.EventParam = 0;
                        return ThisEvent;
                    }
                    break;


                case ES_EXIT:
                    ES_Timer_StopTimer(INFINITY_TIMER);
                    moveSlug(NO_SPEED);
                    break;

                case TRACK_WIRE_FOUND:
                    break;

                case ES_NO_EVENT:
                    break;

                default:
                    break;
            }
            break;
	    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
	    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

        case Reverse:

            moveSlug(-DRIVE_SPEED);

            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    ES_Timer_InitTimer(COLLISION_TIMER, REVERSE_TIMER_TICKS);
                    printf("\r\n SearchForBeacon: In Reverse");
                    break;

                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == FINISH_TIMER) {
                        ES_Timer_StopTimer(COLLISION_TIMER);
                        ThisEvent.EventType = AT_BEACON_TOWER;
                        ThisEvent.EventParam = 0;
                        return ThisEvent;
                    } else {
                        ES_Timer_StopTimer(COLLISION_TIMER);
                        nextState = Turning;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                        break;
                    }

                case ES_EXIT:
                    ES_Timer_StopTimer(COLLISION_TIMER);
                    break;

                case ES_NO_EVENT:
                    break;

                case BUMPER_CHANGED:
                    if (ThisEvent.EventParam == 0b0010) {
                        // rear left bump
                        printf("\r\n    bump: REAR LEFT");
                    } else if (ThisEvent.EventParam == 0b0001) {
                        printf("\r\n    bumped REAR RIGHT");
                    } else if (ThisEvent.EventParam == 0b0011) {
                        printf("\r\n    bumped REAR BOTH");
                    }

                    nextState = Turning;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    ES_Timer_StopTimer(COLLISION_TIMER);

                    break;

                case TAPE_SENSED:
                    if ((ThisEvent.EventParam == 0b0001) || (ThisEvent.EventParam == 0b0010) || (ThisEvent.EventParam == 0b0011)) {
                        nextState = Turning;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                        ES_Timer_StopTimer(COLLISION_TIMER);
                    }

                    break;

                case TRACK_WIRE_FOUND:
                    break;

                default: // all unhandled events pass the event back up to the next level
                    break;

            }
            break;
	    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
	    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

        case Turning: // in the first state, replace this with correct names

            slugSpeed(DRIVE_SPEED);

            switch (ThisEvent.EventType) {

                case ES_ENTRY:
                    printf("\r\n SearchForBeacon: In Turning");
                    if (collisionFrom == FRONT_RIGHT) {
                        printf("\r\n    Turning RIGHT");
                        ES_Timer_InitTimer(COLLISION_TIMER, TURN_TIMER_TICKS);
                        turnSlugSharpLeft(DRIVE_SPEED);

                    } else if (collisionFrom == FRONT_LEFT) {
                        printf("\r\n    Turning LEFT");
                        ES_Timer_InitTimer(COLLISION_TIMER, TURN_TIMER_TICKS);
                        turnSlugSharpRight(DRIVE_SPEED);

                    } else if ((collisionFrom == FRONT_BOTH) || (collisionFrom == DEAD_BOT)) {
                        printf("\r\n    Turning RIGHT 90");
                        ES_Timer_InitTimer(COLLISION_TIMER, TURN_90_TIMER_TICKS);
                        turnSlugSharpRight(DRIVE_SPEED);
                    }
                    break;

                case TAPE_SENSED:
                    printf("\r\n TAPE SENSED: %d", ThisEvent.EventParam);
                    if (ThisEvent.EventParam == 0b0100) {

                        collisionFrom = FRONT_RIGHT;
                        printf("\r\n    Tape: RIGHT");
                    } else if (ThisEvent.EventParam == 0b1000) {
                        collisionFrom = FRONT_LEFT;
                        printf("\r\n    Tape: LEFT");
                    } else if (ThisEvent.EventParam == 0b1100) {
                        collisionFrom = FRONT_BOTH;
                        printf("\r\n    Tape: FRONT");
                    }

                    nextState = Reverse;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case BUMPER_CHANGED:
                    printf("\r\n    BUMPER CHANGED: %d", ThisEvent.EventParam);
                    if (ThisEvent.EventParam == 0b0100) {
                        collisionFrom = FRONT_RIGHT;
                        printf("\r\n    Bump: RIGHT");
                    } else if (ThisEvent.EventParam == 0b1000) {
                        collisionFrom = FRONT_LEFT;
                        printf("\r\n    Bump: LEFT");
                    } else if (ThisEvent.EventParam == 0b1100) {
                        collisionFrom = FRONT_BOTH;
                        printf("\r\n    Bump: FRONT");
                    } else {
                        ThisEvent.EventType = ES_NO_EVENT;
                    }

                    // maybe restart timer here to continue infinity search if we want that behavior
                    // currently: remaning time is used for search after coming back to this state                  
                    nextState = Reverse;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case TOP_BUMPER_CHANGED:
                    collisionFrom = DEAD_BOT;
                    nextState = Reverse;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case ES_EXIT:
                    ES_Timer_StopTimer(COLLISION_TIMER);
                    //ES_Timer_InitTimer(COLLISION_TIMER, SHORT_DRIVE_TIMER_TICKS);
                    break;

                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == FINISH_TIMER) {
                        ES_Timer_StopTimer(COLLISION_TIMER);
                        ThisEvent.EventType = AT_BEACON_TOWER;
                        ThisEvent.EventParam = 0;
                        return ThisEvent;
                    } else {
                        nextState = ShortDrive;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                        break;
                    }

                case TRACK_WIRE_FOUND:
                    break;

                case ES_NO_EVENT:
                default: // all unhandled events pass the event back up to the next level
                    break;
            }
            break;
	    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
	    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

        case ShortDrive: // in the first state, replace this with correct names
            switch (ThisEvent.EventType) {

                case ES_ENTRY:
                    ES_Timer_InitTimer(COLLISION_TIMER, SHORT_DRIVE_TIMER_TICKS);
                    moveSlug(DRIVE_SPEED);
                    printf("\r\n SearchForBeacon: In ShortDrive");
                    break;

                case TAPE_SENSED:
                    printf("\r\n    TAPE SENSED: %d", ThisEvent.EventParam);
                    if (ThisEvent.EventParam == 0b0100) {
                        collisionFrom = FRONT_RIGHT;
                        printf("\r\n    Tape: RIGHT");
                    } else if (ThisEvent.EventParam == 0b1000) {
                        collisionFrom = FRONT_LEFT;
                        printf("\r\n    Tape: LEFT");
                    } else if (ThisEvent.EventParam == 0b1100) {
                        collisionFrom = FRONT_BOTH;
                        printf("\r\n    Tape: FRONT");
                    }

                    nextState = Reverse;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case BUMPER_CHANGED:
                    printf("\r\n    BUMPER_CHANGED: %d", ThisEvent.EventParam);
                    if (ThisEvent.EventParam == 0b0100) {
                        collisionFrom = FRONT_RIGHT;
                        printf("\r\n    Bump: RIGHT");
                    } else if (ThisEvent.EventParam == 0b1000) {
                        collisionFrom = FRONT_LEFT;
                        printf("\r\n    Bump: LEFT");
                    } else if (ThisEvent.EventParam == 0b1100) {
                        collisionFrom = FRONT_BOTH;
                        printf("\r\n    Bump: FRONT");
                    } else {
                        ThisEvent.EventType = ES_NO_EVENT;
                    }

                    // maybe restart timer here to continue infinity search if we want that behavior
                    // currently: remaning time is used for search after coming back to this state
                    nextState = Reverse;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case TOP_BUMPER_CHANGED:
                    collisionFrom = DEAD_BOT;
                    nextState = Reverse;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case ES_EXIT:
                    ES_Timer_StopTimer(COLLISION_TIMER);
                    break;

                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == FINISH_TIMER) {
                        ES_Timer_StopTimer(COLLISION_TIMER);
                        ThisEvent.EventType = AT_BEACON_TOWER;
                        ThisEvent.EventParam = 0;
                        return ThisEvent;
                    } else {
                        nextState = InfinitySearchLeft;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                        break;
                    }

                case TRACK_WIRE_FOUND:
                    break;

                case ES_NO_EVENT:
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
        RunSearchForBeaconSubHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunSearchForBeaconSubHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

