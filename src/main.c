#include "board.h"

int main(void)
{
  // initialize HAL, clocks and basic peripherals
  Board_Init();

  // PA0 is connected to the LED cathode (active-LOW). Drive high to keep LED off.
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); // LED off (PA0 high)
  while (1) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
    HAL_Delay(1000);
  }
}