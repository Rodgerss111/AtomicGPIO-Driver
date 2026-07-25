#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include <stdint.h>

/* 
 * Hardware Memory Offsets (Specific to the microcontroller family, e.g., STM32)
 */
#define GPIO_IN_OFFSET   0x10  /* Offset for Input Data Register (IDR) */
#define GPIO_BSRR_OFFSET 0x18  /* Offset for Bit Set/Reset Register (BSRR) */

/* 
 * MEMORY ACCESS MACRO
 * The 'volatile' keyword strictly forbids the C compiler from caching this address.
 * It forces a physical read/write over the system bus every single time, 
 * ensuring our driver reacts to real-time hardware changes.
 */
#define GET_REG(base, offset) ((volatile uint32_t *)((base) + (offset)))

/* 
 * ATOMIC WRITE MACROS (BSRR Strategy)
 * We use direct assignment (=) instead of OR/AND gates (|=, &=) here.
 * 
 * - SET: Writes the pin bit to the lower 16 bits of BSRR to turn it ON.
 * - CLEAR: Shifts the pin bit 16 spaces left (<< 16u) into the upper half to turn it OFF.
 * Both execute in a single CPU cycle, completely immune to interrupt race conditions.
 */
#define GPIO_SET_PIN(port, pin)     ((port)->BSRR = (pin))
#define GPIO_CLEAR_PIN(port, pin)   ((port)->BSRR = ((pin) << 16u))

/* Function Prototypes */
uint8_t gpio_read(uint32_t port_base, uint8_t pin);

#endif /* GPIO_DRIVER_H */