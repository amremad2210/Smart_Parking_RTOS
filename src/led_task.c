/*
 * led_task.c
 *
 * Purpose:
 * - Implements the LED Control Task.
 *
 * Responsibilities:
 * - Read the current gate state.
 * - Control the green LED and red LED.
 *
 * Expected LED behavior:
 * - OPENING: green LED on
 * - CLOSING: red LED on
 * - REVERSING: green LED on
 * - IDLE_OPEN: LEDs off
 * - IDLE_CLOSED: LEDs off
 * - STOPPED_MIDWAY: LEDs off
 */