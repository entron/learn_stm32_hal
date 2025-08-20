// Minimal servo demo on PA1 (TIM2_CH2)
#include "board.h"

int main(void)
{
  Board_Init();
  Servo_Init();

  // Move to three positions with 1 s delay between
  while (1) {
    Servo_WriteMicroseconds(1000); // ~0 deg
    HAL_Delay(1000);
    Servo_WriteMicroseconds(1500); // ~90 deg
    HAL_Delay(1000);
    Servo_WriteMicroseconds(2000); // ~180 deg
    HAL_Delay(1000);
  }
}