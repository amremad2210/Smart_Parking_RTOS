/*
 * gate_fsm.c
 *
 * Purpose:
 * - Implements the gate finite state machine.
 *
 * Responsibilities:
 * - Store and update the current gate state.
 * - Protect shared gate state using a mutex.
 * - Process gate events.
 * - Control transitions between:
 *      - IDLE_OPEN
 *      - IDLE_CLOSED
 *      - OPENING
 *      - CLOSING
 *      - STOPPED_MIDWAY
 *      - REVERSING
 *
 * This file should contain the main state-transition rules.
 */

 #include "gate_fsm.h"
 #include "gate_shared.h"
 #include "gate_types.h"

 void GateFSM_Init(void)
 {
    xSemaphoreTake(xGateStateMutex, portMAX_DELAY);
    gGateState = IDLE_CLOSED;
    xSemaphoreGive(xGateStateMutex);
 }

 GateState_t GateFSM_GetState(void)
 {
    GateState_t state;
    xSemaphoreTake(xGateStateMutex, portMAX_DELAY);
    state = gGateState;
    xSemaphoreGive(xGateStateMutex);
    return state;
 }

 void GateFSM_SetState(GateState_t newState)
 {
    xSemaphoreTake(xGateStateMutex, portMAX_DELAY);
    gGateState = newState;
    xSemaphoreGive(xGateStateMutex);
 }

 void GateFSM_ProcessEvent(GateEvent_t event)
 {
    switch(gGateState)
    {
        case IDLE_OPEN:
            if(event.type == EVT_DRIVER_CLOSE_AUTO || event.type == EVT_SECURITY_CLOSE_AUTO 
            || event.type == EVT_DRIVER_CLOSE_MANUAL_START || event.type == EVT_SECURITY_CLOSE_MANUAL_START)
            {
                GateFSM_SetState(CLOSING);
                //turn LED red
            }
            break;
        case IDLE_CLOSED:
            if(event.type == EVT_DRIVER_OPEN_AUTO || event.type == EVT_SECURITY_OPEN_AUTO 
            || event.type == EVT_DRIVER_OPEN_MANUAL_START || event.type == EVT_SECURITY_OPEN_MANUAL_START)
            {
                GateFSM_SetState(OPENING);
                //turn LED green
            }
            break;
        case OPENING:
            if(event.type == EVT_DRIVER_OPEN_MANUAL_RELEASE || event.type == EVT_SECURITY_OPEN_MANUAL_RELEASE
            || event.type == EVT_CONFLICT)
            {
                GateFSM_SetState(STOPPED_MIDWAY);
                //turn off LED
            }
            else if(event.type == EVT_OPEN_LIMIT_PRESS)
            {
                GateFSM_SetState(IDLE_OPEN);
                //turn off LED
            }
            break;
        case CLOSING:
            if(event.type == EVT_DRIVER_CLOSE_MANUAL_RELEASE || event.type == EVT_SECURITY_CLOSE_MANUAL_RELEASE
            || event.type == EVT_CONFLICT)
            {
                GateFSM_SetState(STOPPED_MIDWAY);
                //turn off LED
            }
            else if(event.type == EVT_CLOSED_LIMIT_PRESS)
            {
                GateFSM_SetState(IDLE_CLOSED);
                //turn off LED
            }
            else if(event.type == EVT_OBSTACLE_PRESS)
            {
                GateFSM_SetState(REVERSING);
                xSemaphoreGive(xObstacleSemaphore);
                //turn LED green
            }
            break;
        case STOPPED_MIDWAY:
            if(event.type == EVT_DRIVER_OPEN_AUTO || event.type == EVT_SECURITY_OPEN_AUTO 
            || event.type == EVT_DRIVER_OPEN_MANUAL_START || event.type == EVT_SECURITY_OPEN_MANUAL_START)
            {
                GateFSM_SetState(OPENING);
                //turn LED green
            }
            else if(event.type == EVT_DRIVER_CLOSE_AUTO || event.type == EVT_SECURITY_CLOSE_AUTO 
            || event.type == EVT_DRIVER_CLOSE_MANUAL_START || event.type == EVT_SECURITY_CLOSE_MANUAL_START)
            {
                GateFSM_SetState(CLOSING);
                //turn LED red
            }
            break;
        case REVERSING:
            if (event.type == EVT_REVERSE_TIMEOUT)
            {
                GateFSM_SetState(STOPPED_MIDWAY);
                //turn off LED
            }
            break;
        default:
            break;
    }
 }