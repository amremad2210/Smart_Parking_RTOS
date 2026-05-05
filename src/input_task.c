/*
 * input_task.c
 *
 * Purpose:
 * - Implements the Input Task.
 *
 * Responsibilities:
 * - Read all button inputs.
 * - Debounce all buttons.
 * - Detect button press events.
 * - Detect button release events.
 * - Detect short press versus held press.
 * - Convert button activity into gate events.
 * - Send events to the gate event queue.
 *
 * Inputs handled:
 * - Driver OPEN
 * - Driver CLOSE
 * - Security OPEN
 * - Security CLOSE
 * - Open limit
 * - Closed limit
 * - Obstacle
 */