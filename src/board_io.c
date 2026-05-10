#include "board_io.h"
#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h" // Standard register definitions

/**
 * @brief Initializes all GPIO peripherals for the Smart Parking System.
 * 
 * Pins Summary:
 * - PF3: Green LED (Output)
 * - PF1: Red LED (Output)
 * - PF4: Obstacle Sensor (Input, PDR)
 * - PD1, PD0: Driver Buttons (Input, PDR)
 * - PE1, PE0: Security Buttons (Input, PDR)
 * - PB1, PB0: Limit Switches (Input, PDR)
 */
void Board_Init(void)
{
    /* 1. Enable clocks for Ports B, D, E, and F */
    /* Port B: bit 1, Port D: bit 3, Port E: bit 4, Port F: bit 5 */
    SYSCTL_RCGCGPIO_R |= 0x3A; 
    
    /* 2. Wait for peripherals to be ready */
    while ((SYSCTL_PRGPIO_R & 0x3A) != 0x3A) { }

    /* 3. UNLOCK Port D Pin 0 (Required for Driver CLOSE) */
    /* PD0 is locked by default as an NMI pin. We must unlock it to use as GPIO. */
    GPIO_PORTD_LOCK_R = 0x4C4F434B; // Unlock Key
    GPIO_PORTD_CR_R |= 0x01;        // Allow changes to PD0
    GPIO_PORTD_LOCK_R = 0;          // Relock

    /* 4. Configure PORT F (LEDs PF1, PF3 | Obstacle PF4) */
    GPIO_PORTF_DIR_R   |=  (1U << 1) | (1U << 3); // PF1, PF3 as Outputs
    GPIO_PORTF_DIR_R   &= ~(1U << 4);             // PF4 as Input
    GPIO_PORTF_AFSEL_R &= ~0x1A;                  // Disable alternate functions
    GPIO_PORTF_AMSEL_R &= ~0x1A;                  // Disable analog mode
    //GPIO_PORTF_PDR_R   |=  (1U << 4);             // Enable Pull-Down Resistor on PF4
    GPIO_PORTF_DEN_R   |=  0x1A;                  // Digital Enable PF1, PF3, PF4

    /* 5. Configure PORT E (Security PE0, PE1) */
    GPIO_PORTE_DIR_R   &= ~0x03;                  // Inputs
    GPIO_PORTE_AMSEL_R &= ~0x03;                  // Disable analog
    GPIO_PORTE_PDR_R   |=  0x03;                  // Enable Pull-Down Resistors
		//GPIO_PORTE_PDR_R &= ~0x03;
    GPIO_PORTE_DEN_R   |=  0x03;                  // Digital Enable

    /* 6. Configure PORT B (Limits PB0, PB1) */
    GPIO_PORTB_DIR_R   &= ~0x03;                  // Inputs
    GPIO_PORTB_AMSEL_R &= ~0x03;                  // Disable analog
    GPIO_PORTB_PDR_R   |=  0x03;                  // Enable Pull-Down Resistors
		//GPIO_PORTB_PDR_R &= ~0x03;
    GPIO_PORTB_DEN_R   |=  0x03;                  // Digital Enable

    /* 7. Configure PORT D (Driver PD0, PD1) */
    GPIO_PORTD_DIR_R   &= ~0x03;                  // Inputs
    GPIO_PORTD_AMSEL_R &= ~0x03;                  // Disable analog
    GPIO_PORTD_PDR_R   |=  0x03;                  // Enable Pull-Down Resistors
		//GPIO_PORTD_PDR_R &= ~0x03;
    GPIO_PORTD_DEN_R   |=  0x03;                  // Digital Enable
		
		/* Driver (PD0, PD1) and Security (PE0, PE1) 
       Set to Standard (no PDR/PUR) so you can manually uncheck boxes */
    //GPIO_PORTD_DIR_R &= ~0x03; GPIO_PORTD_PUR_R &= ~0x03; GPIO_PORTD_PDR_R &= ~0x03; GPIO_PORTD_DEN_R |= 0x03;
   // GPIO_PORTE_DIR_R &= ~0x03; GPIO_PORTE_PUR_R &= ~0x03; GPIO_PORTE_PDR_R &= ~0x03; GPIO_PORTE_DEN_R |= 0x03;

    /* Start with LEDs OFF */
    GPIO_PORTF_DATA_R &= ~((1U << 1) | (1U << 3));
}

/* --- LED Control Functions --- */

void Board_SetGreenLED(bool on) {
    if (on) GPIO_PORTF_DATA_R |= (1U << 3);
    else    GPIO_PORTF_DATA_R &= ~(1U << 3);
}

void Board_SetRedLED(bool on) {
    if (on) GPIO_PORTF_DATA_R |= (1U << 1);
    else    GPIO_PORTF_DATA_R &= ~(1U << 1);
}

/* --- Input Read Functions --- */
/* Logic: PDR forces pin to 0. Checking the box in simulator forces it to 1.
   Therefore, if bit is NOT 0, the button is "Pressed". */

bool Board_ReadDriverOpen(void)    { return (GPIO_PORTD_DATA_R & (1U << 1)) != 0; }
bool Board_ReadDriverClose(void)   { return (GPIO_PORTD_DATA_R & (1U << 0)) != 0; }

bool Board_ReadSecurityOpen(void)  { return (GPIO_PORTE_DATA_R & (1U << 1)) != 0; }
bool Board_ReadSecurityClose(void) { return (GPIO_PORTE_DATA_R & (1U << 0)) != 0; }

bool Board_ReadOpenLimit(void)     { return (GPIO_PORTB_DATA_R & (1U << 1)) != 0; }
bool Board_ReadClosedLimit(void)   { return (GPIO_PORTB_DATA_R & (1U << 0)) != 0; }

bool Board_ReadObstacle(void)      { return (GPIO_PORTF_DATA_R & (1U << 4)) != 0; }