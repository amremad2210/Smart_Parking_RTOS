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

#include "status_task.h"
#include "app_config.h"

void vStatusTask(void *pvParameters)
{
    (void) pvParameters;

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(STATUS_TASK_PERIOD_MS));
    }
}