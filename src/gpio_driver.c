#include "gpio_driver.h"

/*
 * Reads the real-time state of a specific GPIO pin (e.g., for a sensor or button).
 * 
 * Parameters:
 *  - port_base: The base memory address of the GPIO Port (e.g., 0x40020400 for GPIOB)
 *  - pin: The specific pin number to read (0 to 15)
 */
uint8_t gpio_read(uint32_t port_base, uint8_t pin) 
{
    /* 1. Fetch the physical Input Data Register using our volatile macro */
    volatile uint32_t *in_reg = GET_REG(port_base, GPIO_IN_OFFSET);
    
    /* 2. Apply the Bitwise AND (&) Mask.
     * The expression (1 << pin) creates a mask with a '1' only at the target pin.
     * The AND gate forces all other pins to '0', isolating our target.
     * 
     * If the hardware pin is HIGH, the result is greater than 0 (True).
     * If the hardware pin is LOW, the result is exactly 0 (False).
     */
    if (*in_reg & (1 << pin)) 
    {
        return 1; // Sensor/Button is active
    } 
    else 
    {
        return 0; // Sensor/Button is inactive
    }
}