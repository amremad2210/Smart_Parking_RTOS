/*
 * board_io.c
 *
 * Uses TM4C123 register access for simulator-friendly GPIO.
 */

#include "TM4C123GH6PM.h"
#include "board_io.h"

#define BIT(n)                      (1U << (n))

/* Port E: driver/security buttons */
#define DOPEN_MASK                  BIT(0)  /* PE0 */
#define DCLOSE_MASK                 BIT(1)  /* PE1 */
#define SOPEN_MASK                  BIT(2)  /* PE2 */
#define SCLOSE_MASK                 BIT(3)  /* PE3 */

/* Port B: limits + obstacle */
#define LIM_OPEN_MASK               BIT(0)  /* PB0 */
#define LIM_CLOSED_MASK             BIT(1)  /* PB1 */
#define OBSTACLE_MASK               BIT(2)  /* PB2 */

static bool is_pressed(volatile uint32_t *portData, uint32_t mask)
{
    /* Active-low because inputs use internal pull-ups. */
    return (((*portData) & mask) == 0U);
}

void BoardIO_Init(void)
{
    SYSCTL_RCGCGPIO_R |= (BIT(1) | BIT(4) | BIT(5)); /* B, E, F */
    while ((SYSCTL_PRGPIO_R & (BIT(1) | BIT(4) | BIT(5))) != (BIT(1) | BIT(4) | BIT(5)))
    {
    }

    /* Port E input setup */
    GPIO_PORTE_DIR_R &= ~(DOPEN_MASK | DCLOSE_MASK | SOPEN_MASK | SCLOSE_MASK);
    GPIO_PORTE_DEN_R |= (DOPEN_MASK | DCLOSE_MASK | SOPEN_MASK | SCLOSE_MASK);
    GPIO_PORTE_PUR_R |= (DOPEN_MASK | DCLOSE_MASK | SOPEN_MASK | SCLOSE_MASK);

    /* Port B input setup */
    GPIO_PORTB_DIR_R &= ~(LIM_OPEN_MASK | LIM_CLOSED_MASK | OBSTACLE_MASK);
    GPIO_PORTB_DEN_R |= (LIM_OPEN_MASK | LIM_CLOSED_MASK | OBSTACLE_MASK);
    GPIO_PORTB_PUR_R |= (LIM_OPEN_MASK | LIM_CLOSED_MASK | OBSTACLE_MASK);

    /* Port F LEDs (PF1 Red, PF3 Green) */
    GPIO_PORTF_LOCK_R = 0x4C4F434BU;
    GPIO_PORTF_CR_R |= (BIT(1) | BIT(3));
    GPIO_PORTF_DIR_R |= (BIT(1) | BIT(3));
    GPIO_PORTF_DEN_R |= (BIT(1) | BIT(3));
    GPIO_PORTF_DATA_R &= ~(BIT(1) | BIT(3));
}

bool BoardIO_ReadButton(button_id_t id)
{
    switch (id)
    {
        case BTN_DRIVER_OPEN:    return is_pressed(&GPIO_PORTE_DATA_R, DOPEN_MASK);
        case BTN_DRIVER_CLOSE:   return is_pressed(&GPIO_PORTE_DATA_R, DCLOSE_MASK);
        case BTN_SECURITY_OPEN:  return is_pressed(&GPIO_PORTE_DATA_R, SOPEN_MASK);
        case BTN_SECURITY_CLOSE: return is_pressed(&GPIO_PORTE_DATA_R, SCLOSE_MASK);
        case BTN_LIMIT_OPEN:     return is_pressed(&GPIO_PORTB_DATA_R, LIM_OPEN_MASK);
        case BTN_LIMIT_CLOSED:   return is_pressed(&GPIO_PORTB_DATA_R, LIM_CLOSED_MASK);
        case BTN_OBSTACLE:       return is_pressed(&GPIO_PORTB_DATA_R, OBSTACLE_MASK);
        default:                 return false;
    }
}

void BoardIO_SetGreenLed(bool on)
{
    if (on) { GPIO_PORTF_DATA_R |= BIT(3); }
    else { GPIO_PORTF_DATA_R &= ~BIT(3); }
}

void BoardIO_SetRedLed(bool on)
{
    if (on) { GPIO_PORTF_DATA_R |= BIT(1); }
    else { GPIO_PORTF_DATA_R &= ~BIT(1); }
}

void BoardIO_SetLeds(bool greenOn, bool redOn)
{
    BoardIO_SetGreenLed(greenOn);
    BoardIO_SetRedLed(redOn);
}