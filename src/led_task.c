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
#include "gate_shared.h"
#include "gate_fsm.h"
#include "app_config.h"

void vLEDTask(void *pvParameters)
{
    (void) pvParameters;
    
    GateState_t currentState;
    
    while(1)
    {
        /* Read current gate state safely */
        currentState = GateFSM_GetState();
        
        /* Control LEDs based on state */
        switch(currentState)
        {
            case OPENING:
            case REVERSING:
                Board_SetGreenLED(true);
                Board_SetRedLED(false);
                break;
                
            case CLOSING:
                Board_SetGreenLED(false);
                Board_SetRedLED(true);
                break;
                
            case IDLE_OPEN:
            case IDLE_CLOSED:
            case STOPPED_MIDWAY:
            default:
                Board_SetGreenLED(false);
                Board_SetRedLED(false);
                break;
        }
        
        vTaskDelay(pdMS_TO_TICKS(LED_TASK_PERIOD_MS));
    }
}