/*
 * main.c
 */

#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "app_config.h"
#include "gate_types.h"
#include "gate_shared.h"
#include "board_io.h"
#include "input_task.h"
#include "gate_control_task.h"
#include "led_task.h"
#include "safety_task.h"
#include "status_task.h"

QueueHandle_t gGateEventQueue = 0;
SemaphoreHandle_t gGateStateMutex = 0;
SemaphoreHandle_t gOpenLimitSem = 0;
SemaphoreHandle_t gClosedLimitSem = 0;
SemaphoreHandle_t gObstacleSem = 0;
gate_status_t gGateStatus = {GATE_IDLE_CLOSED, MODE_NONE, DIR_NONE, SRC_SYSTEM};

static bool app_create_rtos_objects(void)
{
    gGateEventQueue = xQueueCreate(GATE_EVENT_QUEUE_LENGTH, sizeof(gate_event_t));
    gGateStateMutex = xSemaphoreCreateMutex();
    gOpenLimitSem = xSemaphoreCreateBinary();
    gClosedLimitSem = xSemaphoreCreateBinary();
    gObstacleSem = xSemaphoreCreateBinary();

    return (gGateEventQueue != 0) &&
           (gGateStateMutex != 0) &&
           (gOpenLimitSem != 0) &&
           (gClosedLimitSem != 0) &&
           (gObstacleSem != 0);
}

int main(void)
{
    BoardIO_Init();
    if (!app_create_rtos_objects())
    {
        while (1)
        {
        }
    }

    (void)xTaskCreate(InputTask, "Input", STACK_INPUT_TASK, 0, PRIO_INPUT_TASK, 0);
    (void)xTaskCreate(GateControlTask, "GateCtrl", STACK_GATE_CONTROL_TASK, 0, PRIO_GATE_CONTROL_TASK, 0);
    (void)xTaskCreate(LedTask, "LED", STACK_LED_TASK, 0, PRIO_LED_TASK, 0);
    (void)xTaskCreate(SafetyTask, "Safety", STACK_SAFETY_TASK, 0, PRIO_SAFETY_TASK, 0);
    (void)xTaskCreate(StatusTask, "Status", STACK_STATUS_TASK, 0, PRIO_STATUS_TASK, 0);

    vTaskStartScheduler();

    while (1)
    {
    }
}


