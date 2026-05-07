#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/*
 * app_config.h
 *
 * Purpose:
 * - Stores project-wide configuration values.
 *
 * This file contains:
 * - task priorities
 * - task stack sizes
 * - queue length
 * - debounce time
 * - tap/hold timing threshold
 * - obstacle reverse duration
 *
 * GPIO pin assignments are kept in board_io.c because they are
 * hardware-specific.
 */

/*
 * Task priorities
 *
 * Higher number = higher priority in FreeRTOS.
 *
 * Safety Task is highest because obstacle handling must override
 * normal gate movement.
 */
#define SAFETY_TASK_PRIORITY          4
#define INPUT_TASK_PRIORITY           3
#define GATE_CONTROL_TASK_PRIORITY    2
#define LED_TASK_PRIORITY             2
#define STATUS_TASK_PRIORITY          1

/*
 * Task stack sizes
 *
 * These are starting values.
 * Increase later if a task crashes or if more local variables/printing
 * are added.
 */
#define SAFETY_TASK_STACK_SIZE        128
#define INPUT_TASK_STACK_SIZE         128
#define GATE_TASK_STACK_SIZE          128
#define LED_TASK_STACK_SIZE           128
#define STATUS_TASK_STACK_SIZE        128

/*
 * Queue configuration
 *
 * The event queue carries button events from the Input Task
 * to the Gate Control Task.
 */
#define GATE_EVENT_QUEUE_LENGTH       10

/*
 * Timing configuration
 */
#define INPUT_POLL_PERIOD_MS          20
#define LED_TASK_PERIOD_MS            50
#define STATUS_TASK_PERIOD_MS         1000

/*
 * Button debounce time.
 *
 * A button state must remain stable for this long before it is accepted.
 */
#define DEBOUNCE_TIME_MS              20

/*
 * Tap/hold threshold.
 *
 * Press shorter than this can be treated as one-touch auto mode.
 * Press longer than this can be treated as manual hold mode.
 */
#define TAP_TIME_MS                   300

/*
 * Obstacle reverse duration.
 *
 * When obstacle is detected during auto-closing:
 * - stop immediately
 * - reverse/open for this duration
 * - stop midway
 */
#define REVERSE_TIME_MS               500

#endif