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
    /* Driver */
    EVT_DRIVER_OPEN_AUTO,
    EVT_DRIVER_OPEN_MANUAL_START,
    EVT_DRIVER_OPEN_MANUAL_RELEASE,
    EVT_DRIVER_CLOSE_AUTO,
    EVT_DRIVER_CLOSE_MANUAL_START,
    EVT_DRIVER_CLOSE_MANUAL_RELEASE,
    /* Security */
    EVT_SECURITY_OPEN_AUTO,
    EVT_SECURITY_OPEN_MANUAL_START,
    EVT_SECURITY_OPEN_MANUAL_RELEASE,
    EVT_SECURITY_CLOSE_AUTO,
    EVT_SECURITY_CLOSE_MANUAL_START,
    EVT_SECURITY_CLOSE_MANUAL_RELEASE,
    /* Common */
    EVT_OPEN_LIMIT_PRESS,
    EVT_CLOSED_LIMIT_PRESS,
    EVT_OBSTACLE_PRESS,
    EVT_CONFLICT,           /* Sent when Open/Close on same panel are < 0.5s apart */
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