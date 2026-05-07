/*
 * gate_shared.h
 *
 * Shared RTOS handles and gate status.
 */

#ifndef GATE_SHARED_H
#define GATE_SHARED_H

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "gate_types.h"

extern QueueHandle_t gGateEventQueue;
extern SemaphoreHandle_t gGateStateMutex;
extern SemaphoreHandle_t gOpenLimitSem;
extern SemaphoreHandle_t gClosedLimitSem;
extern SemaphoreHandle_t gObstacleSem;
extern gate_status_t gGateStatus;

#endif /* GATE_SHARED_H */