#include "board.h"

int main(void)
{
  // initialize HAL, clocks and basic peripherals
  Board_Init();

  // PC13 LED is active-LOW on most Blue Pills
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); // LED off
  while (1) {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    HAL_Delay(200);
  }
}