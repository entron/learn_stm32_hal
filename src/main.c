#include "board.h"

int main(void)
{
  // initialize HAL, clocks and basic peripherals
  Board_Init();

  // Array of pins for PA0 to PA5
  const uint16_t led_pins[] = {
    GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5
  };
  const int num_leds = sizeof(led_pins)/sizeof(led_pins[0]);

  // Turn all LEDs off (active-low)
  for (int i = 0; i < num_leds; ++i) {
    HAL_GPIO_WritePin(GPIOA, led_pins[i], GPIO_PIN_SET);
  }

  int current = 0;
  while (1) {
    // Turn previous LED off
    HAL_GPIO_WritePin(GPIOA, led_pins[(current+num_leds-1)%num_leds], GPIO_PIN_SET);
    // Turn current LED on
    HAL_GPIO_WritePin(GPIOA, led_pins[current], GPIO_PIN_RESET);
    HAL_Delay(150);
    current = (current+1) % num_leds;
  }
}