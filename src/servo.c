#include "servo.h"
#include "stm32f1xx_hal_tim.h"

TIM_HandleTypeDef htim2;

static void SERVO_PWM_TIM2_CH2_Init(void);
static void Servo_GPIO_Init(void);
static inline void servo_write_us(uint16_t us);

void Servo_Init(void)
{
  // Initialize servo GPIO then configure TIM2 CH2 for servo PWM
  Servo_GPIO_Init();
  SERVO_PWM_TIM2_CH2_Init();
}

static void SERVO_PWM_TIM2_CH2_Init(void)
{
  __HAL_RCC_TIM2_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  // PA1 -> TIM2_CH2
  GPIO_InitTypeDef g = {0};
  g.Pin   = GPIO_PIN_1;
  g.Mode  = GPIO_MODE_AF_PP;
  g.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &g);

  htim2.Instance = TIM2;
  htim2.Init.Prescaler         = SERVO_PWM_PSC - 1;   // 8-1 -> 1 MHz tick
  htim2.Init.CounterMode       = TIM_COUNTERMODE_UP;
  htim2.Init.Period            = SERVO_PWM_ARR - 1;   // 20000-1 -> 50 Hz
  htim2.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;  // <-- enable ARR preload
  HAL_TIM_Base_Init(&htim2);                                    // <-- (optional but clear)

  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK) Error_Handler();

  TIM_OC_InitTypeDef sConfigOC = {0};
  sConfigOC.OCMode     = TIM_OCMODE_PWM1;
  sConfigOC.Pulse      = SERVO_US_NEUTRAL;   // 1500 us
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) Error_Handler();

  if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2) != HAL_OK) Error_Handler();
}

static void Servo_GPIO_Init(void)
{
  // Enable GPIOA clock and configure servo pin as AF push-pull
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = SERVO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SERVO_GPIO_PORT, &GPIO_InitStruct);
}

static inline void servo_write_us(uint16_t us) {
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, us);  // 1 tick = 1 µs
}

void Servo_WriteMicroseconds(uint16_t us)
{
  // Clamp to safe limits
  if (us < SERVO_US_MIN) us = SERVO_US_MIN;
  if (us > SERVO_US_MAX) us = SERVO_US_MAX;
  servo_write_us(us);
}

// Public wrapper to set servo position in degrees (0..180)
void Servo_WriteDegrees(float deg) {
  if (deg < 0.0f) deg = 0.0f;
  if (deg > 180.0f) deg = 180.0f;
  uint16_t us = (uint16_t)(SERVO_US_MIN + ((SERVO_US_MAX - SERVO_US_MIN) * (deg / 180.0f)));
  servo_write_us(us);
}
