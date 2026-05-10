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

#include <stdio.h>

 void GateFSM_Init(void)
 {
    xSemaphoreTake(xGateStateMutex, portMAX_DELAY);
    gGateState = IDLE_CLOSED;
    xSemaphoreGive(xGateStateMutex);
		printf("Gate initialized at idle closed position.\n");
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
							printf("FSM: Gate is closing\n");
                //turn LED red
            }
            break;
        case IDLE_CLOSED:
            if(event.type == EVT_DRIVER_OPEN_AUTO || event.type == EVT_SECURITY_OPEN_AUTO 
            || event.type == EVT_DRIVER_OPEN_MANUAL_START || event.type == EVT_SECURITY_OPEN_MANUAL_START)
            {
                GateFSM_SetState(OPENING);
							printf("FSM: Gate is opening\n");
                //turn LED green
            }
            break;
        case OPENING:
            if(event.type == EVT_DRIVER_OPEN_MANUAL_RELEASE || event.type == EVT_SECURITY_OPEN_MANUAL_RELEASE
            || event.type == EVT_CONFLICT)
            {
                GateFSM_SetState(STOPPED_MIDWAY);
							printf("FSM: Gate is stopped midway while opening\n");
                //turn off LED
            }
            else if(event.type == EVT_OPEN_LIMIT_PRESS)
            {
                GateFSM_SetState(IDLE_OPEN);
							printf("FSM: Gate is idle open\n");
                //turn off LED
            }
            else if (event.type == EVT_SECURITY_CLOSE_AUTO || event.type == EVT_SECURITY_CLOSE_MANUAL_START)
            {
                GateFSM_SetState(CLOSING);
                printf("FSM: Security override, changing state from opening to closing");
                // turn off green and turn on red LED
            }
            break;
        case CLOSING:
            if(event.type == EVT_DRIVER_CLOSE_MANUAL_RELEASE || event.type == EVT_SECURITY_CLOSE_MANUAL_RELEASE
            || event.type == EVT_CONFLICT)
            {
                GateFSM_SetState(STOPPED_MIDWAY);
							printf("FSM: Gate is stopped midway while closing\n");
                //turn off LED
            }
            else if(event.type == EVT_CLOSED_LIMIT_PRESS)
            {
                GateFSM_SetState(IDLE_CLOSED);
							printf("FSM: Gate is idle closed\n");
                //turn off LED
            }
            else if(event.type == EVT_OBSTACLE_PRESS)
            {
                GateFSM_SetState(REVERSING);
                xSemaphoreGive(xObstacleSemaphore);
							printf("FSM: Obstacle detected, Gate is reversing\n");
                // add event to handle reverse stop (return to STOPPED_MIDWAY) after security task timer stops.
                //turn LED green
            }
            else if (event.type == EVT_SECURITY_OPEN_AUTO || event.type == EVT_SECURITY_OPEN_MANUAL_START)
            {
                GateFSM_SetState(OPENING);
                printf("FSM: Security override, changing state from closing to opening");
                // turn off red and turn on green LED
            }
            break;
        case STOPPED_MIDWAY:
            if(event.type == EVT_DRIVER_OPEN_AUTO || event.type == EVT_SECURITY_OPEN_AUTO 
            || event.type == EVT_DRIVER_OPEN_MANUAL_START || event.type == EVT_SECURITY_OPEN_MANUAL_START)
            {
                GateFSM_SetState(OPENING);
							printf("FSM: Gate is opening from midway\n");
                //turn LED green
            }
            else if(event.type == EVT_DRIVER_CLOSE_AUTO || event.type == EVT_SECURITY_CLOSE_AUTO 
            || event.type == EVT_DRIVER_CLOSE_MANUAL_START || event.type == EVT_SECURITY_CLOSE_MANUAL_START)
            {
                GateFSM_SetState(CLOSING);
							printf("FSM: Gate is closing from midway\n");
                //turn LED red
            }
            break;
        case REVERSING:
            if (event.type == EVT_REVERSE_TIMEOUT)
            {
                GateFSM_SetState(STOPPED_MIDWAY);
							printf("FSM: Gate is stopped midway while reversing\n");
                //turn off LED
            }
            break;
        default:
            break;
    }
 }