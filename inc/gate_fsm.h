#ifndef GATE_FSM_H
#define GATE_FSM_H

/*
 * gate_fsm.h
 *
 * Purpose:
 * - Declares functions for the gate finite state machine.
 *
 * The FSM will:
 * - store/update the current gate state
 * - process incoming gate events
 * - handle transitions between IDLE_OPEN, IDLE_CLOSED,
 *   OPENING, CLOSING, STOPPED_MIDWAY, and REVERSING
 */

#include "gate_types.h"

void GateFSM_Init(void);
void GateFSM_ProcessEvent(GateEvent_t event);
GateState_t GateFSM_GetState(void);
void GateFSM_SetState(GateState_t newState);

#endif