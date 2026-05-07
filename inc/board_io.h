#ifndef BOARD_IO_H
#define BOARD_IO_H

#include <stdint.h>
#include <stdbool.h>



/*
 * board_io.h
 *
 * Purpose:
 * - Provides the hardware abstraction layer for the Smart Parking Gate project.
 * - Task files should call these functions instead of directly using TivaWare GPIO functions.
 * - This keeps all pin mappings and hardware-specific code inside board_io.c.
 *
 * Simulator pin mapping:
 *
 * Outputs:
 * - Green LED      -> PF3
 * - Red LED        -> PF1
 *
 * Inputs:
 * - Driver OPEN    -> PF4
 * - Driver CLOSE   -> PF0
 * - Security OPEN  -> PE1
 * - Security CLOSE -> PE0
 * - Open Limit     -> PB1
 * - Closed Limit   -> PB0
 * - Obstacle       -> PD1
 * - Spare input    -> PD0
 *
 * Notes:
 * - Most simulator switches are expected to be active-low:
 *      pressed     = logic 0
 *      not pressed = logic 1
 * - The read functions should return true when the button is pressed.
 */

/*
 * Initializes all board-level GPIO used by the project.
 *
 * This should configure:
 * - Port F LEDs
 * - Port F buttons
 * - Port E switch inputs
 * - Port B switch inputs
 * - Port D switch inputs
 */
 
 
 /* LED pins */
#define GREEN_LED_PIN       GPIO_PIN_3
#define RED_LED_PIN         GPIO_PIN_1

/* Port F input pins */
#define DRIVER_OPEN_PIN     GPIO_PIN_4
#define DRIVER_CLOSE_PIN    GPIO_PIN_0

/* Port E input pins */
#define SECURITY_OPEN_PIN   GPIO_PIN_1
#define SECURITY_CLOSE_PIN  GPIO_PIN_0

/* Port B input pins */
#define OPEN_LIMIT_PIN      GPIO_PIN_1
#define CLOSED_LIMIT_PIN    GPIO_PIN_0

/* Port D input pins */
#define OBSTACLE_PIN        GPIO_PIN_1
#define SPARE_INPUT_PIN     GPIO_PIN_0


/*
 * Button polarity:
 *
 * Port F simulator buttons:
 * - PF4 Driver OPEN  = active-high
 * - PF0 Driver CLOSE = active-high
 *
 * All other simulator switches:
 * - PE1, PE0, PB1, PB0, PD1, PD0 = active-low
 */
#define IS_PRESSED_ACTIVE_LOW(port, pin)    ((GPIOPinRead((port), (pin)) & (pin)) == 0)
#define IS_PRESSED_ACTIVE_HIGH(port, pin)   ((GPIOPinRead((port), (pin)) & (pin)) != 0)
 
 
void Board_Init(void);
void Board_SetGreenLED(bool on);
void Board_SetRedLED(bool on);

/*
 * Driver panel input functions.
 */
 
bool Board_ReadDriverOpen(void);
bool Board_ReadDriverClose(void);

/*
 * Security panel input functions.
 */

bool Board_ReadSecurityOpen(void);
bool Board_ReadSecurityClose(void);

/*
 * Gate limit input functions.
 */

bool Board_ReadOpenLimit(void);
bool Board_ReadClosedLimit(void);

/*
 * Obstacle input function.
 */

bool Board_ReadObstacle(void);


/*
 * Optional spare input.
 * This can be used later for testing or debugging.
 */

bool _Board_ReadSpareInput();

 
 
 #endif