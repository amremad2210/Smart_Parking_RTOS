#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/* Task priorities */
#define SAFETY_TASK_PRIORITY          4
#define INPUT_TASK_PRIORITY           3
#define GATE_CONTROL_TASK_PRIORITY    2
#define LED_TASK_PRIORITY             2
#define STATUS_TASK_PRIORITY          1

/* Task stack sizes */
#define INPUT_TASK_STACK_SIZE         256   // Change from 128 to 256
#define GATE_TASK_STACK_SIZE          256
#define SAFETY_TASK_STACK_SIZE        256
#define LED_TASK_STACK_SIZE           256
#define STATUS_TASK_STACK_SIZE        256

/* Queue configuration */
#define GATE_EVENT_QUEUE_LENGTH       10

/* Timing configuration */
#define INPUT_POLL_PERIOD_MS          20
#define LED_TASK_PERIOD_MS            50
#define STATUS_TASK_PERIOD_MS         1000

/* 
 * Debounce time: Instruction requires 50ms 
 */
#define DEBOUNCE_TIME_MS              50
#define TAP_TIME_MS                   300
#define REVERSE_TIME_MS               500

/* Debug Toggles */
#define INPUT_DEBUG                   1

#endif