#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Motor Control (TB6612FNG) definitions
// Motor PWM on PA2 (TIM2_CH3), direction control on PA4 (AIN1) and PA5 (AIN2)
#define MOTOR_PWM_GPIO_PORT GPIOA
#define MOTOR_PWM_PIN GPIO_PIN_2
#define MOTOR_AIN1_GPIO_PORT GPIOA
#define MOTOR_AIN1_PIN GPIO_PIN_4
#define MOTOR_AIN2_GPIO_PORT GPIOA
#define MOTOR_AIN2_PIN GPIO_PIN_5

// Timer handle reference (declared in servo.h)
extern TIM_HandleTypeDef htim2;

// Initialize motor control GPIOs and PWM timer
void Motor_Init(void);

// Set motor speed: -100 (full reverse) to +100 (full forward), 0 = stop
void Motor_SetSpeed(int8_t speed);

#ifdef __cplusplus
}
#endif

#endif // __MOTOR_H
