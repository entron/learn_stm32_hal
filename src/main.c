#include "board.h"

int main(void)
{
  // initialize HAL, clocks and basic peripherals
  Board_Init();

  // Start with LED off
  Board_SetLed(false);
  while (1) {
    HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
    HAL_Delay(1000);
  }
}