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

#include "gate_control_task.h"
#include "gate_shared.h"
#include "gate_fsm.h"

void vGateControlTask(void *pvParameters)
{
    (void) pvParameters;
    GateEvent_t event;
    GateFSM_Init();
    while(1)
    {
       if(xQueueReceive(xGateEventQueue, &event, portMAX_DELAY) == pdPASS)
        {
            GateFSM_ProcessEvent(event);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}