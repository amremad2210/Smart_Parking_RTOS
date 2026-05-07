#ifndef GATE_SHARED_H
#define GATE_SHARED_H

/*
 * gate_shared.h
 *
 * Purpose:
 * - Declares shared FreeRTOS objects and shared state.
 *
 * Shared objects:
 * - gate event queue
 * - gate state mutex
 * - open limit semaphore
 * - closed limit semaphore
 * - obstacle semaphore
 * - shared gate state variable
 */

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include "gate_types.h"

/*
 * Queue used to send button/events from Input Task
 * to Gate Control Task.
 */
extern QueueHandle_t xGateEventQueue;

/*
 * Mutex used to protect the shared gate state.
 */
extern SemaphoreHandle_t xGateStateMutex;

/*
 * Semaphores used for urgent limit and safety signals.
 */
extern SemaphoreHandle_t xOpenLimitSemaphore;
extern SemaphoreHandle_t xClosedLimitSemaphore;
extern SemaphoreHandle_t xObstacleSemaphore;

/*
 * Shared gate state.
 */
extern volatile GateState_t gGateState;

#endif