/*
 * gate_fsm.c
 *
 * Purpose:
 * - Implements the gate finite state machine.
 *
 * Responsibilities:
 * - Store and update the current gate state.
 * - Protect shared gate state using a mutex.
 * - Process gate events.
 * - Control transitions between:
 *      - IDLE_OPEN
 *      - IDLE_CLOSED
 *      - OPENING
 *      - CLOSING
 *      - STOPPED_MIDWAY
 *      - REVERSING
 *
 * This file should contain the main state-transition rules.
 */