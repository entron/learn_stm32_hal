#include "led.h"

void LED_Init(void)
{
  // Enable GPIOA clock for LEDs. If you add more ports, enable them here.
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  // Configure PA0 as push-pull output for simple LED control
  GPIO_InitStruct.Pin   = LED_PIN; // PA0
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
}

void Board_SetLed(bool on)
{
#if LED_ACTIVE_LOW
  HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, on ? GPIO_PIN_RESET : GPIO_PIN_SET);
#else
  HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
#endif
}

// Set specific LED pin to low (on) or high (off)
void Board_SetLedPin(uint16_t pin, bool on)
{
#if LED_ACTIVE_LOW
  HAL_GPIO_WritePin(LED_GPIO_PORT, pin, on ? GPIO_PIN_RESET : GPIO_PIN_SET);
#else
  HAL_GPIO_WritePin(LED_GPIO_PORT, pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
#endif
}
