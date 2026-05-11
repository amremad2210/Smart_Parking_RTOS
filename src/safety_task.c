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
#include "queue.h"
#include <stdio.h>

#include "safety_task.h"
#include "gate_shared.h"
#include "gate_types.h"
#include "app_config.h"

void vSafetyTask(void *pvParameters)
{
    (void) pvParameters;
    GateEvent_t event;

    while(1)
    {
        if (xSemaphoreTake(xObstacleSemaphore, portMAX_DELAY) == pdPASS)
        {
            /* Keep reverse motion active for the required safety interval. */
            vTaskDelay(pdMS_TO_TICKS(REVERSE_TIME_MS));

            event.type = EVT_REVERSE_TIMEOUT;
            if (xQueueSend(xGateEventQueue, &event, 0) != pdPASS)
            {
                printf("[SAFETY] Warning: queue full, reverse timeout dropped\n");
            }
        }
    }
}