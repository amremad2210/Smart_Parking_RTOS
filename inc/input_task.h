#ifndef INPUT_TASK_H
#define INPUT_TASK_H

#include "FreeRTOS.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool rawState;
    bool stableState;
    bool previousStableState;
    TickType_t lastDebounceTime;
    
    TickType_t pressStartTime;    // For 0.1s auto-uncheck
    TickType_t firstClickTime;    // For 0.5s Tap/Hold window
    uint8_t clickCount;           // 0: Idle, 1: First click detected
    bool isInManualMode;          // True if we are currently "Holding"
    
    uint32_t portBase;
    uint8_t pinMask;
} ButtonState_t;

void vInputTask(void *pvParameters);

#endif