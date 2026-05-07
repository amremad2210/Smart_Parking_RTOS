/*
 * led_task.c
 */

#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "app_config.h"
#include "gate_fsm.h"
#include "board_io.h"

void LedTask(void *pvParameters)
{
    (void)pvParameters;

    for (;;)
    {
        gate_status_t s = GateFSM_GetStatus();
        bool green = false;
        bool red = false;

        if ((s.state == GATE_OPENING) || (s.state == GATE_REVERSING))
        {
            green = true;
        }
        else if (s.state == GATE_CLOSING)
        {
            red = true;
        }

        BoardIO_SetLeds(green, red);
        vTaskDelay(pdMS_TO_TICKS(LED_REFRESH_PERIOD_MS));
    }
}