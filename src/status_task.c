/*
 * status_task.c
 *
 * Purpose:
 * - Implements the Status or Debug Task.
 *
 * Responsibilities:
 * - Display or print the current system state.
 * - Help verify system behavior during testing.
 * - Provide debug output through UART or debugger console.
 *
 * Useful information to display:
 * - current gate state
 * - current mode, manual or auto
 * - last received event
 * - safety status
 */
 
#include "FreeRTOS.h"
#include "task.h"


#include  <stdio.h>

#include "status_task.h"
#include "app_config.h"
#include "gate_fsm.h"

static const char* GateStateToString(GateState_t state){

    switch(state){
        case IDLE_OPEN: return "IDLE_OPEN";
        case IDLE_CLOSED: return "IDLE_CLOSED";
        case OPENING: return "OPENING";
        case CLOSING: return "CLOSING";
        case STOPPED_MIDWAY: return "STOPPED_MIDWAY";
        case REVERSING: return "REVERSING";
        default:        return "UNKNOWN";
    }


}




void vStatusTask(void *pvParameters)
{
    (void) pvParameters;

    TickType_t xLastWakeTime = xTaskGetTickCount();
    GateState_t prevState = GateFSM_GetState();
    uint32_t heartBeatCount = 0U;


    printf("[STATUS]: Task started, Initial state: %s\n", GateStateToString(prevState));

    while(1)
    {
        GateState_t currentState = GateFSM_GetState();

        if(currentState != prevState){
            printf("[STATUS] State changed: %s --> %s\n", GateStateToString(prevState), GateStateToString(currentState));
            prevState = currentState;
        }

        heartBeatCount++;
        if(heartBeatCount >= 5U){
            printf("[STATUS] Heartbeat, Current State: %s\n", GateStateToString(currentState));
            heartBeatCount = 0U;
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(STATUS_TASK_PERIOD_MS));


    }
}