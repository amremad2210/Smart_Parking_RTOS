#ifndef INPUT_TASK_H
#define INPUT_TASK_H

#include "FreeRTOS.h"
#include <stdbool.h>

typedef struct {
    bool rawState;
    bool stableState;
    bool previousStableState;
    TickType_t lastDebounceTime;
    TickType_t pressStartTime;
} ButtonState_t;

void vInputTask(void *pvParameters);

#endif