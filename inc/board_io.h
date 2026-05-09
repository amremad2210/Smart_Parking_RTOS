#ifndef BOARD_IO_H
#define BOARD_IO_H

#include <stdint.h>
#include <stdbool.h>

/* LED pins */
#define GREEN_LED_PIN       (1 << 3) // PF3
#define RED_LED_PIN         (1 << 1) // PF1

/* Required Pin Mapping Table */
#define OBSTACLE_PIN        (1 << 4) // PF4
#define SECURITY_CLOSE_PIN  (1 << 0) // PE0
#define SECURITY_OPEN_PIN   (1 << 1) // PE1
#define CLOSED_LIMIT_PIN    (1 << 0) // PB0
#define OPEN_LIMIT_PIN      (1 << 1) // PB1
#define DRIVER_CLOSE_PIN    (1 << 0) // PD0
#define DRIVER_OPEN_PIN     (1 << 1) // PD1

/* 
 * Helper macros for polarity 
 * Returns true if logic matches 'pressed' state
 */
#define READ_ACTIVE_LOW(val, mask)  (((val) & (mask)) == 0)
#define READ_ACTIVE_HIGH(val, mask) (((val) & (mask)) != 0)

void Board_Init(void);
void Board_SetGreenLED(bool on);
void Board_SetRedLED(bool on);

bool Board_ReadDriverOpen(void);
bool Board_ReadDriverClose(void);
bool Board_ReadSecurityOpen(void);
bool Board_ReadSecurityClose(void);
bool Board_ReadOpenLimit(void);
bool Board_ReadClosedLimit(void);
bool Board_ReadObstacle(void);

#endif