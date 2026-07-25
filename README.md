# AtomicGPIO-Driver

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C Standard](https://img.shields.io/badge/C-C99-blue.svg)]()

**A reusable, zero‑overhead GPIO driver for bare‑metal ARM microcontrollers (STM32, etc.) that guarantees atomic pin writes using the BSRR register – all while respecting memory volatility and hardware reality.**

> *"Good drivers don't lie to the compiler. Great drivers force the compiler to talk to the hardware every time."*

---

## Why This Driver Is Special

Most GPIO libraries use `|=` and `&=` to set/clear pins.  
**Problem:** A read‑modify‑write sequence is not atomic – an interrupt between the read and write can corrupt the pin state.

**Our solution:** We hit the **BSRR (Bit Set/Reset Register)** directly:
- Write a `1` to the lower 16 bits → pin goes HIGH.
- Write a `1` to the upper 16 bits (i.e., `pin << 16`) → pin goes LOW.
- These writes are **single STR instructions**, immune to IRQ races.
- No bit‑banding regions needed – works on any Cortex‑M with BSRR.

We also:
- Use `volatile` register pointers to **defeat compiler optimisations** (no infinite‑loop removal, no cached values).
- Abstract hardware offsets via macros, making the driver instantly portable across GPIO ports and MCU families.

---

## 📂 Project Structure

AtomicGPIO-Driver/
├── include/
│ └── gpio_driver.h # Macros & function prototypes
├── src/
│ └── gpio_driver.c # gpio_read() implementation
├── examples/
│ └── stm32f4_button_led/
│ ├── main.c # Full demo (button → LED)
│ └── README.md

---

## 🚀 Quick Start

### 1️⃣ Add the driver to your project

Copy `gpio_driver.h` and `gpio_driver.c` into your source tree and add them to your build.

### 2️⃣ Define your GPIO base address and pin mapping

In your own header or `main.c`:

```c
#define GPIOB_BASE   0x40020400    // Example: STM32F4 GPIOB
#define BUTTON_PIN   5
#define GPIO_PIN_0   (1 << 0)

### 3️Use the API
#include "gpio_driver.h"

// Atomic set / clear (no IRQ masking needed)
GPIO_SET_PIN(GPIOB, GPIO_PIN_0);   // Pin 0 HIGH
GPIO_CLEAR_PIN(GPIOB, GPIO_PIN_0); // Pin 0 LOW

// Read a pin state (button, sensor)
uint8_t state = gpio_read(GPIOB_BASE, BUTTON_PIN);


### 4️ Run the provided example

The examples/stm32f4_button_led/main.c shows a complete, loop‑back demo:

Push button on PB5 → LED on PB0 lights for ~500k cycle delay.

The button polling uses gpio_read() which internally reads the volatile IDR.

The LED control uses GPIO_SET_PIN / GPIO_CLEAR_PIN – no read‑modify‑write.

