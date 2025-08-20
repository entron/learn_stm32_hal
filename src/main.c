// Minimal servo demo on PA1 (TIM2_CH2)
#include "board.h"


int main(void)
{
  Board_Init();
  Servo_Init();
  // Move to three positions with 1 s delay between
  while (1) {
  Servo_WriteDegrees(0); // ~0 deg
    HAL_Delay(3000);
  Servo_WriteDegrees(90); // ~90 deg
    HAL_Delay(3000);
  Servo_WriteDegrees(180); // ~180 deg
    HAL_Delay(3000);
  }
}