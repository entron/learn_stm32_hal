#ifndef __BOARD_H
#define __BOARD_H

#include "stm32f1xx_hal.h"
#include <stdbool.h>

// Include all component headers
#include "led.h"
#include "keys.h"
#include "buzzer.h"
#include "light_sensor.h"
#include "ir_sensor.h"
#include "servo.h"
#include "motor.h"

#ifdef __cplusplus
extern "C" {
#endif

// Initialize HAL, clocks and minimal board peripherals used by examples
void Board_Init(void);

// I2C handle used by peripherals (e.g. SSD1306 library expects hi2c1)
extern I2C_HandleTypeDef hi2c1;

// Initialize I2C peripheral used for external devices (default I2C1 on PB6/SCL PB7/SDA)
// Adjust pins in implementation if your wiring differs.
void Board_I2C_Init(void);

// Expose error handler (used by HAL callbacks)
void Error_Handler(void);

// System tick and clock setup used internally by the board
void SystemClock_Config(void);
void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif // __BOARD_H
