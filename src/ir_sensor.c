#include "ir_sensor.h"

// Initialize through-beam IR sensor input on PB14 as EXTI line 14.
void IR_Init(void)
{
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = IRSENSOR_PIN;
  /* Assume sensor output is active-low when beam is broken; use pull-up */
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(IRSENSOR_GPIO_PORT, &GPIO_InitStruct);

  /* EXTI15_10_IRQn already enabled in Keys_Init; line 14 will be handled there */
}
