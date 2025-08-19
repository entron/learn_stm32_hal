#ifndef __BOARD_H
#define __BOARD_H

#include "stm32f1xx_hal.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Initialize HAL, clocks and minimal board peripherals used by examples
void Board_Init(void);


// LED configuration: change these macros to move the LED to another pin
#define LED_GPIO_PORT GPIOA
#define LED_PIN  GPIO_PIN_1 | GPIO_PIN_2
// Set to 1 if the LED is active low (MCU pin low turns LED on)
#define LED_ACTIVE_LOW 1

// Keys/buttons configuration: B11 and B1
#define KEY_GPIO_PORT GPIOB
#define KEY_PINS (GPIO_PIN_11 | GPIO_PIN_1)

// Control LED (true = on, false = off) — hides active-low detail
void Board_SetLed(bool on);

// Set specific LED pin to low (on) or high (off)
void Board_SetLedPin(uint16_t pin, bool on);

// Expose error handler (used by HAL callbacks)
void Error_Handler(void);

// System tick and clock setup used internally by the board
void SystemClock_Config(void);
void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif // __BOARD_H
