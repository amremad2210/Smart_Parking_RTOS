/*
 * board_io.c
 *
 * Purpose:
 * - Implements board-level input/output functions.
 *
 * Responsibilities:
 * - Initialize GPIO ports.
 * - Configure button pins.
 * - Configure LED pins.
 * - Read raw button states.
 * - Control raw LED outputs.
 *
 * This should be the only file that directly accesses GPIO registers.
 */
 
 
#include "board_io.h"

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"


static void Board_UnlockPortF(void)
{
    /*
     * Port F register addresses:
     *
     * GPIO_PORTF_BASE = 0x40025000
     * GPIOLOCK offset = 0x520
     * GPIOCR offset   = 0x524
     *
     * PF0 must be unlocked because it is protected by default.
     */

    volatile uint32_t *GPIO_PORTF_LOCK_R = (volatile uint32_t *)(0x40025000 + 0x520);
    volatile uint32_t *GPIO_PORTF_CR_R   = (volatile uint32_t *)(0x40025000 + 0x524);

    /*
     * Unlock key for GPIOLOCK register.
     */
    *GPIO_PORTF_LOCK_R = 0x4C4F434B;

    /*
     * Allow changes to PF0.
     */
    *GPIO_PORTF_CR_R |= 0x01;

    /*
     * Lock the port again.
     */
    *GPIO_PORTF_LOCK_R = 0;
}


void Board_Init(){

	//System clock is kept as default 16 MHz
	
	/*
   * Enable GPIO clocks.
   */
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	
	/*
   * Wait for peripherals to be ready
   */
	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){}
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE)){}
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB)){}
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD)){}

		
	
    /*
     * Unlock PF0.
     *
     * PF0 is locked by default because it can be used as NMI.
     * We unlock it so it can be used as Driver CLOSE input.
     */
    
		Board_UnlockPortF();
		
		
		
    /*
     * Configure LEDs as outputs.
     *
     * PF3 = Green LED
     * PF1 = Red LED
     */
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GREEN_LED_PIN | RED_LED_PIN);

    /*
     * Turn both LEDs off initially.
     */
    GPIOPinWrite(GPIO_PORTF_BASE, GREEN_LED_PIN | RED_LED_PIN, 0);

    /*
     * Configure Driver buttons on Port F.
     *
     * PF4 = Driver OPEN
     * PF0 = Driver CLOSE
     *
     * These are active-high in your simulator,
     * so use internal pull-down resistors.
     */
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, DRIVER_OPEN_PIN | DRIVER_CLOSE_PIN);

    GPIOPadConfigSet(
        GPIO_PORTF_BASE,
        DRIVER_OPEN_PIN | DRIVER_CLOSE_PIN,
        GPIO_STRENGTH_2MA,
        GPIO_PIN_TYPE_STD_WPU
    );

    /*
     * Configure Security buttons on Port E.
     *
     * PE1 = Security OPEN
     * PE0 = Security CLOSE
     *
     * These are active-low,
     * so use internal pull-up resistors.
     */
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, SECURITY_OPEN_PIN | SECURITY_CLOSE_PIN);

    GPIOPadConfigSet(
        GPIO_PORTE_BASE,
        SECURITY_OPEN_PIN | SECURITY_CLOSE_PIN,
        GPIO_STRENGTH_2MA,
        GPIO_PIN_TYPE_STD_WPU
    );

    /*
     * Configure Limit buttons on Port B.
     *
     * PB1 = Open Limit
     * PB0 = Closed Limit
     *
     * These are active-low,
     * so use internal pull-up resistors.
     */
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, OPEN_LIMIT_PIN | CLOSED_LIMIT_PIN);

    GPIOPadConfigSet(
        GPIO_PORTB_BASE,
        OPEN_LIMIT_PIN | CLOSED_LIMIT_PIN,
        GPIO_STRENGTH_2MA,
        GPIO_PIN_TYPE_STD_WPU
    );

    /*
     * Configure Obstacle and Spare inputs on Port D.
     *
     * PD1 = Obstacle
     * PD0 = Spare input
     *
     * These are active-low,
     * so use internal pull-up resistors.
     */
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, OBSTACLE_PIN | SPARE_INPUT_PIN);

    GPIOPadConfigSet(
        GPIO_PORTD_BASE,
        OBSTACLE_PIN | SPARE_INPUT_PIN,
        GPIO_STRENGTH_2MA,
        GPIO_PIN_TYPE_STD_WPU
    );
}


void Board_SetGreenLED(bool on)
{
    if(on)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GREEN_LED_PIN, GREEN_LED_PIN);
    }
    else
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GREEN_LED_PIN, 0);
    }
}


void Board_SetRedLED(bool on)
{
    if(on)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, RED_LED_PIN, RED_LED_PIN);
    }
    else
    {
        GPIOPinWrite(GPIO_PORTF_BASE, RED_LED_PIN, 0);
    }
}


bool Board_ReadDriverOpen(void)
{
    return IS_PRESSED_ACTIVE_LOW(GPIO_PORTF_BASE, DRIVER_OPEN_PIN);
}

bool Board_ReadDriverClose(void)
{
    return IS_PRESSED_ACTIVE_LOW(GPIO_PORTF_BASE, DRIVER_CLOSE_PIN);
}

bool Board_ReadSecurityOpen(void)
{
    return IS_PRESSED_ACTIVE_LOW(GPIO_PORTE_BASE, SECURITY_OPEN_PIN);
}


bool Board_ReadSecurityClose(void)
{
    return IS_PRESSED_ACTIVE_LOW(GPIO_PORTE_BASE, SECURITY_CLOSE_PIN);
}


bool Board_ReadOpenLimit(void)
{
    return IS_PRESSED_ACTIVE_LOW(GPIO_PORTB_BASE, OPEN_LIMIT_PIN);
}


bool Board_ReadClosedLimit(void)
{
    return IS_PRESSED_ACTIVE_LOW(GPIO_PORTB_BASE, CLOSED_LIMIT_PIN);
}


bool Board_ReadObstacle(void)
{
    return IS_PRESSED_ACTIVE_LOW(GPIO_PORTD_BASE, OBSTACLE_PIN);
}


bool Board_ReadSpareInput(void)
{
    return IS_PRESSED_ACTIVE_LOW(GPIO_PORTD_BASE, SPARE_INPUT_PIN);
}



