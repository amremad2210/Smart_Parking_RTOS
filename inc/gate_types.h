#ifndef GATE_TYPES_H
#define GATE_TYPES_H

#include <stdint.h>

/* Existing Gate States */
typedef enum
{
    IDLE_OPEN = 0,
    IDLE_CLOSED,
    OPENING,
    CLOSING,
    STOPPED_MIDWAY,
    REVERSING
} GateState_t;

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
		EVT_OBSTACLE_RELEASE,
    EVT_CONFLICT,           /* Sent when Open/Close on same panel are < 0.5s apart */
    EVT_REVERSE_TIMEOUT 
} GateEventType_t;

/* Queue Message Structure */
typedef struct
{
    GateEventType_t type;
} GateEvent_t;

#endif /* GATE_TYPES_H */