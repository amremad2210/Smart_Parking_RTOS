/*
 * status_task.c
 */

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "app_config.h"
#include "gate_fsm.h"

void StatusTask(void *pvParameters)
{
    (void)pvParameters;

    for (;;)
    {
        gate_status_t s = GateFSM_GetStatus();
        printf("[Gate] state=%s mode=%s dir=%u src=%u\r\n",
               GateFSM_StateToString(s.state),
               GateFSM_ModeToString(s.mode),
               (unsigned int)s.direction,
               (unsigned int)s.active_source);

        vTaskDelay(pdMS_TO_TICKS(STATUS_PRINT_PERIOD_MS));
    }
}