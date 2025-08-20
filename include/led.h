#ifndef __LED_H
#define __LED_H

#include "stm32f1xx_hal.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// LED configuration: single LED anode on PA0 (TIM2_CH1)
#define LED_GPIO_PORT GPIOA
#define LED_PIN  GPIO_PIN_0
// LED is active high (MCU pin high turns LED on)
#define LED_ACTIVE_LOW 0

// Initialize LED GPIO
void LED_Init(void);

// Control LED (true = on, false = off) — hides active-low detail
void Board_SetLed(bool on);

// Set specific LED pin to low (on) or high (off)
void Board_SetLedPin(uint16_t pin, bool on);

#ifdef __cplusplus
}
#endif

#endif // __LED_H
