/*
 * led_task.c
 *
 * Purpose:
 * - Implements the LED Control Task.
 *
 * Responsibilities:
 * - Read the current gate state.
 * - Control the green LED and red LED.
 *
 * Expected LED behavior:
 * - OPENING: green LED on
 * - CLOSING: red LED on
 * - REVERSING: green LED on
 * - IDLE_OPEN: LEDs off
 * - IDLE_CLOSED: LEDs off
 * - STOPPED_MIDWAY: LEDs off
 */
 
#include "FreeRTOS.h"
#include "task.h"

#include "led_task.h"
#include "board_io.h"
#include "app_config.h"

void vLEDTask(void *pvParameters)
{
    bool ledOn = false;

    (void) pvParameters;

    while(1)
    {
        ledOn = !ledOn;
        Board_SetGreenLED(ledOn);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}