/*
 * app_config.h
 *
 * Central configuration for task timing, priorities, and GPIO mapping.
 */

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <stdint.h>

/* FreeRTOS object sizing */
#define GATE_EVENT_QUEUE_LENGTH               (32U)

/* Timing */
#define INPUT_SCAN_PERIOD_MS                  (20U)
#define BUTTON_DEBOUNCE_COUNT                 (2U)     /* 2*20ms = 40ms */
#define TAP_THRESHOLD_MS                      (250U)
#define OBSTACLE_REVERSE_MS                   (500U)
#define STATUS_PRINT_PERIOD_MS                (500U)
#define LED_REFRESH_PERIOD_MS                 (20U)

/* Task priorities (higher number => higher priority) */
#define PRIO_STATUS_TASK                      (1U)
#define PRIO_GATE_CONTROL_TASK                (2U)
#define PRIO_LED_TASK                         (2U)
#define PRIO_INPUT_TASK                       (3U)
#define PRIO_SAFETY_TASK                      (4U)

/* Stack sizing (words) */
#define STACK_INPUT_TASK                      (256U)
#define STACK_GATE_CONTROL_TASK               (256U)
#define STACK_LED_TASK                        (128U)
#define STACK_SAFETY_TASK                     (192U)
#define STACK_STATUS_TASK                     (256U)

/* LED mapping on TM4C123 LaunchPad */
#define LED_PORT                              GPIO_PORTF_DATA_R
#define LED_RED_MASK                          (1U << 1) /* PF1 */
#define LED_GREEN_MASK                        (1U << 3) /* PF3 */

#endif /* APP_CONFIG_H */