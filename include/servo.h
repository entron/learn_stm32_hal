#ifndef __SERVO_H
#define __SERVO_H

#include "stm32f1xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

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

// Timer handle for PWM operations (servo on CH2, motor on CH3)
extern TIM_HandleTypeDef htim2;

// Initialize TIM2 CH2 to generate 50 Hz PWM for hobby servos.
// Uses 1 MHz timer tick (1 tick = 1 us), period 20 ms (ARR = 19999).
void Servo_Init(void);

// Set pulse width in microseconds (typical 1000..2000 us). Values are clamped.
void Servo_WriteMicroseconds(uint16_t us);

// Set servo position in degrees (0..180). Values are clamped.
void Servo_WriteDegrees(float deg);

// Error handler function
void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif // __SERVO_H
