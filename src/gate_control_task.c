/*
 * gate_control_task.c
 */

#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "gate_shared.h"
#include "gate_fsm.h"

typedef struct
{
    bool drvOpenHeld;
    bool drvCloseHeld;
    bool secOpenHeld;
    bool secCloseHeld;
} held_map_t;

static void enforce_held_policy(const held_map_t *h)
{
    /* Same-panel conflicts must always stop safely. */
    if ((h->drvOpenHeld && h->drvCloseHeld) || (h->secOpenHeld && h->secCloseHeld))
    {
        GateFSM_SetState(GATE_STOPPED_MIDWAY, MODE_NONE, DIR_NONE, SRC_SYSTEM);
        return;
    }

    /* Security holds override all driver holds. */
    if (h->secOpenHeld)
    {
        GateFSM_SetState(GATE_OPENING, MODE_MANUAL, DIR_OPEN, SRC_SECURITY);
        return;
    }
    if (h->secCloseHeld)
    {
        GateFSM_SetState(GATE_CLOSING, MODE_MANUAL, DIR_CLOSE, SRC_SECURITY);
        return;
    }

    if (h->drvOpenHeld)
    {
        GateFSM_SetState(GATE_OPENING, MODE_MANUAL, DIR_OPEN, SRC_DRIVER);
        return;
    }
    if (h->drvCloseHeld)
    {
        GateFSM_SetState(GATE_CLOSING, MODE_MANUAL, DIR_CLOSE, SRC_DRIVER);
        return;
    }
}

void GateControlTask(void *pvParameters)
{
    gate_event_t evt;
    held_map_t held = {0};
    (void)pvParameters;

    GateFSM_Init();

    for (;;)
    {
        /* Urgent limit handling by semaphore path */
        if (xSemaphoreTake(gOpenLimitSem, 0U) == pdTRUE)
        {
            gate_event_t e = {EVT_LIMIT_OPEN, SRC_SYSTEM, (uint32_t)xTaskGetTickCount()};
            GateFSM_ProcessEvent(&e);
        }
        if (xSemaphoreTake(gClosedLimitSem, 0U) == pdTRUE)
        {
            gate_event_t e = {EVT_LIMIT_CLOSED, SRC_SYSTEM, (uint32_t)xTaskGetTickCount()};
            GateFSM_ProcessEvent(&e);
        }

        if (xQueueReceive(gGateEventQueue, &evt, pdMS_TO_TICKS(20U)) == pdTRUE)
        {
            switch (evt.type)
            {
                case EVT_CMD_OPEN_HOLD_START:
                    if (evt.source == SRC_SECURITY) { held.secOpenHeld = true; }
                    else { held.drvOpenHeld = true; }
                    enforce_held_policy(&held);
                    break;

                case EVT_CMD_CLOSE_HOLD_START:
                    if (evt.source == SRC_SECURITY) { held.secCloseHeld = true; }
                    else { held.drvCloseHeld = true; }
                    enforce_held_policy(&held);
                    break;

                case EVT_CMD_OPEN_HOLD_RELEASE:
                    if (evt.source == SRC_SECURITY) { held.secOpenHeld = false; }
                    else { held.drvOpenHeld = false; }
                    enforce_held_policy(&held);
                    if (!held.secOpenHeld && !held.secCloseHeld && !held.drvOpenHeld && !held.drvCloseHeld)
                    {
                        GateFSM_ProcessEvent(&evt);
                    }
                    break;

                case EVT_CMD_CLOSE_HOLD_RELEASE:
                    if (evt.source == SRC_SECURITY) { held.secCloseHeld = false; }
                    else { held.drvCloseHeld = false; }
                    enforce_held_policy(&held);
                    if (!held.secOpenHeld && !held.secCloseHeld && !held.drvOpenHeld && !held.drvCloseHeld)
                    {
                        GateFSM_ProcessEvent(&evt);
                    }
                    break;

                case EVT_CMD_OPEN_TAP:
                case EVT_CMD_CLOSE_TAP:
                    /* Security tap has priority if both happen near each other. */
                    if ((evt.source == SRC_DRIVER) && (held.secOpenHeld || held.secCloseHeld))
                    {
                        break;
                    }
                    GateFSM_ProcessEvent(&evt);
                    break;

                case EVT_LIMIT_OPEN:
                case EVT_LIMIT_CLOSED:
                    GateFSM_ProcessEvent(&evt);
                    break;

                case EVT_OBSTACLE:
                case EVT_NONE:
                default:
                    break;
            }
        }
    }
}