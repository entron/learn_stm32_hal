#include "board.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>

/* Button state shared between IRQ and main loop. Updated in
  HAL_GPIO_EXTI_Callback() (interrupt context) and read in the
  main loop for display and other logic. */
volatile bool g_key_b11_pressed = false;
volatile bool g_key_b1_pressed = false;
volatile bool g_key_state_changed = false;

int main(void)
{
  // initialize HAL, clocks and minimal board peripherals
  Board_Init();

  // Ensure LEDs start off
  Board_SetLed(false);

  // Initialize OLED (SSD1306)
  ssd1306_Init();
  ssd1306_Fill(Black);
  ssd1306_SetCursor(10, 0);
  // Title
  char title[] = "SSD1306";
  ssd1306_WriteString(title, Font_11x18, White);
  ssd1306_UpdateScreen();

  // Poll keys and control LEDs: B11 -> LED pin PA1, B1 -> LED pin PA2
  // Read initial key state once (so display/LEDs start consistent)
  g_key_b11_pressed = (HAL_GPIO_ReadPin(KEY_GPIO_PORT, GPIO_PIN_11) == GPIO_PIN_RESET);
  g_key_b1_pressed = (HAL_GPIO_ReadPin(KEY_GPIO_PORT, GPIO_PIN_1) == GPIO_PIN_RESET);
  // Force an initial OLED refresh to show the starting states
  g_key_state_changed = true;

  for (;;)
  {
    if (g_key_state_changed == true)
    {
      g_key_state_changed = false; // Reset flag

      // Re-read current states
      bool disp_b11 = g_key_b11_pressed;
      bool disp_b1 = g_key_b1_pressed;
      char buf1[16];
      char buf2[16];
      snprintf(buf1, sizeof(buf1), "B11: %s", disp_b11 ? "ON" : "OFF");
      snprintf(buf2, sizeof(buf2), "B1:  %s", disp_b1 ? "ON" : "OFF");

      ssd1306_Fill(Black);
      ssd1306_SetCursor(10, 0);
      ssd1306_WriteString(title, Font_11x18, White);
      ssd1306_SetCursor(0, 30);
      ssd1306_WriteString(buf1, Font_6x8, White);
      ssd1306_SetCursor(64, 30);
      ssd1306_WriteString(buf2, Font_6x8, White);
      ssd1306_UpdateScreen();

      // Update LEDs according to the stored button state
      bool key_b11_pressed = g_key_b11_pressed;
      bool key_b1_pressed = g_key_b1_pressed;
      Board_SetLedPin(GPIO_PIN_1, key_b11_pressed);
      Board_SetLedPin(GPIO_PIN_2, key_b1_pressed);
    }

    HAL_Delay(50);
  }
}

// Example EXTI callback: updates LEDs and the OLED when user keys trigger
// an external interrupt. This is a lightweight example — keep callbacks
// short and avoid heavy processing in interrupt context if you need low
// latency or multitasking.
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  // Keep IRQ handler short: update shared state and mark it changed.
  if (GPIO_Pin == GPIO_PIN_11)
  {
    g_key_b11_pressed = (HAL_GPIO_ReadPin(KEY_GPIO_PORT, GPIO_PIN_11) == GPIO_PIN_RESET);
    g_key_state_changed = true;
  }
  else if (GPIO_Pin == GPIO_PIN_1)
  {
    g_key_b1_pressed = (HAL_GPIO_ReadPin(KEY_GPIO_PORT, GPIO_PIN_1) == GPIO_PIN_RESET);
    g_key_state_changed = true;
  }
}