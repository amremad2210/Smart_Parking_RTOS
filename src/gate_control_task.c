/*
 * gate_control_task.c
 *
 * Purpose:
 * - Implements the Gate Control Task.
 *
 * Responsibilities:
 * - Receive events from the gate event queue.
 * - Pass events to the finite state machine.
 * - Decide whether the gate should open, close, stop, or reverse.
 * - Handle manual mode.
 * - Handle one-touch auto mode.
 * - Handle security priority over driver commands.
 * - Handle conflicting OPEN and CLOSE commands safely.
 * - Update the shared gate state through the FSM layer.
 */
 
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdbool.h>
#include <stdio.h>

#include "gate_control_task.h"
#include "gate_shared.h"
#include "gate_fsm.h"

bool securityInControl = false;


void UpdateSecurityFlag(GateEvent_t event) {
    if (event.type == EVT_SECURITY_OPEN_AUTO || event.type == EVT_SECURITY_OPEN_MANUAL_START
    || event.type == EVT_SECURITY_CLOSE_AUTO || event.type == EVT_SECURITY_CLOSE_MANUAL_START)
    {
        securityInControl = true;
        printf("Security is in control\n");
    }
    else if ((event.type == EVT_SECURITY_OPEN_MANUAL_RELEASE || event.type == EVT_SECURITY_CLOSE_MANUAL_RELEASE
    || event.type == EVT_OPEN_LIMIT_PRESS || event.type == EVT_CLOSED_LIMIT_PRESS || event.type == EVT_OBSTACLE_PRESS) 
    && securityInControl)
    {
        securityInControl = false;
        printf("Clearing security control flag\n");
    }
}

bool isDriverCmd (GateEvent_t event) {
    return event.type == EVT_DRIVER_OPEN_AUTO || event.type == EVT_DRIVER_OPEN_MANUAL_START
    || event.type == EVT_DRIVER_CLOSE_AUTO || event.type == EVT_DRIVER_CLOSE_MANUAL_START
    || event.type == EVT_DRIVER_OPEN_MANUAL_RELEASE || event.type == EVT_DRIVER_CLOSE_MANUAL_RELEASE;
}

void vGateControlTask(void *pvParameters)
{
    (void) pvParameters;
    GateEvent_t event;
    GateFSM_Init();
    while(1)
    {
        if (xSemaphoreTake(xOpenLimitSemaphore, 0) == pdPASS)
        {
            event.type = EVT_OPEN_LIMIT_PRESS;
            UpdateSecurityFlag(event);
            GateFSM_ProcessEvent(event);
        }
        if (xSemaphoreTake(xClosedLimitSemaphore, 0) == pdPASS)
        {
            event.type = EVT_CLOSED_LIMIT_PRESS;
            UpdateSecurityFlag(event);
            GateFSM_ProcessEvent(event);
        }
       if(xQueueReceive(xGateEventQueue, &event, portMAX_DELAY) == pdPASS)
        {
            UpdateSecurityFlag(event);
            if (isDriverCmd(event) && securityInControl)
            {
                printf("Security is in control, ignoring driver command\n");
                continue;
            }
            else
            {
                GateFSM_ProcessEvent(event);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}