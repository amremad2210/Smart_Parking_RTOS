/*
 * input_task.c
 */

#include <stdbool.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "app_config.h"
#include "board_io.h"
#include "gate_shared.h"

typedef struct
{
    bool stablePressed;
    uint8_t stableCount;
    TickType_t pressTick;
    bool holdSent;
} btn_tracker_t;

static void send_evt(gate_event_type_t type, gate_source_t src)
{
    gate_event_t evt;
    evt.type = type;
    evt.source = src;
    evt.tick = xTaskGetTickCount();
    (void)xQueueSend(gGateEventQueue, &evt, 0U);
}

static void process_cmd_button(button_id_t id, btn_tracker_t *t, gate_source_t src, bool isOpen)
{
    bool raw = BoardIO_ReadButton(id);
    if (raw == t->stablePressed)
    {
        t->stableCount = 0U;
        return;
    }

    if (++(t->stableCount) < BUTTON_DEBOUNCE_COUNT)
    {
        return;
    }

    t->stableCount = 0U;
    t->stablePressed = raw;

    if (raw)
    {
        t->pressTick = xTaskGetTickCount();
        t->holdSent = false;
    }
    else
    {
        TickType_t dt = xTaskGetTickCount() - t->pressTick;
        if (!t->holdSent && (pdTICKS_TO_MS(dt) < TAP_THRESHOLD_MS))
        {
            send_evt(isOpen ? EVT_CMD_OPEN_TAP : EVT_CMD_CLOSE_TAP, src);
        }
        else
        {
            send_evt(isOpen ? EVT_CMD_OPEN_HOLD_RELEASE : EVT_CMD_CLOSE_HOLD_RELEASE, src);
        }
    }

    if (t->stablePressed && !t->holdSent)
    {
        TickType_t dtHold = xTaskGetTickCount() - t->pressTick;
        if (pdTICKS_TO_MS(dtHold) >= TAP_THRESHOLD_MS)
        {
            t->holdSent = true;
            send_evt(isOpen ? EVT_CMD_OPEN_HOLD_START : EVT_CMD_CLOSE_HOLD_START, src);
        }
    }
}

static void process_limit_or_obstacle(button_id_t id, btn_tracker_t *t, gate_event_type_t evtType, SemaphoreHandle_t sem)
{
    bool raw = BoardIO_ReadButton(id);
    if (raw == t->stablePressed)
    {
        t->stableCount = 0U;
        return;
    }

    if (++(t->stableCount) < BUTTON_DEBOUNCE_COUNT)
    {
        return;
    }

    t->stableCount = 0U;
    t->stablePressed = raw;
    if (raw)
    {
        send_evt(evtType, SRC_SYSTEM);
        (void)xSemaphoreGive(sem);
    }
}

void InputTask(void *pvParameters)
{
    btn_tracker_t tracker[BTN_COUNT] = {0};
    TickType_t nextWake = xTaskGetTickCount();
    (void)pvParameters;

    for (;;)
    {
        process_cmd_button(BTN_DRIVER_OPEN, &tracker[BTN_DRIVER_OPEN], SRC_DRIVER, true);
        process_cmd_button(BTN_DRIVER_CLOSE, &tracker[BTN_DRIVER_CLOSE], SRC_DRIVER, false);
        process_cmd_button(BTN_SECURITY_OPEN, &tracker[BTN_SECURITY_OPEN], SRC_SECURITY, true);
        process_cmd_button(BTN_SECURITY_CLOSE, &tracker[BTN_SECURITY_CLOSE], SRC_SECURITY, false);

        process_limit_or_obstacle(BTN_LIMIT_OPEN, &tracker[BTN_LIMIT_OPEN], EVT_LIMIT_OPEN, gOpenLimitSem);
        process_limit_or_obstacle(BTN_LIMIT_CLOSED, &tracker[BTN_LIMIT_CLOSED], EVT_LIMIT_CLOSED, gClosedLimitSem);
        process_limit_or_obstacle(BTN_OBSTACLE, &tracker[BTN_OBSTACLE], EVT_OBSTACLE, gObstacleSem);

        for (button_id_t id = BTN_DRIVER_OPEN; id <= BTN_SECURITY_CLOSE; id++)
        {
            if (tracker[id].stablePressed && !tracker[id].holdSent)
            {
                TickType_t dtHold = xTaskGetTickCount() - tracker[id].pressTick;
                if (pdTICKS_TO_MS(dtHold) >= TAP_THRESHOLD_MS)
                {
                    tracker[id].holdSent = true;
                    switch (id)
                    {
                        case BTN_DRIVER_OPEN:    send_evt(EVT_CMD_OPEN_HOLD_START, SRC_DRIVER); break;
                        case BTN_DRIVER_CLOSE:   send_evt(EVT_CMD_CLOSE_HOLD_START, SRC_DRIVER); break;
                        case BTN_SECURITY_OPEN:  send_evt(EVT_CMD_OPEN_HOLD_START, SRC_SECURITY); break;
                        case BTN_SECURITY_CLOSE: send_evt(EVT_CMD_CLOSE_HOLD_START, SRC_SECURITY); break;
                        default: break;
                    }
                }
            }
        }

        vTaskDelayUntil(&nextWake, pdMS_TO_TICKS(INPUT_SCAN_PERIOD_MS));
    }
}