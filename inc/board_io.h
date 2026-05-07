/*
 * board_io.h
 */

#ifndef BOARD_IO_H
#define BOARD_IO_H

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    BTN_DRIVER_OPEN = 0,
    BTN_DRIVER_CLOSE,
    BTN_SECURITY_OPEN,
    BTN_SECURITY_CLOSE,
    BTN_LIMIT_OPEN,
    BTN_LIMIT_CLOSED,
    BTN_OBSTACLE,
    BTN_COUNT
} button_id_t;

void BoardIO_Init(void);
bool BoardIO_ReadButton(button_id_t id);
void BoardIO_SetGreenLed(bool on);
void BoardIO_SetRedLed(bool on);
void BoardIO_SetLeds(bool greenOn, bool redOn);

#endif /* BOARD_IO_H */