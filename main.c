/*
 * main.c
 *
 * Purpose:
 * - Entry point of the Smart Parking Gate project.
 * - Initializes the hardware layer.
 * - Creates the FreeRTOS queue, semaphores, and mutex.
 * - Creates all RTOS tasks.
 * - Starts the FreeRTOS scheduler.
 *
 * This file should stay clean.
 * Do not place button logic, FSM logic, LED logic, or safety logic here.
 */


#include "TM4C123GH6PM.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <basic_io.h>
#include <stdio.h>


#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

#include "board_io.h"





int main(void)
{
    bool driverOpen;
    bool driverClose;

    Board_Init();

    while(1)
    {
        driverOpen = Board_ReadDriverOpen();
        driverClose = Board_ReadDriverClose();

        Board_SetGreenLED(driverOpen);
        Board_SetRedLED(driverClose);
    }
}
