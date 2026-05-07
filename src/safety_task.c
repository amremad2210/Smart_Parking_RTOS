/*
 * safety_task.c
 */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "app_config.h"
#include "gate_shared.h"
#include "gate_fsm.h"

void SafetyTask(void *pvParameters)
{
    (void)pvParameters;

    for (;;)
    {
        if (xSemaphoreTake(gObstacleSem, portMAX_DELAY) == pdTRUE)
        {
            gate_status_t s = GateFSM_GetStatus();
            if ((s.state == GATE_CLOSING) && (s.mode == MODE_AUTO))
            {
                /* Stop -> reverse for 0.5s -> stop midway. */
                GateFSM_SetState(GATE_STOPPED_MIDWAY, MODE_NONE, DIR_NONE, SRC_SYSTEM);
                GateFSM_SetState(GATE_REVERSING, MODE_AUTO, DIR_OPEN, SRC_SYSTEM);
                vTaskDelay(pdMS_TO_TICKS(OBSTACLE_REVERSE_MS));
                GateFSM_SetState(GATE_STOPPED_MIDWAY, MODE_NONE, DIR_NONE, SRC_SYSTEM);
            }
        }
    }
}