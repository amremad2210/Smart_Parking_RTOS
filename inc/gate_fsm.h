/*
 * gate_fsm.h
 */

#ifndef GATE_FSM_H
#define GATE_FSM_H

#include "gate_types.h"

void GateFSM_Init(void);
void GateFSM_ProcessEvent(const gate_event_t *evt);
void GateFSM_SetState(gate_state_t newState, gate_mode_t mode, gate_direction_t dir, gate_source_t src);
gate_status_t GateFSM_GetStatus(void);
const char *GateFSM_StateToString(gate_state_t s);
const char *GateFSM_ModeToString(gate_mode_t m);

#endif /* GATE_FSM_H */