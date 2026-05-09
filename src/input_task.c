#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "input_task.h"
#include "board_io.h"
#include "gate_shared.h"
#include "gate_types.h"
#include "app_config.h"
#include <stdio.h>

/* Static Helpers */
static void ProcessButton(ButtonState_t *btn, bool currentRaw, 
                          GateEventType_t pressEvt, GateEventType_t releaseEvt, 
                          const char* dbgName);
static void SendEvent(GateEventType_t type);

void vInputTask(void *pvParameters)
{
    /* Initialize button state structures */
    static ButtonState_t bDrOpen = {0}, bDrClose = {0}, bSecOpen = {0}, bSecClose = {0};
    static ButtonState_t bLimOpen = {0}, bLimClose = {0}, bObs = {0};

    /* 1. Stabilization Delay 
     * Give the simulator 100ms to settle the Pull-up resistors 
     * so the pins are actually High (1) before we read them. */
    vTaskDelay(pdMS_TO_TICKS(100));

    /* 2. Synchronize "Previous State" with "Current Hardware"
     * This "swallows" the initial state so we don't trigger 
     * a false PRESS event on startup. */
    bDrOpen.previousStableState    = Board_ReadDriverOpen();
    bDrOpen.stableState            = bDrOpen.previousStableState;

    bDrClose.previousStableState   = Board_ReadDriverClose();
    bDrClose.stableState           = bDrClose.previousStableState;

    bSecOpen.previousStableState   = Board_ReadSecurityOpen();
    bSecOpen.stableState           = bSecOpen.previousStableState;

    bSecClose.previousStableState  = Board_ReadSecurityClose();
    bSecClose.stableState          = bSecClose.previousStableState;

    bLimOpen.previousStableState   = Board_ReadOpenLimit();
    bLimOpen.stableState           = bLimOpen.previousStableState;

    bLimClose.previousStableState  = Board_ReadClosedLimit();
    bLimClose.stableState          = bLimClose.previousStableState;

    bObs.previousStableState       = Board_ReadObstacle();
    bObs.stableState               = bObs.previousStableState;

    if (INPUT_DEBUG) printf("[INPUT] System Synced. Buttons are idle.\n");

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(INPUT_POLL_PERIOD_MS);

    for (;;)
    {
        /* Now the loop starts, but previousStableState matches current 
           hardware, so no events are sent until a REAL change happens. */
        
        ProcessButton(&bDrOpen, Board_ReadDriverOpen(), 
                      EVT_DRIVER_OPEN_PRESS, EVT_DRIVER_OPEN_RELEASE, "Driver OPEN");
        
        ProcessButton(&bDrClose, Board_ReadDriverClose(), 
                      EVT_DRIVER_CLOSE_PRESS, EVT_DRIVER_CLOSE_RELEASE, "Driver CLOSE");

        ProcessButton(&bSecOpen, Board_ReadSecurityOpen(), 
                      EVT_SECURITY_OPEN_PRESS, EVT_SECURITY_OPEN_RELEASE, "Security OPEN");

        ProcessButton(&bSecClose, Board_ReadSecurityClose(), 
                      EVT_SECURITY_CLOSE_PRESS, EVT_SECURITY_CLOSE_RELEASE, "Security CLOSE");

        ProcessButton(&bLimOpen, Board_ReadOpenLimit(), 
                      EVT_OPEN_LIMIT_PRESS, EVT_NONE, "Open Limit");

        ProcessButton(&bLimClose, Board_ReadClosedLimit(), 
                      EVT_CLOSED_LIMIT_PRESS, EVT_NONE, "Closed Limit");

        ProcessButton(&bObs, Board_ReadObstacle(), 
                      EVT_OBSTACLE_PRESS, EVT_NONE, "Obstacle");

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

static void ProcessButton(ButtonState_t *btn, bool currentRaw, 
                          GateEventType_t pressEvt, GateEventType_t releaseEvt, 
                          const char* dbgName) {
    TickType_t now = xTaskGetTickCount();

    if (currentRaw != btn->rawState) {
        btn->lastDebounceTime = now;
    }
    btn->rawState = currentRaw;

    if ((now - btn->lastDebounceTime) >= pdMS_TO_TICKS(DEBOUNCE_TIME_MS)) {
        btn->stableState = currentRaw;

        if (btn->stableState != btn->previousStableState) {
            if (btn->stableState) {
                btn->pressStartTime = now;
                if (INPUT_DEBUG) printf("[INPUT] %s pressed\n", dbgName);
                if (pressEvt != EVT_NONE) SendEvent(pressEvt);
            } else {
                if (INPUT_DEBUG) printf("[INPUT] %s released\n", dbgName);
                if (releaseEvt != EVT_NONE) SendEvent(releaseEvt);
            }
            btn->previousStableState = btn->stableState;
        }
    }
}

static void SendEvent(GateEventType_t type) {
    GateEvent_t event;
    event.type = type;

    /* xGateEventQueue is an extern from gate_shared.h */
    if (xQueueSend(xGateEventQueue, &event, 0) != pdPASS) {
        if (INPUT_DEBUG) printf("[INPUT] Warning: Event queue full!\n");
    }
}