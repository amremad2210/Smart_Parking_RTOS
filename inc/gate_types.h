#ifndef GATE_TYPES_H
#define GATE_TYPES_H

/*
 * gate_types.h
 *
 * Purpose:
 * - Shared state and event types for the whole project.
 * - No hardware code here.
 * - No FreeRTOS objects here.
 */

/*
 * Required gate states from the project state machine.
 */
typedef enum
{
    IDLE_OPEN = 0,
    IDLE_CLOSED,
    OPENING,
    CLOSING,
    STOPPED_MIDWAY,
    REVERSING
} GateState_t;

/*
 * Events that the Input Task will eventually send to the Gate Control Task.
 */
typedef enum
{
    EVT_NONE = 0,

    EVT_DRIVER_OPEN_PRESS,
    EVT_DRIVER_OPEN_RELEASE,
    EVT_DRIVER_CLOSE_PRESS,
    EVT_DRIVER_CLOSE_RELEASE,

    EVT_SECURITY_OPEN_PRESS,
    EVT_SECURITY_OPEN_RELEASE,
    EVT_SECURITY_CLOSE_PRESS,
    EVT_SECURITY_CLOSE_RELEASE,

    EVT_OPEN_LIMIT_PRESS,
    EVT_CLOSED_LIMIT_PRESS,

    EVT_OBSTACLE_PRESS,

    EVT_REVERSE_TIMEOUT
} GateEventType_t;

/*
 * Queue message structure.
 */
typedef struct
{
    GateEventType_t type;
} GateEvent_t;

#endif