/*
 * main.c
 *
 * Purpose:
 * - Entry point of the Smart Parking Gate project.
 * - Initializes the hardware layer.
 * - Creates the FreeRTOS queue, semaphores, and mutex.
 * - Creates all RTOS tasks.
 * - Starts the FreeRTOS scheduler.
 *
 * This file should stay clean.
 * Do not place button logic, FSM logic, LED logic, or safety logic here.
 */


#include "TM4C123GH6PM.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include <basic_io.h>



#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>>

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

#include "board_io.h"

#include "app_config.h"
#include "gate_types.h"
#include "gate_shared.h"
#include "board_io.h"

#include "input_task.h"
#include "gate_control_task.h"
#include "safety_task.h"
#include "led_task.h"
#include "status_task.h"

/*
 * Shared RTOS objects.
 * These are declared as extern in gate_shared.h.
 */
QueueHandle_t xGateEventQueue = NULL;

SemaphoreHandle_t xGateStateMutex = NULL;
SemaphoreHandle_t xOpenLimitSemaphore = NULL;
SemaphoreHandle_t xClosedLimitSemaphore = NULL;
SemaphoreHandle_t xObstacleSemaphore = NULL;

/*
 * Shared gate state.
 * Initial condition: assume gate starts fully closed.
 */
volatile GateState_t gGateState = IDLE_CLOSED;


static void App_CreateRTOSObjects(void)
{
    /*
     * Queue used to send gate events from Input Task
     * to Gate Control Task.
     */
    xGateEventQueue = xQueueCreate(GATE_EVENT_QUEUE_LENGTH,sizeof(GateEvent_t));

    /*
     * Mutex used to protect shared gate state.
     */
    xGateStateMutex = xSemaphoreCreateMutex();

    /*
     * Binary semaphores for urgent events.
     */
    xOpenLimitSemaphore = xSemaphoreCreateBinary();
    xClosedLimitSemaphore = xSemaphoreCreateBinary();
    xObstacleSemaphore = xSemaphoreCreateBinary();

    /*
     * Simple safety check.
     * If any object failed to create, stop here.
     */
    if( (xGateEventQueue == NULL) ||
        (xGateStateMutex == NULL) ||
        (xOpenLimitSemaphore == NULL) ||
        (xClosedLimitSemaphore == NULL) ||
        (xObstacleSemaphore == NULL) )
    {
        while(1)
        {
            /*
             * Object creation failed.
             * Later we can add debug print or LED error indication here.
             */
        }
    }
}

static void App_CreateTasks(void)
{
    xTaskCreate(vInputTask, "Input", INPUT_TASK_STACK_SIZE, NULL, INPUT_TASK_PRIORITY, NULL);
    xTaskCreate(vGateControlTask, "GateCtrl", GATE_TASK_STACK_SIZE, NULL, GATE_CONTROL_TASK_PRIORITY, NULL);
    xTaskCreate(vSafetyTask, "Safety", SAFETY_TASK_STACK_SIZE, NULL, SAFETY_TASK_PRIORITY, NULL);
    xTaskCreate(vLEDTask, "LED", LED_TASK_STACK_SIZE, NULL, LED_TASK_PRIORITY, NULL);
    xTaskCreate(vStatusTask, "Status", STATUS_TASK_STACK_SIZE, NULL, STATUS_TASK_PRIORITY, NULL);
}


int main(void)
{
    /*
     * Initialize GPIO and board hardware.
     */
    Board_Init();

    /*
     * Create RTOS synchronization and communication objects.
     */
    App_CreateRTOSObjects();

    /*
     * Create all tasks.
     */
    App_CreateTasks();

    /*
     * Start FreeRTOS scheduler.
     */
    vTaskStartScheduler();

    /*
     * Should never reach here.
     */
    while(1)
    {
    }
}