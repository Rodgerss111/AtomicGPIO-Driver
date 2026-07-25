#include <stdint.h>
#include "gpio_driver.h"

/* 
 * Hardware-Specific Definitions (Example for GPIOB) 
 */
#define GPIOB_BASE   0x40020400 
#define BUTTON_PIN   5
#define GPIO_PIN_0   (1 << 0)  /* Target output pin (e.g., Motor or LED) */

/* 
 * Struct mapping to allow the (port)->BSRR syntax in our macros.
 * The volatile keyword ensures the struct members are never cached.
 */
typedef struct {
    uint32_t DUMMY[6];       /* Padding to reach the BSRR offset at 0x18 */
    volatile uint32_t BSRR;  /* The Bit Set/Reset Register */
} GPIO_TypeDef;

/* Cast the base address to our struct pointer */
#define GPIOB ((GPIO_TypeDef *)GPIOB_BASE)

int main(void)
{
    /* Hardware initialization would normally go here (e.g., enabling clocks) */

    /* Infinite application loop */
    while (1)
    {
        /* 
         * SENSOR INPUT POLLING
         * Wait here infinitely until the physical button on Pin 5 is pressed.
         * Because gpio_read() uses our 'volatile' GET_REG macro, the compiler 
         * will not optimize this loop away. It accurately reads the physical silicon.
         */
        while (gpio_read(GPIOB_BASE, BUTTON_PIN) == 0) 
        {
            // Do nothing, just wait for the HIGH signal
        }

        /* 
         * OUTPUT TRIGGER
         * The button was pressed. Drive Pin 0 HIGH atomically.
         * The hardware safely ignores all other pins on GPIOB.
         */
        GPIO_SET_PIN(GPIOB, GPIO_PIN_0);
        
        /* 
         * Debounce/Delay loop
         * (Using volatile so the compiler doesn't delete the empty loop)
         */
        for (volatile uint32_t i = 0; i < 500000; i++); 
        
        /* 
         * Drive Pin 0 LOW atomically using the upper 16 bits of BSRR.
         */
        GPIO_CLEAR_PIN(GPIOB, GPIO_PIN_0);
    }
}