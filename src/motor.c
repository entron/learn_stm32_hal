#include "motor.h"
#include "servo.h"  // For htim2 handle

static void Motor_GPIO_Init(void);
static void Motor_PWM_TIM2_CH3_Init(void);

void Motor_Init(void)
{
  // Initialize motor GPIO pins and configure TIM2 CH3 for motor PWM
  Motor_GPIO_Init();
  Motor_PWM_TIM2_CH3_Init();
  
  // Set initial state: motor stopped
  Motor_SetSpeed(0);
}

void Motor_SetSpeed(int8_t speed)
{
  // Clamp speed to valid range
  if (speed > 100) speed = 100;
  if (speed < -100) speed = -100;
  
  if (speed == 0) {
    // Stop motor: both AIN1 and AIN2 low
    HAL_GPIO_WritePin(MOTOR_AIN1_GPIO_PORT, MOTOR_AIN1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_AIN2_GPIO_PORT, MOTOR_AIN2_PIN, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
  } else if (speed > 0) {
    // Forward direction: AIN1 = HIGH, AIN2 = LOW
    HAL_GPIO_WritePin(MOTOR_AIN1_GPIO_PORT, MOTOR_AIN1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_AIN2_GPIO_PORT, MOTOR_AIN2_PIN, GPIO_PIN_RESET);
    // Set PWM duty cycle (0-20000 corresponds to 0-100%)
    uint16_t pwm_value = (uint16_t)((speed * 20000) / 100);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pwm_value);
  } else {
    // Reverse direction: AIN1 = LOW, AIN2 = HIGH
    HAL_GPIO_WritePin(MOTOR_AIN1_GPIO_PORT, MOTOR_AIN1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_AIN2_GPIO_PORT, MOTOR_AIN2_PIN, GPIO_PIN_SET);
    // Set PWM duty cycle for reverse speed (make speed positive)
    uint16_t pwm_value = (uint16_t)((-speed * 20000) / 100);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pwm_value);
  }
}

static void Motor_GPIO_Init(void)
{
  // Enable GPIOA clock and configure motor control pins
  __HAL_RCC_GPIOA_CLK_ENABLE();
  
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  // Configure PA2 as alternate function for TIM2_CH3 (PWM)
  GPIO_InitStruct.Pin = MOTOR_PWM_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(MOTOR_PWM_GPIO_PORT, &GPIO_InitStruct);
  
  // Configure PA4 (AIN1) and PA5 (AIN2) as push-pull outputs
  GPIO_InitStruct.Pin = MOTOR_AIN1_PIN | MOTOR_AIN2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MOTOR_AIN1_GPIO_PORT, &GPIO_InitStruct);
  
  // Set initial state: motor stopped
  HAL_GPIO_WritePin(MOTOR_AIN1_GPIO_PORT, MOTOR_AIN1_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(MOTOR_AIN2_GPIO_PORT, MOTOR_AIN2_PIN, GPIO_PIN_RESET);
}

static void Motor_PWM_TIM2_CH3_Init(void)
{
  // TIM2 is already initialized by servo, just need to configure CH3
  // Note: TIM2 is already running with 1MHz tick rate and 20ms period
  
  TIM_OC_InitTypeDef sConfigOC = {0};
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;  // Start with 0% duty cycle
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) {
    Error_Handler();
  }
  
  if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3) != HAL_OK) {
    Error_Handler();
  }
}
