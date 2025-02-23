/*
 * File: Collection1SubHSM.h
 * Author of Template: J. Edward Carryer
 * Modified: Gabriel H Elkaim and Soja-Marie Morgens
 * Current Version by: Aleida Diaz-Roque
 *
 * File to set up a Heirarchical SubState Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that 
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * Make sure each SubState machine has a unique name and is #include in the
 * higher level state machine using it
 *
 * Created on 23/Oct/2011
 * Updated on 16/Sep/2013
 */

#ifndef SUB_COLLECTION1_HSM_H
#define SUB_COLLECTION1_HSM_H 


/*******************************************************************************
 * PUBLIC #INCLUDES                                                            *
 ******************************************************************************/

#include "ES_Configure.h"   // defines ES_Event, INIT_EVENT, ENTRY_EVENT, and EXIT_EVENT

/*******************************************************************************
 * PUBLIC #DEFINES                                                             *
 ******************************************************************************/


/*******************************************************************************
 * PUBLIC TYPEDEFS                                                             *
 ******************************************************************************/


/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

/**
 * @Function InitCollection1SubHSM(void)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author Aleida Diaz-Roque */
uint8_t InitCollection1SubHSM(void);

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
ES_Event RunCollection1SubHSM(ES_Event ThisEvent);

#endif /* SUB_HSM_Collection1_H */

