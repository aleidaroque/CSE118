/*
 * File: TemplateService.h
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a simple service to work with the Events and Services
 * Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that this file
 * will need to be modified to fit your exact needs, and most of the names will have
 * to be changed to match your code.
 *
 * This is provided as an example and a good place to start.
 *
 * Created on 23/Oct/2011
 * Updated on 13/Nov/2013
 */

/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "BOARD.h"
#include "roach.h"
#include "AD.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "Service.h"
#include "HSM.h"
#include <stdio.h>
// #include "FSM.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/

#define BATTERY_DISCONNECT_THRESHOLD 175
#define LIGHT_THRESHOLD 470
#define DARK_THRESHOLD 530
#define TIMER_0_TICKS 5

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                    *
 ******************************************************************************/
/* You will need MyPriority and maybe a state variable; you may need others
 * as well. */

static uint8_t MyPriority;

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateService(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateService function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitService(uint8_t Priority) {
    ES_Event ThisEvent;

    MyPriority = Priority;

    // in here you write your initialization code
    // this includes all hardware and software initialization
    // that needs to occur.

    // post the initial transition event
    ES_Timer_InitTimer(SIMPLE_SERVICE_TIMER, TIMER_0_TICKS);
    ThisEvent.EventType = ES_INIT;
    if (ES_PostToService(MyPriority, ThisEvent) == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @Function PostTemplateService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostService(ES_Event ThisEvent) {
    return ES_PostToService(MyPriority, ThisEvent);
}

/**
 * @Function RunTemplateService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the service,
 *        as this is called any time a new event is passed to the event queue. 
 * @note Remember to rename to something appropriate.
 *       Returns ES_NO_EVENT if the event have been "consumed." 
 * @author J. Edward Carryer, 2011.10.23 19:25 */
#define BUMPER_BUFFER_SIZE 8
ES_Event RunService(ES_Event ThisEvent) {
    ES_Event ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

    /********************************************
     in here you write your service code
     *******************************************/
    static ES_EventTyp_t lastEvent = BUMPER_RELEASED;
    static uint16_t prevBumperValue = 0x0;
    // static char pastValues = 0x00;
    static uint16_t bumperValues [BUMPER_BUFFER_SIZE];
    static int index = 0;
    ES_EventTyp_t curEvent;

    switch (ThisEvent.EventType) {
        case ES_INIT:
            // No hardware initialization or single time setups, those
            // go in the init function above.
            //
            // This section is used to reset service for some reason
            break;

        case ES_TIMERACTIVE:
        case ES_TIMERSTOPPED:
            break;

        case ES_TIMEOUT:
            ES_Timer_InitTimer(SIMPLE_SERVICE_TIMER, TIMER_0_TICKS);

            // Read Bumper Code
            uint16_t bumperValue = Roach_ReadBumpers();
            bumperValues[index] = bumperValue;
            index = (index + 1) % BUMPER_BUFFER_SIZE;
            
            // check that every value in array is same
            int stable = 1;
            for (int i = 0; i < BUMPER_BUFFER_SIZE; i++) {
                if (bumperValues[i] != bumperValue) {
                    stable = 0;
                    break;
                }
            }
            
   
            
//            pastValues = pastValues << 1;
//
//
//
//
//            if (bumperValue != 0x0) { // all released
//                pastValues = pastValues | 0x01;
//            }
//
//            if (lastEvent == BUMPER_RELEASED) { // debounce the press
//                if (pastValues & 0xFFFFFFFF == 0xFFFFFFFF) {
//                    curEvent = BUMPER_PRESSED;
//                } else {
//                    curEvent = BUMPER_RELEASED;
//                }
//            } else if (lastEvent == BUMPER_PRESSED) { // debounce the release
//                if (pastValues == 0) {
//                    curEvent = BUMPER_RELEASED;
//                } else {
//                    curEvent = BUMPER_PRESSED;
//                }
//            }
            
            
            // do light check
            static ES_EventTyp_t lastLightEvent = LIGHT_SENSOR_LIGHT;
            ES_EventTyp_t curLightEvent;
            ES_Event thisLightEvent;
            uint16_t lightValue = Roach_LightLevel();
            
            if (lastLightEvent == LIGHT_SENSOR_LIGHT && lightValue > DARK_THRESHOLD) {
                curLightEvent = LIGHT_SENSOR_DARK;
            } else if (lastLightEvent == LIGHT_SENSOR_DARK && lightValue < LIGHT_THRESHOLD) {
                curLightEvent = LIGHT_SENSOR_LIGHT;
            } else {
                curLightEvent = lastLightEvent;
            }
            
            

            // bumper pressed/released
//            if (curEvent != lastEvent) { // check for change from last time
//                ReturnEvent.EventType = curEvent;
//                ReturnEvent.EventParam = bumperValue;
//                lastEvent = curEvent; // update history
                
            // bumper changed
            if (stable == 1 && bumperValue != prevBumperValue) { // check for change from last time
                  ReturnEvent.EventType = BUMPER_CHANGED;
                  ReturnEvent.EventParam = bumperValue;
                  prevBumperValue = bumperValue; // update history
            
#ifndef SIMPLESERVICE_TEST           // keep this as is for test harness
                PostHSM(ReturnEvent);
#else
                PostService(ReturnEvent);
#endif   
            }
            
            if (curLightEvent != lastLightEvent) { // check for change from last time
                printf("\r\nProcessed light event in the service instead of event checker!");
                ReturnEvent.EventType = curLightEvent;
                ReturnEvent.EventParam = lightValue;
                lastLightEvent = curLightEvent; // update history
            
#ifndef SIMPLESERVICE_TEST           // keep this as is for test harness
                PostHSM(ReturnEvent);
#else
                PostService(ReturnEvent);
#endif   
            }
            break;
#ifdef SIMPLESERVICE_TEST     // keep this as is for test harness      
        default:
            printf("\r\nEvent: %s\tParam: 0x%X",
                    EventNames[ThisEvent.EventType], ThisEvent.EventParam);
            break;
#endif
    }

    return ReturnEvent;
}

/*******************************************************************************
 * PRIVATE FUNCTIONs                                                           *
 ******************************************************************************/

