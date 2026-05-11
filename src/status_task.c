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
#include "semphr.h"


#include  <stdio.h>

#include "status_task.h"
#include "app_config.h"
#include "gate_fsm.h"
#include "gate_control_task.h"


static volatile GateEventType_t gLastEvent = EVT_NONE;
static SemaphoreHandle_t xStatusMutex = NULL;

static void Status_EnsureInit(void)
{
    if (xStatusMutex == NULL)
    {
        xStatusMutex = xSemaphoreCreateMutex();
    }
}

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

static const char* GateEventToString(GateEventType_t evt)
{
    switch (evt)
    {
        case EVT_NONE: return "EVT_NONE";
        case EVT_DRIVER_OPEN_AUTO: return "EVT_DRIVER_OPEN_AUTO";
        case EVT_DRIVER_OPEN_MANUAL_START: return "EVT_DRIVER_OPEN_MANUAL_START";
        case EVT_DRIVER_OPEN_MANUAL_RELEASE: return "EVT_DRIVER_OPEN_MANUAL_RELEASE";
        case EVT_DRIVER_CLOSE_AUTO: return "EVT_DRIVER_CLOSE_AUTO";
        case EVT_DRIVER_CLOSE_MANUAL_START: return "EVT_DRIVER_CLOSE_MANUAL_START";
        case EVT_DRIVER_CLOSE_MANUAL_RELEASE: return "EVT_DRIVER_CLOSE_MANUAL_RELEASE";
        case EVT_SECURITY_OPEN_AUTO: return "EVT_SECURITY_OPEN_AUTO";
        case EVT_SECURITY_OPEN_MANUAL_START: return "EVT_SECURITY_OPEN_MANUAL_START";
        case EVT_SECURITY_OPEN_MANUAL_RELEASE: return "EVT_SECURITY_OPEN_MANUAL_RELEASE";
        case EVT_SECURITY_CLOSE_AUTO: return "EVT_SECURITY_CLOSE_AUTO";
        case EVT_SECURITY_CLOSE_MANUAL_START: return "EVT_SECURITY_CLOSE_MANUAL_START";
        case EVT_SECURITY_CLOSE_MANUAL_RELEASE: return "EVT_SECURITY_CLOSE_MANUAL_RELEASE";
        case EVT_OPEN_LIMIT_PRESS: return "EVT_OPEN_LIMIT_PRESS";
        case EVT_CLOSED_LIMIT_PRESS: return "EVT_CLOSED_LIMIT_PRESS";
        case EVT_OBSTACLE_PRESS: return "EVT_OBSTACLE_PRESS";
        case EVT_CONFLICT: return "EVT_CONFLICT";
        case EVT_REVERSE_TIMEOUT: return "EVT_REVERSE_TIMEOUT";
        default: return "EVT_UNKNOWN";
    }
}




void vStatusTask(void *pvParameters)
{
    (void) pvParameters;
    Status_EnsureInit();

    TickType_t xLastWakeTime = xTaskGetTickCount();
    GateState_t prevState = GateFSM_GetState();
    uint32_t heartBeatCount = 0U;

    printf("[STATUS][%10lu] START      state=%-14s\n",
           (unsigned long)xLastWakeTime,
           GateStateToString(prevState));

    while (1)
    {
        GateState_t currentState = GateFSM_GetState();
        TickType_t now = xTaskGetTickCount();
        GateEventType_t lastEvt = Status_GetLastEvent();

        if (currentState != prevState)
        {
            printf("[STATUS][%10lu] TRANSITION %-14s -> %-14s | evt=%-32s\n",
                   (unsigned long)now,
                   GateStateToString(prevState),
                   GateStateToString(currentState),
                   GateEventToString(lastEvt));

            prevState = currentState;
            heartBeatCount = 0U;
        }
        else
        {
            heartBeatCount++;
            if (heartBeatCount >= 5U)
            {
                printf("[STATUS][%10lu] HEARTBEAT  state=%-14s | evt=%-32s\n",
                       (unsigned long)now,
                       GateStateToString(currentState),
                       GateEventToString(lastEvt));
                heartBeatCount = 0U;
            }
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(STATUS_TASK_PERIOD_MS));
    }
}


void Status_SetLastEvent(GateEventType_t event)
{
    Status_EnsureInit();
    if (xStatusMutex != NULL)
    {
        xSemaphoreTake(xStatusMutex, portMAX_DELAY);
        gLastEvent = event;
        xSemaphoreGive(xStatusMutex);
    }
}

GateEventType_t Status_GetLastEvent(void)
{
    GateEventType_t evt = EVT_NONE;
    Status_EnsureInit();
    if (xStatusMutex != NULL)
    {
        xSemaphoreTake(xStatusMutex, portMAX_DELAY);
        evt = gLastEvent;
        xSemaphoreGive(xStatusMutex);
    }
    return evt;
}