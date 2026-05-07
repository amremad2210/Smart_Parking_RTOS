/*
 * gate_types.h
 *
 * Shared enums/structs for the smart gate.
 */

#ifndef GATE_TYPES_H
#define GATE_TYPES_H

#include <stdint.h>

typedef enum
{
    GATE_IDLE_OPEN = 0,
    GATE_IDLE_CLOSED,
    GATE_OPENING,
    GATE_CLOSING,
    GATE_STOPPED_MIDWAY,
    GATE_REVERSING
} gate_state_t;

typedef enum
{
    MODE_NONE = 0,
    MODE_MANUAL,
    MODE_AUTO
} gate_mode_t;

typedef enum
{
    DIR_NONE = 0,
    DIR_OPEN,
    DIR_CLOSE
} gate_direction_t;

typedef enum
{
    SRC_DRIVER = 0,
    SRC_SECURITY,
    SRC_SYSTEM
} gate_source_t;

typedef enum
{
    EVT_NONE = 0,
    EVT_CMD_OPEN_TAP,
    EVT_CMD_CLOSE_TAP,
    EVT_CMD_OPEN_HOLD_START,
    EVT_CMD_CLOSE_HOLD_START,
    EVT_CMD_OPEN_HOLD_RELEASE,
    EVT_CMD_CLOSE_HOLD_RELEASE,
    EVT_LIMIT_OPEN,
    EVT_LIMIT_CLOSED,
    EVT_OBSTACLE
} gate_event_type_t;

typedef struct
{
    gate_event_type_t type;
    gate_source_t source;
    uint32_t tick;
} gate_event_t;

typedef struct
{
    gate_state_t state;
    gate_mode_t mode;
    gate_direction_t direction;
    gate_source_t active_source;
} gate_status_t;

#endif /* GATE_TYPES_H */