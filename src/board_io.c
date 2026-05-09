#include "board_io.h"
#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h" // Use the header with direct register definitions

void Board_Init(void)
{
    /* 1. Enable clocks for Ports B, D, E, F */
    SYSCTL_RCGCGPIO_R |= 0x3A; // 0x3A = 0011 1010 (Bits for F, E, D, B)
    
    /* 2. Wait for peripherals to be ready */
    while ((SYSCTL_PRGPIO_R & 0x3A) != 0x3A) { }

    /* 3. UNLOCK Port D Pin 0 (Required for Driver CLOSE) */
    GPIO_PORTD_LOCK_R = 0x4C4F434B; // Magic Key
    GPIO_PORTD_CR_R |= 0x01;        // Commit PD0
    GPIO_PORTD_LOCK_R = 0;          // Relock

    /* --- PORT F Configuration (LEDs PF1, PF3 | Obstacle PF4) --- */
    GPIO_PORTF_DIR_R   |=  (1U << 1) | (1U << 3); // PF1, PF3 outputs
    GPIO_PORTF_DIR_R   &= ~(1U << 4);             // PF4 input
    GPIO_PORTF_AFSEL_R &= ~0x1A;                  // Disable alt functions
    GPIO_PORTF_PDR_R   |=  (1U << 4);             // Obstacle is active-high (Pull-down)
    GPIO_PORTF_DEN_R   |=  0x1A;                  // Digital enable PF1, 3, 4

    /* --- PORT E Configuration (Security PE0, PE1) --- */
    GPIO_PORTE_DIR_R   &= ~0x03;                  // Inputs
    GPIO_PORTE_PUR_R   |=  0x03;                  // Active-low (Pull-up)
    GPIO_PORTE_DEN_R   |=  0x03;                  // Digital enable

    /* --- PORT B Configuration (Limits PB0, PB1) --- */
    GPIO_PORTB_DIR_R   &= ~0x03;                  // Inputs
    GPIO_PORTB_PUR_R   |=  0x03;                  // Active-low (Pull-up)
    GPIO_PORTB_DEN_R   |=  0x03;                  // Digital enable

    /* --- PORT D Configuration (Driver PD0, PD1) --- */
    GPIO_PORTD_DIR_R   &= ~0x03;                  // Inputs
    GPIO_PORTD_PUR_R   |=  0x03;                  // Active-low (Pull-up)
    GPIO_PORTD_DEN_R   |=  0x03;                  // Digital enable
}

/* --- Output Logic --- */
void Board_SetGreenLED(bool on) {
    if (on) GPIO_PORTF_DATA_R |= (1U << 3);
    else    GPIO_PORTF_DATA_R &= ~(1U << 3);
}

void Board_SetRedLED(bool on) {
    if (on) GPIO_PORTF_DATA_R |= (1U << 1);
    else    GPIO_PORTF_DATA_R &= ~(1U << 1);
}

/* --- Input Logic (Matching PDF requirements) --- */

// PD1 (Driver OPEN) - Active Low
bool Board_ReadDriverOpen(void)    { return (GPIO_PORTD_DATA_R & (1U << 1)) == 0; }

// PD0 (Driver CLOSE) - Active Low
bool Board_ReadDriverClose(void)   { return (GPIO_PORTD_DATA_R & (1U << 0)) == 0; }

// PE1 (Security OPEN) - Active Low
bool Board_ReadSecurityOpen(void)  { return (GPIO_PORTE_DATA_R & (1U << 1)) == 0; }

// PE0 (Security CLOSE) - Active Low
bool Board_ReadSecurityClose(void) { return (GPIO_PORTE_DATA_R & (1U << 0)) == 0; }

// PB1 (Open Limit) - Active Low
bool Board_ReadOpenLimit(void)     { return (GPIO_PORTB_DATA_R & (1U << 1)) == 0; }

// PB0 (Closed Limit) - Active Low
bool Board_ReadClosedLimit(void)   { return (GPIO_PORTB_DATA_R & (1U << 0)) == 0; }

// PF4 (Obstacle) - Active High (Instruction says Active-High)
bool Board_ReadObstacle(void)      { return (GPIO_PORTF_DATA_R & (1U << 4)) != 0; }