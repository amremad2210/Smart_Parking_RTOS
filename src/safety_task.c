/*
 * safety_task.c
 *
 * Purpose:
 * - Implements the Safety Task.
 *
 * Responsibilities:
 * - Monitor obstacle-related signals.
 * - Enforce obstacle protection.
 * - Override normal gate movement when safety is triggered.
 *
 * Required obstacle behavior:
 * - If obstacle is detected during auto-closing:
 *      - stop immediately
 *      - reverse/open for 0.5 seconds
 *      - stop midway
 *
 * Obstacle should be ignored when not relevant, such as during opening.
 */
 
 
#include "FreeRTOS.h"
#include "task.h"

#include "safety_task.h"

void vSafetyTask(void *pvParameters)
{
    (void) pvParameters;

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}