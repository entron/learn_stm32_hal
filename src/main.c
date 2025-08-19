#include "board.h"

int main(void)
{
  // initialize HAL, clocks and basic peripherals
  Board_Init();
  
  // After initialization, the LED is on but we will turn it off
  Board_SetLed(false);

  // After 2 seconds, turn only A1 LED on
  HAL_Delay(2000);
  Board_SetLedPin(GPIO_PIN_1, true);

  // After another 2 seconds, turn A1 LED off and turn A2 LED on
  HAL_Delay(2000);
  Board_SetLedPin(GPIO_PIN_1, false);
  Board_SetLedPin(GPIO_PIN_2, true);

  // After another 2 seconds, turn A2 LED off
  HAL_Delay(2000);
  Board_SetLedPin(GPIO_PIN_2, false);

}