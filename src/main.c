#include "board.h"

int main(void)
{
  // initialize HAL, clocks and minimal board peripherals
  Board_Init();

  // Ensure LEDs start off
  Board_SetLed(false);

  // Poll keys and control LEDs: B11 -> LED pin PA1, B1 -> LED pin PA2
  for (;;) {
    // Keys are configured with pull-up; pressed == LOW (GPIO_PIN_RESET)
    bool key_b11_pressed = (HAL_GPIO_ReadPin(KEY_GPIO_PORT, GPIO_PIN_11) == GPIO_PIN_RESET);
    bool key_b1_pressed  = (HAL_GPIO_ReadPin(KEY_GPIO_PORT, GPIO_PIN_1)  == GPIO_PIN_RESET);

    // Update LEDs: when button pressed -> LED on; when released -> LED off
    Board_SetLedPin(GPIO_PIN_1, key_b11_pressed);
    Board_SetLedPin(GPIO_PIN_2, key_b1_pressed);

    HAL_Delay(10); // simple polling interval / basic debounce
  }
}