/*
 * gate_fsm.c
 */

#include "gate_fsm.h"
#include "gate_shared.h"

static void lock_state(void)
{
    (void)xSemaphoreTake(gGateStateMutex, portMAX_DELAY);
}

static void unlock_state(void)
{
    (void)xSemaphoreGive(gGateStateMutex);
}

void GateFSM_Init(void)
{
    GateFSM_SetState(GATE_IDLE_CLOSED, MODE_NONE, DIR_NONE, SRC_SYSTEM);
}

void GateFSM_SetState(gate_state_t newState, gate_mode_t mode, gate_direction_t dir, gate_source_t src)
{
    lock_state();
    gGateStatus.state = newState;
    gGateStatus.mode = mode;
    gGateStatus.direction = dir;
    gGateStatus.active_source = src;
    unlock_state();
}

gate_status_t GateFSM_GetStatus(void)
{
    gate_status_t snapshot;
    lock_state();
    snapshot = gGateStatus;
    unlock_state();
    return snapshot;
}

static void start_open(gate_mode_t mode, gate_source_t source)
{
    GateFSM_SetState((mode == MODE_AUTO) ? GATE_OPENING : GATE_OPENING, mode, DIR_OPEN, source);
}

static void start_close(gate_mode_t mode, gate_source_t source)
{
    GateFSM_SetState(GATE_CLOSING, mode, DIR_CLOSE, source);
}

void GateFSM_ProcessEvent(const gate_event_t *evt)
{
    gate_status_t s;
    if (evt == 0)
    {
        return;
    }

    s = GateFSM_GetStatus();
    switch (evt->type)
    {
        case EVT_CMD_OPEN_HOLD_START:
            start_open(MODE_MANUAL, evt->source);
            break;

        case EVT_CMD_CLOSE_HOLD_START:
            start_close(MODE_MANUAL, evt->source);
            break;

        case EVT_CMD_OPEN_HOLD_RELEASE:
            if ((s.mode == MODE_MANUAL) && (s.direction == DIR_OPEN))
            {
                GateFSM_SetState(GATE_STOPPED_MIDWAY, MODE_NONE, DIR_NONE, SRC_SYSTEM);
            }
            break;

        case EVT_CMD_CLOSE_HOLD_RELEASE:
            if ((s.mode == MODE_MANUAL) && (s.direction == DIR_CLOSE))
            {
                GateFSM_SetState(GATE_STOPPED_MIDWAY, MODE_NONE, DIR_NONE, SRC_SYSTEM);
            }
            break;

        case EVT_CMD_OPEN_TAP:
            start_open(MODE_AUTO, evt->source);
            break;

        case EVT_CMD_CLOSE_TAP:
            start_close(MODE_AUTO, evt->source);
            break;

        case EVT_LIMIT_OPEN:
            if ((s.state == GATE_OPENING) || (s.state == GATE_REVERSING))
            {
                GateFSM_SetState(GATE_IDLE_OPEN, MODE_NONE, DIR_NONE, SRC_SYSTEM);
            }
            break;

        case EVT_LIMIT_CLOSED:
            if (s.state == GATE_CLOSING)
            {
                GateFSM_SetState(GATE_IDLE_CLOSED, MODE_NONE, DIR_NONE, SRC_SYSTEM);
            }
            break;

        case EVT_OBSTACLE:
        case EVT_NONE:
        default:
            break;
    }
}

const char *GateFSM_StateToString(gate_state_t s)
{
    switch (s)
    {
        case GATE_IDLE_OPEN: return "IDLE_OPEN";
        case GATE_IDLE_CLOSED: return "IDLE_CLOSED";
        case GATE_OPENING: return "OPENING";
        case GATE_CLOSING: return "CLOSING";
        case GATE_STOPPED_MIDWAY: return "STOPPED_MIDWAY";
        case GATE_REVERSING: return "REVERSING";
        default: return "UNKNOWN";
    }
}

const char *GateFSM_ModeToString(gate_mode_t m)
{
    switch (m)
    {
        case MODE_NONE: return "NONE";
        case MODE_MANUAL: return "MANUAL";
        case MODE_AUTO: return "AUTO";
        default: return "UNKNOWN";
    }
}