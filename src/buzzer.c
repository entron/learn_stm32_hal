#include "buzzer.h"

// Initialize buzzer GPIO (push-pull output)
void Buzzer_Init(void)
{
  // Ensure GPIOB clock is enabled (Keys_Init also enables it, but do here to be safe)
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin   = BUZZER_PIN;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUZZER_GPIO_PORT, &GPIO_InitStruct);

  // Default to buzzer off
#if BUZZER_ACTIVE_LOW
  HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, GPIO_PIN_SET);
#else
  HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, GPIO_PIN_RESET);
#endif
}

// Control buzzer (true = on, false = off)
void Board_Buzzer_Set(bool on)
{
#if BUZZER_ACTIVE_LOW
  HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, on ? GPIO_PIN_RESET : GPIO_PIN_SET);
#else
  HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
#endif
}
