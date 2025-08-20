#include "keys.h"

// Initialize keys/buttons on GPIOB pins B11 and B1.
// Assumption: buttons are wired to ground when pressed and need internal pull-up.
void Keys_Init(void)
{
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* Configure keys as input with pull-up and enable EXTI on both
    edges so presses (falling) and releases (rising) generate
    interrupts. This ensures the shared button state is updated on
    both transitions. */
  GPIO_InitStruct.Pin  = KEY_PINS;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP; // use pull-up; pressed = LOW
  HAL_GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStruct);

  /* Enable and set EXTI interrupt priority for lines used by the keys */
  HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}
