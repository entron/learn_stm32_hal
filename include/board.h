#ifndef __BOARD_H
#define __BOARD_H

#include "stm32f1xx_hal.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Initialize HAL, clocks and minimal board peripherals used by examples
void Board_Init(void);


// LED configuration: single LED anode on PA0 (TIM2_CH1)
#define LED_GPIO_PORT GPIOA
#define LED_PIN  GPIO_PIN_0
// LED is active high (MCU pin high turns LED on)
#define LED_ACTIVE_LOW 0

// Keys/buttons configuration: B11 and B1
#define KEY_GPIO_PORT GPIOB
#define KEY_PINS (GPIO_PIN_11 | GPIO_PIN_1)

// Buzzer configuration: PB12
#define BUZZER_GPIO_PORT GPIOB
#define BUZZER_PIN GPIO_PIN_12
// Set to 1 if the buzzer is active low (MCU pin low turns buzzer on)
#define BUZZER_ACTIVE_LOW 1

// Control LED (true = on, false = off) — hides active-low detail
void Board_SetLed(bool on);

// Set specific LED pin to low (on) or high (off)
void Board_SetLedPin(uint16_t pin, bool on);

// Buzzer control
// Initialize buzzer GPIO (call from Board_Init)
void Board_Buzzer_Init(void);
// Control buzzer (true = on, false = off)
void Board_Buzzer_Set(bool on);

// --- Servo (SG90) on PA1 (TIM2_CH2) ---
// Initialize TIM2 CH2 to generate 50 Hz PWM for hobby servos.
// Uses 1 MHz timer tick (1 tick = 1 us), period 20 ms (ARR = 19999).
void Servo_Init(void);
// Set pulse width in microseconds (typical 1000..2000 us). Values are clamped.
void Servo_WriteMicroseconds(uint16_t us);

// Servo pin definitions (use macros for consistency with other peripherals)
#define SERVO_GPIO_PORT GPIOA
#define SERVO_PIN GPIO_PIN_1
#define SERVO_TIMER_CHANNEL TIM_CHANNEL_2
// Servo pulse width limits (microseconds). Tweak these to match your SG90.
#define SERVO_US_MIN 500   // minimum pulse width in us (safe lower bound)
#define SERVO_US_MAX 2500  // maximum pulse width in us (safe upper bound)
#define SERVO_US_NEUTRAL 1500 // neutral/center pulse in us
// Servo PWM timer settings
// SERVO_PWM_PSC is the value used as the prescaler (timer PSC register will be PSC-1)
// SERVO_PWM_ARR is the autoreload value (timer period in timer ticks). With
// HSI=8MHz and PSC=8 you'll get a 1 MHz timer tick (1 us per tick). ARR=20000 -> 20 ms.
#define SERVO_PWM_PSC 8
#define SERVO_PWM_ARR 20000

// LED PWM removed

// Light sensor (digital output) on PB13
#define LIGHT_SENSOR_GPIO_PORT GPIOB
#define LIGHT_SENSOR_PIN GPIO_PIN_13
// Set to 1 if sensor DO is active high (DO = HIGH when light detected)
#define LIGHT_SENSOR_ACTIVE_HIGH 0

// Through-beam IR sensor (digital output) on PB14 used as an external
// interrupt to count events (e.g. beam broken). Adjust pull/config
// depending on your sensor wiring. By default we assume active-low
// (pulled up, output goes LOW when beam is broken).
#define IRSENSOR_GPIO_PORT GPIOB
#define IRSENSOR_PIN GPIO_PIN_14

// Initialize digital light sensor input (call from Board_Init)
void Board_Light_Init(void);
// Read digital light sensor (true = light detected)
bool Board_Light_Read(void);

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
