/*
 * File: TemplateHSM.h
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that 
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * There is another template file for the SubHSM's that is slightly differet, and
 * should be used for all of the subordinate state machines (flat or heirarchical)
 *
 * This is provided as an example and a good place to start.
 *
 * Created on 23/Oct/2011
 * Updated on 16/Sep/2013
 */

#ifndef HSM_TOP_H  // <- This should be changed to your own guard on both
#define HSM_TOP_H  //    of these lines


/*******************************************************************************
 * PUBLIC #INCLUDES                                                            *
 ******************************************************************************/

#include "ES_Configure.h"   // defines ES_Event, INIT_EVENT, ENTRY_EVENT, and EXIT_EVENT

/*******************************************************************************
 * PUBLIC #DEFINES                                                             *
 ******************************************************************************/

#define FRONT_BOTH 0b1100
#define FRONT_RIGHT 0b0100
#define FRONT_LEFT 0b1000
#define REAR_BOTH 0b0011
#define REAR_RIGHT 0b0001
#define REAR_LEFT 0b0010

#define FRONT_BOTH_BUMP 0b110011
#define FRONT_RIGHT_BUMP 0b010011
#define FRONT_LEFT_BUMP 0b100011
#define REAR_BOTH_BUMP 0b001111
#define REAR_RIGHT_BUMP 0b000111
#define REAR_LEFT_BUMP 0b001011

#define DEAD_BOT 0b1111
#define DEAD_BOT_MIDDLE 1234


#define START 1000
#define WALL 200
#define TED 350 
#define TAPE 250
#define OBSTACLE 300
#define FRANK 400

#define TOP_LEFT 987
#define TOP_RIGHT 654


#define TRACK_RIGHT 0b01
#define TRACK_LEFT 0b10
#define TRACK_BOTH 0b11

// speed values
#define NO_SPEED 0
#define SPIN_SPEED 600
#define ROLLER_SPEED 400
#define DRIVE_SPEED 900


#define FORWARD 1
#define BACKWARD 0

#define LEFT 0
#define RIGHT 100
#define WALL 200
#define ROLLER 300
#define ALL 400

/*******************************************************************************
 * PUBLIC TYPEDEFS                                                             *
 ******************************************************************************/


/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

/**
 * @Function InitTemplateHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitTopHSM(uint8_t Priority);


/**
 * @Function PostTemplateHSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostTopHSM(ES_Event ThisEvent);




/**
 * @Function RunTemplateHSM(ES_Event ThisEvent)
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
ES_Event RunTopHSM(ES_Event ThisEvent);

#endif /* HSM_TOP_H */

