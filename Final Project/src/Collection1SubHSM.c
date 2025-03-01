/*
 * File: Collection1SubHSM.c
 * Author of Template: J. Edward Carryer
 * Modified: Gabriel H Elkaim, Aleida Diaz-Roque
 *
 * File to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. 
 *
 * History
 * When           Who     What/Why
 * -------------- ---     --------
 * 05/15/24 10:00 adr	   modified code for CSE 118 Spring 2024 project	
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
#include "Collection1SubHSM.h"
#include "sensors.h"
#include "motors.h"
#include <stdio.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    InitPSubState,
    Reverse,
    CollisionReverse,
    StuckReverse,
    Turn90Left,
    Turn90Right,
    Turn45Left,
    Turn45Right,
    WallFollow,
    WallAdjust,
    OtherWallFollow,
    OtherWallAdjust,
    RightAlign,
    TapeFollowRight,
    Adjust90Left,
    DriveForward,
    AdjustingLeft,
    AdjustingRight,
    AlignReverse,




} Collection1SubHSMState_t;

static const char *StateNames[] = {
	"InitPSubState",
	"Reverse",
	"CollisionReverse",
	"StuckReverse",
	"Turn90Left",
	"Turn90Right",
	"Turn45Left",
	"Turn45Right",
	"WallFollow",
	"WallAdjust",
	"OtherWallFollow",
	"OtherWallAdjust",
	"RightAlign",
	"TapeFollowRight",
	"Adjust90Left",
	"DriveForward",
	"AdjustingLeft",
	"AdjustingRight",
	"AlignReverse",
};

#define REVERSE_TIMER_TICKS 400
#define TURN_90_TIMER_TICKS 600

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

static Collection1SubHSMState_t CurrentState = InitPSubState; // <- change name to match ENUM
static uint8_t MyPriority;

static int collisionFrom = START;
static int spinDirection;
static int alignCounter = 0;
static int bumperCounter = 0;
static int rightBumped = 0;
static int leftBumped = 0;
static int fromWall;


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitCollection1SubHSM(void)
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunCollection1SubFSM function.
 *        Returns TRUE if successful, FALSE otherwise
 * @author Aleida Diaz-Roque */
uint8_t InitCollection1SubHSM(void) {
    ES_Event returnEvent;

    CurrentState = InitPSubState;
    returnEvent = RunCollection1SubHSM(INIT_EVENT);
    if (returnEvent.EventType == ES_NO_EVENT) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @Function RunCollection1SubHSM(ES_Event ThisEvent)
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
 * @author Aleida Diaz-Roque */
ES_Event RunCollection1SubHSM(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE; // use to flag transition
    Collection1SubHSMState_t nextState; // <- change type to correct enum

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
                spinDirection = START;
                nextState = Reverse;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
            ///////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////

        case Reverse:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    if (spinDirection == START || spinDirection == RIGHT) {
                        turnSlugLeft(-DRIVE_SPEED);
                    } else {
                        turnSlugRight(-DRIVE_SPEED);
                    }
                    if (collisionFrom == TAPE) {
                        ES_Timer_InitTimer(CHECK_TIMER, REVERSE_TIMER_TICKS);
                    } else {
                        ES_Timer_InitTimer(CHECK_TIMER, REVERSE_TIMER_TICKS - 200);
                    }

                    printf("\r\nCollection1: Reverse");
                    break;

                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == CHECK_TIMER) {
                        if (spinDirection == START) {
                            nextState = Turn90Left;
                        } else if (spinDirection == LEFT) {
                            nextState = Adjust90Left;
                        } else if (spinDirection == RIGHT) {
                            nextState = Turn90Right;
                        }
                    }

                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case TAPE_SENSED:
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case ES_EXIT:
                    ES_Timer_StopTimer(CHECK_TIMER);
                    break;
                case ES_NO_EVENT:
                    break;
                default:
                    break;
            }
            break;
	    ///////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////

        case CollisionReverse:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    moveSlug(-DRIVE_SPEED);
                    ES_Timer_InitTimer(REVERSE_TIMER, REVERSE_TIMER_TICKS - 200);
                    printf("\r\nCollection1: CollisionReverse");
                    break;

                case ES_TIMEOUT:
                    if (spinDirection == LEFT) {
                        nextState = Turn45Right;
                    } else if (spinDirection == RIGHT) {
                        nextState = Turn45Left;
                    }

                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case TAPE_SENSED:
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case ES_EXIT:
                    ES_Timer_StopTimer(REVERSE_TIMER);
                    break;
                case ES_NO_EVENT:
                    break;
                default:
                    break;
            }
            break;
	    ///////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////
            
        case StuckReverse:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    moveSlug(-DRIVE_SPEED);
                    ES_Timer_InitTimer(CHECK_TIMER, REVERSE_TIMER_TICKS - 200);
                    printf("\r\nCollection1: CollisionReverse");
                    break;

                case ES_TIMEOUT:
                    if (spinDirection == LEFT) {
                        fromWall = TRUE;
                        nextState = Turn90Right;
                    } else if (spinDirection == RIGHT) {
                        nextState = Turn90Left;
                    }
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case TAPE_SENSED:
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case ES_EXIT:
                    ES_Timer_StopTimer(CHECK_TIMER);
                    break;
                case ES_NO_EVENT:
                    break;
                default:
                    break;
            }
            break;
	    ///////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////


        case Turn90Left:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    ES_Timer_InitTimer(FOLLOW_TIMER, 1000);
                    spinSlug(LEFT, SPIN_SPEED);
                    break;

                case ES_TIMEOUT:
                    nextState = WallFollow;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case ES_EXIT:
                    ES_Timer_StopTimer(FOLLOW_TIMER);
                    break;

                case TAPE_SENSED:
                    break;

                case ES_NO_EVENT:
                    break;
                default:
                    break;
            }
            break;
	    ///////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////

        case Adjust90Left:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    ES_Timer_InitTimer(FOLLOW_TIMER, 1000);
                    spinSlug(LEFT, SPIN_SPEED);
                    break;

                case ES_TIMEOUT:
                    if (fromWall == TRUE) {
                        nextState = WallFollow;
                    } else {
                        nextState = DriveForward;
                    }
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case TAPE_SENSED:
                    break;

                case ES_EXIT:
                    ES_Timer_StopTimer(FOLLOW_TIMER);
                    break;

                case ES_NO_EVENT:
                    break;
                default:
                    break;
            }
            break;
	    ///////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////

        case Turn45Left:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    ES_Timer_InitTimer(FOLLOW_TIMER, 500);
                    spinSlug(LEFT, SPIN_SPEED);
                    break;

                case ES_TIMEOUT:
                    nextState = WallFollow;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case ES_EXIT:
                    ES_Timer_StopTimer(FOLLOW_TIMER);
                    break;

                case ES_NO_EVENT:
                    break;
                default:
                    break;
            }
            break;
	    ///////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////

        case Turn45Right:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    ES_Timer_InitTimer(FOLLOW_TIMER, 500);
                    spinSlug(RIGHT, SPIN_SPEED);
                    break;

                case ES_TIMEOUT:
                    nextState = OtherWallFollow;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case ES_EXIT:
                    ES_Timer_StopTimer(FOLLOW_TIMER);
                    break;

                case ES_NO_EVENT:
                    break;
                default:
                    break;
            }
            break;
	    ///////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////


        case WallFollow:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    spinDirection = RIGHT;
                    fromWall = FALSE;
                    dragSlug(DRIVE_SPEED, DRIVE_SPEED - 200);
                    ES_Timer_InitTimer(COLLISION_TIMER, 5000);
                    printf("\r\nwall follow");
                    break;

                case WALL_FOUND:
                    nextState = WallAdjust;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case BUMPER_CHANGED:
                    nextState = WallAdjust;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case TOP_BUMPER_CHANGED:
                    nextState = CollisionReverse;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case TAPE_SENSED:
                    if (ThisEvent.EventParam == FRONT_LEFT) {
                        // LEFT tape hit
                        printf("\r\n    Tape: LEFT");
                        collisionFrom = FRONT_LEFT;
                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    if (ThisEvent.EventParam == FRONT_RIGHT) {
                        // LEFT tape hit
                        printf("\r\n    Tape: RIGHT");
                        collisionFrom = FRONT_RIGHT;
                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }

                    if ((ThisEvent.EventParam == FRONT_BOTH) || (alignCounter > 2)) {
                        printf("\r\n    Tape: FRONT");
                        // BOTH FRONT tape hit
                        alignCounter = 0;
                        collisionFrom = TAPE;
                        nextState = Reverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;


                    }
                    break;

                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == COLLISION_TIMER) {
                        nextState = Reverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;

                case ES_EXIT:
                    //                    ES_Timer_StopTimer(COLLISION_TIMER);
                    break;

                case ES_NO_EVENT:
                    break;
                default:
                    break;
            }
            break;
	    ///////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////

        case WallAdjust:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    spinSlug(LEFT, DRIVE_SPEED - 100);
                    printf("\r\nwall adjust");
                    break;

                case TOP_BUMPER_CHANGED:
                    nextState = CollisionReverse;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case WALL_NOT_FOUND:
                    nextState = WallFollow;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case TAPE_SENSED:
                    if (ThisEvent.EventParam == FRONT_LEFT) {
                        // LEFT tape hit
                        printf("\r\n    Tape: LEFT");
                        collisionFrom = FRONT_LEFT;
                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    if (ThisEvent.EventParam == FRONT_RIGHT) {
                        // LEFT tape hit
                        printf("\r\n    Tape: RIGHT");
                        collisionFrom = FRONT_RIGHT;
                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }

                    if ((ThisEvent.EventParam == FRONT_BOTH) || (alignCounter > 2)) {
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
                    break;
                case ES_NO_EVENT:
                    break;
                default:
                    break;
            }
            break;
	    ///////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////

        case Turn90Right:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    ES_Timer_InitTimer(FOLLOW_TIMER, 1000);
                    spinSlug(RIGHT, SPIN_SPEED);
                    break;

                case ES_TIMEOUT:
                    if (fromWall == TRUE) {
                        nextState = OtherWallFollow;
                    } else {
                        nextState = DriveForward;
                    }
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case TAPE_SENSED:
                    break;

                case ES_EXIT:
                    ES_Timer_StopTimer(FOLLOW_TIMER);
                    break;

                case ES_NO_EVENT:
                    break;
                default:
                    break;
            }
            break;
	    ///////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////

        case OtherWallFollow:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    spinDirection = LEFT;
                    fromWall = FALSE;
                    dragSlug(DRIVE_SPEED - 400, DRIVE_SPEED);
                    ES_Timer_InitTimer(COLLISION_TIMER, 5000);
                    printf("\r\n other wall follow");
                    break;

                case OTHER_WALL_FOUND:
                    nextState = OtherWallAdjust;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case BUMPER_CHANGED:
                    nextState = OtherWallAdjust;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case TOP_BUMPER_CHANGED:
                    nextState = CollisionReverse;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case TAPE_SENSED:
                    if (ThisEvent.EventParam == FRONT_LEFT) {
                        // LEFT tape hit
                        printf("\r\n    Tape: LEFT");
                        collisionFrom = FRONT_LEFT;
                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    if (ThisEvent.EventParam == FRONT_RIGHT) {
                        // LEFT tape hit
                        printf("\r\n    Tape: RIGHT");
                        collisionFrom = FRONT_RIGHT;
                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }

                    if ((ThisEvent.EventParam == FRONT_BOTH) || (alignCounter > 2)) {
                        printf("\r\n    Tape: FRONT");
                        // BOTH FRONT tape hit
                        alignCounter = 0;
                        collisionFrom = TAPE;
                        nextState = Reverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;

                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == COLLISION_TIMER) {
                        nextState = Reverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    break;

                case ES_EXIT:
                    //                    ES_Timer_StopTimer(COLLISION_TIMER);
                    break;

                case ES_NO_EVENT:
                    break;
                default:
                    break;
            }
            break;
	    ///////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////

        case OtherWallAdjust:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    spinSlug(RIGHT, DRIVE_SPEED - 300);
                    printf("\r\n other wall adjust");
                    break;

                case OTHER_WALL_NOT_FOUND:
                    nextState = OtherWallFollow;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case TOP_BUMPER_CHANGED:
                    nextState = CollisionReverse;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                    break;

                case TAPE_SENSED:
                    if (ThisEvent.EventParam == FRONT_LEFT) {
                        // LEFT tape hit
                        printf("\r\n    Tape: LEFT");
                        collisionFrom = FRONT_LEFT;
                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                    if (ThisEvent.EventParam == FRONT_RIGHT) {
                        // LEFT tape hit
                        printf("\r\n    Tape: RIGHT");
                        collisionFrom = FRONT_RIGHT;
                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }

                    if ((ThisEvent.EventParam == FRONT_BOTH) || (alignCounter > 2)) {
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
                    break;
                case ES_NO_EVENT:
                    break;
                default:
                    break;
            }
            break;
	    ///////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////


        case DriveForward:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    moveSlug(DRIVE_SPEED);
                    ES_Timer_InitTimer(FOLLOW_TIMER, 1000);
                    printf("\r\n drive forward");
                    break;

                case BUMPER_CHANGED:
                    printf("\r\n    BUMP SENSED: %d", ThisEvent.EventParam);

                    if (ThisEvent.EventParam == 0b1000) {
                        // LEFT bump hit
                        printf("\r\n    Bump: LEFT");
                        collisionFrom = FRONT_LEFT_BUMP;
                        leftBumped = 1;
                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }

                    if (ThisEvent.EventParam == 0b0100) {
                        // LEFT bump hit
                        printf("\r\n    Bump: RIGHT");
                        collisionFrom = FRONT_RIGHT_BUMP;
                        rightBumped = 1;
                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }

                    if ((ThisEvent.EventParam == 0b1100) || (bumperCounter > 2)) {
                        printf("\r\n    Bump: FRONT");
                        // BOTH FRONT tape hit
                        bumperCounter = 0;
                        leftBumped = 0;
                        rightBumped = 0;
                        fromWall = TRUE;
                        collisionFrom = WALL;

                        nextState = Reverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;

                    }
                    break;

                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == FOLLOW_TIMER) {
                        bumperCounter = 0;
                        leftBumped = 0;
                        rightBumped = 0;
                        fromWall = TRUE;
                        nextState = Reverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }

                    break;

                case ES_EXIT:
                    ES_Timer_StopTimer(FOLLOW_TIMER);
                    break;
                case ES_NO_EVENT:
                    break;
                default:
                    break;
            }
            break;
	    ///////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////

        case AdjustingRight: // in the first state, replace this with correct names
            printf("\r\nCollection2: In Adjusting Right");
            turnSlugSharpRight(DRIVE_SPEED - 75);

            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    break;


                case TAPE_SENSED:
                    printf("\r\n    TAPE SENSED: %d", ThisEvent.EventParam);

                    if (ThisEvent.EventParam == FRONT_LEFT) {
                        // LEFT tape hit
                        printf("\r\n    Tape: LEFT");
                        collisionFrom = FRONT_LEFT;
                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }

                    if ((ThisEvent.EventParam == FRONT_BOTH) || (alignCounter > 2)) {
                        printf("\r\n    Tape: FRONT");
                        // BOTH FRONT tape hit
                        alignCounter = 0;
                        collisionFrom = TAPE;
                        nextState = Reverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;


                    }
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

                    if ((ThisEvent.EventParam == 0b1100) || (bumperCounter > 2)) {
                        printf("\r\n    Bump: FRONT");
                        // BOTH FRONT tape hit
                        bumperCounter = 0;
                        leftBumped = 0;
                        rightBumped = 0;
                        fromWall = TRUE;
                        collisionFrom = WALL;

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
	    ///////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////

        case AdjustingLeft: // in the first state, replace this with correct names
            printf("\r\nCollection2: In Adjusting Right");
            turnSlugSharpLeft(DRIVE_SPEED - 75);

            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    break;


                case TAPE_SENSED:
                    printf("\r\n    TAPE SENSED: %d", ThisEvent.EventParam);

                    if (ThisEvent.EventParam == FRONT_RIGHT) {
                        // LEFT tape hit
                        printf("\r\n    Tape: RIGHT");
                        collisionFrom = FRONT_RIGHT;
                        nextState = AlignReverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }

                    if ((ThisEvent.EventParam == FRONT_BOTH) || (alignCounter > 2)) {
                        printf("\r\n    Tape: FRONT");
                        // BOTH FRONT tape hit
                        alignCounter = 0;
                        collisionFrom = TAPE;
                        nextState = Reverse;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
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

                    if ((ThisEvent.EventParam == 0b1100) || (bumperCounter > 2)) {
                        printf("\r\n    Bump: FRONT");
                        // BOTH FRONT tape hit
                        bumperCounter = 0;
                        leftBumped = 0;
                        rightBumped = 0;
                        fromWall = TRUE;
                        collisionFrom = WALL;

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
	    ///////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////

        case AlignReverse:

            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    // the timer will depend on ig coming from a tape or wall or track wire detection
                    printf("\r\nCollection1: In Align Reverse");
                    moveSlug(-DRIVE_SPEED);
                    alignCounter++;
                    if ((leftBumped == 1) && (rightBumped == 1)) {
                        bumperCounter++;
                    }
                    if ((collisionFrom == FRONT_RIGHT_BUMP) || (collisionFrom == FRONT_LEFT_BUMP)) {
                        ES_Timer_InitTimer(REVERSE_TIMER, 100);
                        printf("\r\n timer started");
                    }
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

                case ES_TIMEOUT:
                    if (collisionFrom == FRONT_LEFT_BUMP) {
                        ES_Timer_StopTimer(REVERSE_TIMER);

                        nextState = AdjustingLeft;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                        printf("\r\nalignreverseleft");
                    } else if (collisionFrom == FRONT_RIGHT_BUMP) {
                        ES_Timer_StopTimer(REVERSE_TIMER);
                        nextState = AdjustingRight;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                        printf("\r\nalignreverseleft");
                    }
                    break;



                case ES_EXIT:
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

        default:
            break;

    }

    // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunCollection1SubHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunCollection1SubHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

