#ifndef STATUS_TASK_H
#define STATUS_TASK_H

/*
 * status_task.h
 *
 * Purpose:
 * - Header for the Status or Debug Task.
 *
 * This file should later declare:
 * - the Status Task function
 * - any public debug/status helper functions if needed
 */

 #include "gate_types.h"


 
 
 void vStatusTask(void *pvParameters);

 void Status_SetLastEvent(GateEventType_t event);
 GateEventType_t Status_GetLastEvent(void);


#endif