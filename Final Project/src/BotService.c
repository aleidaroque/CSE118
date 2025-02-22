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
#include "AD.h"
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "BotService.h"
#include "sensors.h"
#include <stdio.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/

#define BATTERY_DISCONNECT_THRESHOLD 175
#define BEACON_TIMER_TICKS 5


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
#define BUMPER_BUFFER_SIZE 8
static uint8_t lastTrackWireParam = 0x00;
uint8_t trackParamR = 0;
uint8_t trackParamL = 0;
uint8_t trackWireParam = 0;
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
uint8_t InitBotService(uint8_t Priority) {
    ES_Event ThisEvent;

    MyPriority = Priority;

    // in here you write your initialization code
    // this includes all hardware and software initialization
    // that needs to occur.
    sensors_Init();
    // post the initial transition event
    ThisEvent.EventType = ES_INIT;

    sensors_Init();

    ES_Timer_InitTimer(BEACON_CHECK_TIMER, BEACON_TIMER_TICKS);

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
uint8_t PostBotService(ES_Event ThisEvent) {
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
ES_Event RunBotService(ES_Event ThisEvent) {
    ES_Event ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

    /********************************************
     in here you write your service code
     *******************************************/
    // BEACON
    static ES_EventTyp_t lastBeaconEvent = BEACON_NOT_FOUND;
    ES_EventTyp_t curBeaconEvent;
    int beaconStatus = beaconVal(); // read the battery voltage
    

    // BUMPER
    static ES_EventTyp_t lastBumperEvent = BUMPER_CHANGED;
    ES_EventTyp_t curBumperEvent;
    static unsigned char prevBumperValue = 0x0;
    static unsigned char bumperValues [BUMPER_BUFFER_SIZE];
    static int index = 0;
    unsigned char bumperValue = botReadBumpers();
    
    // TOP BUMPER
    static ES_EventTyp_t lastTopBumperEvent = TOP_BUMPER_CHANGED;
    ES_EventTyp_t curTopBumperEvent;
    static unsigned char prevTopBumperValue = 0x0;
    static unsigned char TopBumperValues [BUMPER_BUFFER_SIZE];
    static int Topindex = 0;
    unsigned char TopBumperValue = botReadBumpers();

    // TRACK WIRE
    static ES_EventTyp_t lastTrackWireEvent = TRACK_WIRE_NOT_FOUND;
    ES_EventTyp_t curTrackWireEvent;
    int trackWireRValue = trackWireR(); // read the track wire value
    int trackWireLValue = trackWireL(); // read the track wire value

    switch (ThisEvent.EventType) {
        case ES_INIT:
            // No hardware initialization or single time setups, those
            // go in the init function above.
            //
            // This section is used to reset service for some reason
            break;

        case ES_TIMERACTIVE:
            break;

        case ES_TIMERSTOPPED:
            break;

        case ES_TIMEOUT:
            ES_Timer_InitTimer(BEACON_CHECK_TIMER, BEACON_TIMER_TICKS);

            // BEACON SERVICE --------------------------------------------------            
            //printf("beaconStatus:%d\r\n", beaconStatus);
            beaconStatus = beaconVal();
            if (beaconStatus > 750) { // is battery connected?
                curBeaconEvent = BEACON_FOUND;
            } else if (beaconStatus < 350) {
                curBeaconEvent = BEACON_NOT_FOUND;
            } else {
                curBeaconEvent = lastBeaconEvent;
            }

            // BUMPER SERVICE --------------------------------------------------
            bumperValue = botReadBumpers();
            bumperValues[index] = bumperValue;
            index = (index + 1) % BUMPER_BUFFER_SIZE;
            //printf("\r\n bumper values: %d\r\n",bumperValue);

            // check that every value in array is same
            int stable = 1;
            for (int i = 0; i < BUMPER_BUFFER_SIZE; i++) {
                if (bumperValues[i] != bumperValue) {
                    stable = 0;
                    break;
                }
            }
            
            // TOP BUMPER SERVICE --------------------------------------------------
            TopBumperValue = botReadTopBumpers();
            TopBumperValues[Topindex] = TopBumperValue;
            Topindex = (Topindex + 1) % BUMPER_BUFFER_SIZE;
            //printf("\r\n bumper values: %d\r\n", TopBumperValue);

            // check that every value in array is same
            int TopStable = 1;
            for (int i = 0; i < BUMPER_BUFFER_SIZE; i++) {
                if (TopBumperValues[i] != TopBumperValue) {
                    TopStable = 0;
                    break;
                }
            }

            // TRACK WIRE SERVICE ----------------------------------------------
            trackWireRValue = trackWireR();
            trackWireLValue = trackWireL();
            //printf("\r\n track wire R value: %d\r\n track wire L value: %d\r\n", trackWireRValue, trackWireLValue);
            if (trackWireRValue > 400 || trackWireLValue > 400) {
                curTrackWireEvent = TRACK_WIRE_FOUND;
                if (trackWireRValue > 400) {
                    trackParamR = 1;
                } else if (trackWireRValue < 300) {
                    trackParamR = 0;
                }
                if (trackWireLValue > 400) {
                    trackParamL = 2;
                } else if (trackWireLValue < 300) {
                    trackParamL = 0;
                }
                trackWireParam = trackParamR | trackParamL;
            } else if (trackWireRValue < 300 && trackWireLValue < 300) {
                curTrackWireEvent = TRACK_WIRE_NOT_FOUND;
            } else {
                curTrackWireEvent = lastTrackWireEvent;
            }

            // POSTING ---------------------------------------------------------

            // Posting Beacon
            if (curBeaconEvent != lastBeaconEvent) { // check for change from last time
                ReturnEvent.EventType = curBeaconEvent;
                //ReturnEvent.EventParam = batVoltage;
                lastBeaconEvent = curBeaconEvent; // update history

#ifndef SIMPLESERVICE_TEST           // keep this as is for test harness
                PostTopHSM(ReturnEvent);
#else
                PostBotService(ReturnEvent);
#endif   
            }

            // Posting Bumper
            if (bumperValue == 0) {
                prevBumperValue = bumperValue;
            }
            if ((stable == 1) && (bumperValue != prevBumperValue) && (bumperValue > 0)) { // check for change from last time
                ReturnEvent.EventType = BUMPER_CHANGED;
                ReturnEvent.EventParam = botReadBumpers();
                prevBumperValue = bumperValue; // update history


#ifndef SIMPLESERVICE_TEST           // keep this as is for test harness
                PostTopHSM(ReturnEvent);
#else
                PostBotService(ReturnEvent);
#endif   
            }
            
            // Posting Top Bumper
            if (TopBumperValue == 0) {
                prevTopBumperValue = TopBumperValue;
            }
            if ((TopStable == 1) && (TopBumperValue != prevTopBumperValue) && (TopBumperValue > 0)) { // check for change from last time
                ReturnEvent.EventType = TOP_BUMPER_CHANGED;
                ReturnEvent.EventParam = botReadTopBumpers();
                prevTopBumperValue = TopBumperValue; // update history


#ifndef SIMPLESERVICE_TEST           // keep this as is for test harness
                PostTopHSM(ReturnEvent);
#else
                PostBotService(ReturnEvent);
#endif   
            }

            // Posting Track Wire
            if (curTrackWireEvent != lastTrackWireEvent || lastTrackWireParam != trackWireParam) { // check for change from last time
                ReturnEvent.EventType = curTrackWireEvent;
                ReturnEvent.EventParam = trackParamR | trackParamL;
                //ReturnEvent.EventParam = batVoltage;
                lastTrackWireEvent = curTrackWireEvent; // update history
                lastTrackWireParam = trackWireParam;

#ifndef SIMPLESERVICE_TEST           // keep this as is for test harness
                PostTopHSM(ReturnEvent);
#else
                PostBotService(ReturnEvent);
#endif   
            }


            break;





#ifdef SIMPLESERVICE_TEST     // keep this as is for test harness      
        default:
            printf("\r\nEvent: %s\tParam: 0x%d",
                    EventNames[ThisEvent.EventType], ThisEvent.EventParam);
            break;
#endif
    }

    return ReturnEvent;
}

/*******************************************************************************
 * PRIVATE FUNCTIONs                                                           *
 ******************************************************************************/

