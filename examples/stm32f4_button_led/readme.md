# Button → LED Atomic GPIO Demo (STM32F4)

This example shows the **AtomicGPIO-Driver** in action on any STM32F4 board (e.g., Nucleo-F401RE, Discovery, Black Pill).  
A push button on **PB5** controls an LED on **PB0** using **race‑condition‑free atomic writes** to the BSRR register.

## What You’ll See

- Press the button → LED turns **on**.
- Release the button → LED turns **off** after a short delay.
- The pin transitions are **glitch‑free** because we never use read‑modify‑write (`|=` / `&=`) on the output.

## Wiring

| STM32 Pin | Function   | External Connection                       |
|-----------|------------|-------------------------------------------|
| PB5       | Input      | Push button to GND (with pull‑up enabled*) |
| PB0       | Output     | LED + resistor (220 Ω) → GND              |

> \* The code assumes the button pulls the pin **HIGH** when pressed.  
> If your board has a button connected to GND, enable the internal pull‑up in your hardware init (not shown in the minimal demo) or invert the logic.

If using a Nucleo board:  
- User button (B1) is on **PC13**, not PB5. You can change `BUTTON_PIN` to `13` and `GPIOB_BASE` to `GPIOC_BASE`.  
- The green LED (LD2) is on **PA5**. Adjust `GPIOB` and `GPIO_PIN_0` accordingly.

## Build & Run

### With Make
1. Ensure your ARM GCC toolchain is in PATH.
2. Edit `Makefile` to match your MCU (e.g., `STM32F401xE`).
3. Run:
   ```bash
   make
   make flash   # uses st-flash, openocd, or your debug probe

## With PlatformIO
Copy the src/ and include/ folders into a PlatformIO project.

Set board = genericSTM32F401RE (or your board) in platformio.ini.

Build and upload with pio run -t upload.

## Code Walkthrough

// Wait for button press (reads volatile IDR every iteration)
while (gpio_read(GPIOB_BASE, BUTTON_PIN) == 0);

// Atomic set: write bit 0 to lower BSRR → PB0 goes HIGH
GPIO_SET_PIN(GPIOB, GPIO_PIN_0);

// Software debounce delay (volatile loop so it’s not optimised away)
for (volatile uint32_t i = 0; i < 500000; i++);

// Atomic clear: write bit 0 to upper BSRR → PB0 goes LOW
GPIO_CLEAR_PIN(GPIOB, GPIO_PIN_0);
No interrupt masking needed – BSRR writes are single STR instructions.

The volatile loop ensures the compiler never removes the delay.

The button polling loop never optimises into an infinite empty loop because gpio_read() forces a real hardware read.

Prove It’s Atomic
Connect a logic analyser to PB0 and PB1 (any other output pin).
In a conventional driver, toggling PB0 with |= could corrupt PB1 if an interrupt fires in between.
With our BSRR writes, you will see clean, simultaneous transitions on all pins you control – no glitches.