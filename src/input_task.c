#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "input_task.h"
#include "board_io.h"
#include "gate_shared.h"
#include "gate_types.h"
#include "app_config.h"
#include <stdio.h>
#include <stdbool.h>

/* Forward declarations */
static void ProcessPanelButtons(ButtonState_t *openBtn, ButtonState_t *closeBtn, 
                                GateEventType_t openAuto, GateEventType_t openHold, GateEventType_t openRel,
                                GateEventType_t closeAuto, GateEventType_t closeHold, GateEventType_t closeRel,
                                const char* panelName);

static void ProcessSimpleButton(ButtonState_t *btn, bool currentRaw, 
                                GateEventType_t pressEvt, const char* dbgName, bool showRelease);

static void SendEvent(GateEventType_t type);

void vInputTask(void *pvParameters) {
    static ButtonState_t bDrOpen = {0}, bDrClose = {0}, bSecOpen = {0}, bSecClose = {0};
    static ButtonState_t bLimOpen = {0}, bLimClose = {0}, bObs = {0};

    vTaskDelay(pdMS_TO_TICKS(100));

    /* Sync initial state to prevent false triggers on startup */
    bDrOpen.previousStableState = Board_ReadDriverOpen();
    bDrClose.previousStableState = Board_ReadDriverClose();
    bSecOpen.previousStableState = Board_ReadSecurityOpen();
    bSecClose.previousStableState = Board_ReadSecurityClose();
    bLimOpen.previousStableState = Board_ReadOpenLimit();
    bLimClose.previousStableState = Board_ReadClosedLimit();
    bObs.previousStableState = Board_ReadObstacle();

    if (INPUT_DEBUG) printf("[INPUT] System Ready. 1-click=AUTO, 2-click=HOLD, 3-click=REL\n");

    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        /* Process Driver Panel (Double-Click / Conflict Logic) */
        ProcessPanelButtons(&bDrOpen, &bDrClose, 
                            EVT_DRIVER_OPEN_AUTO, EVT_DRIVER_OPEN_MANUAL_START, EVT_DRIVER_OPEN_MANUAL_RELEASE,
                            EVT_DRIVER_CLOSE_AUTO, EVT_DRIVER_CLOSE_MANUAL_START, EVT_DRIVER_CLOSE_MANUAL_RELEASE,
                            "DRIVER");

        /* Process Security Panel */
        ProcessPanelButtons(&bSecOpen, &bSecClose, 
                            EVT_SECURITY_OPEN_AUTO, EVT_SECURITY_OPEN_MANUAL_START, EVT_SECURITY_OPEN_MANUAL_RELEASE,
                            EVT_SECURITY_CLOSE_AUTO, EVT_SECURITY_CLOSE_MANUAL_START, EVT_SECURITY_CLOSE_MANUAL_RELEASE,
                            "SECURITY");

        /* Process Limit Buttons - showRelease set to false */
        ProcessSimpleButton(&bLimOpen, Board_ReadOpenLimit(), EVT_OPEN_LIMIT_PRESS, "Limit OPEN", false);
        ProcessSimpleButton(&bLimClose, Board_ReadClosedLimit(), EVT_CLOSED_LIMIT_PRESS, "Limit CLSD", false);
        
        /* Process Obstacle - showRelease set to true */
        ProcessSimpleButton(&bObs, Board_ReadObstacle(), EVT_OBSTACLE_PRESS, "Obstacle", true);

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(INPUT_POLL_PERIOD_MS));
    }
}

/**
 * @brief Logic for a Panel (Open + Close pair). 
 * Handles Auto, Manual, and Conflict detection.
 */
static void HandleSinglePanelBtn(ButtonState_t *me, ButtonState_t *other, bool currentRaw,
                                 GateEventType_t autoEvt, GateEventType_t holdEvt, GateEventType_t releaseEvt,
                                 const char* dbgName)
{
    TickType_t now = xTaskGetTickCount();

    if (currentRaw == true && me->previousStableState == false) {
        
        if (other->clickCount > 0 || other->isInManualMode) {
            printf("[INPUT] CONFLICT! %s pressed while other button active. Aborting.\n", dbgName);
            SendEvent(EVT_CONFLICT);
            me->clickCount = 0; me->isInManualMode = false;
            other->clickCount = 0; other->isInManualMode = false;
        }
        else if (me->isInManualMode) {
            printf("[INPUT] %s MANUAL RELEASE detected\n", dbgName);
            SendEvent(releaseEvt);
            me->isInManualMode = false;
            me->clickCount = 0;
        }
        else if (me->clickCount == 0) {
            me->firstClickTime = now;
            me->clickCount = 1;
            if (INPUT_DEBUG) printf("[INPUT] %s Click 1/2...\n", dbgName);
        }
        else if (me->clickCount == 1) {
            if ((now - me->firstClickTime) <= pdMS_TO_TICKS(500)) {
                printf("[INPUT] %s MANUAL HOLD started\n", dbgName);
                SendEvent(holdEvt);
                me->isInManualMode = true;
                me->clickCount = 0;
            }
        }
    }

    if (me->clickCount == 1 && !me->isInManualMode && (now - me->firstClickTime) > pdMS_TO_TICKS(500)) {
        printf("[INPUT] %s AUTO TAP detected\n", dbgName);
        SendEvent(autoEvt);
        me->clickCount = 0;
    }

    me->previousStableState = currentRaw;
}

static void ProcessPanelButtons(ButtonState_t *openBtn, ButtonState_t *closeBtn, 
                                GateEventType_t openAuto, GateEventType_t openHold, GateEventType_t openRel,
                                GateEventType_t closeAuto, GateEventType_t closeHold, GateEventType_t closeRel,
                                const char* panelName)
{
    HandleSinglePanelBtn(openBtn, closeBtn, (panelName[0]=='D' ? Board_ReadDriverOpen() : Board_ReadSecurityOpen()),
                         openAuto, openHold, openRel, panelName);
    
    HandleSinglePanelBtn(closeBtn, openBtn, (panelName[0]=='D' ? Board_ReadDriverClose() : Board_ReadSecurityClose()),
                         closeAuto, closeHold, closeRel, panelName);
}

/**
 * @brief Simple edge detection for Obstacle and Limits.
 * @param showRelease If true, prints "Released" message.
 */
static void ProcessSimpleButton(ButtonState_t *btn, bool currentRaw, 
                                GateEventType_t pressEvt, const char* dbgName, bool showRelease) 
{
    // RISING EDGE
    if (currentRaw == true && btn->previousStableState == false) {
        printf("[INPUT] %s Pressed\n", dbgName);
        SendEvent(pressEvt);
    }
    // FALLING EDGE
    else if (currentRaw == false && btn->previousStableState == true) {
        if (showRelease) {
            printf("[INPUT] %s Released\n", dbgName);
        }
    }
    
    btn->previousStableState = currentRaw;
}

static void SendEvent(GateEventType_t type) {
    if (type == EVT_NONE) return;
    GateEvent_t event = { .type = type };
    
    // Non-blocking send to the shared queue
    if (xQueueSend(xGateEventQueue, &event, 0) != pdPASS) {
        if (INPUT_DEBUG) printf("[INPUT] Warning: Queue full, dropped event %d\n", type);
    }
}