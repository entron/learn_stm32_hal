#include "board.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>
#include <stdint.h>

/* Button state shared between IRQ and main loop. Updated in
  HAL_GPIO_EXTI_Callback() (interrupt context) and read in the
  main loop for display and other logic. */
volatile bool g_key_b11_pressed = false;
volatile bool g_key_b1_pressed = false;
volatile bool g_key_state_changed = false;
// IR sensor counter: incremented from IRQ when beam is broken.
volatile unsigned int g_ir_counter = 0;
// Timer counter (seconds)
volatile uint32_t g_timer_seconds = 0;

int main(void)
{
  // initialize HAL, clocks and minimal board peripherals
  Board_Init();

  // Ensure LEDs start off
  Board_SetLed(false);

  // Breathing LED PWM state
  uint16_t breath_val = 0;      // 0..1000 logical brightness
  int8_t breath_dir = 1;        // 1 = up, -1 = down
  const uint16_t breath_max = 1000; // matches TIM2 ARR+1
  const uint16_t step = 4;      // adjust for speed

  // Initialize OLED (SSD1306)
  ssd1306_Init();
  ssd1306_Fill(Black);
  ssd1306_UpdateScreen();

  // Poll keys and control LEDs: B11 -> LED pin PA1, B1 -> LED pin PA2
  // Read initial key state once (so display/LEDs start consistent)
  g_key_b11_pressed = (HAL_GPIO_ReadPin(KEY_GPIO_PORT, GPIO_PIN_11) == GPIO_PIN_RESET);
  g_key_b1_pressed = (HAL_GPIO_ReadPin(KEY_GPIO_PORT, GPIO_PIN_1) == GPIO_PIN_RESET);
  // Force an initial OLED refresh to show the starting states
  g_key_state_changed = true;

  for (;;)
  {
  // Update breathing PWM every loop (linear)
  Board_LED_SetBrightness(breath_val);

    // Advance triangle wave
    if (breath_dir > 0) {
      if (breath_val + step < breath_max) breath_val += step; else { breath_val = breath_max; breath_dir = -1; }
    } else {
      if (breath_val > step) breath_val -= step; else { breath_val = 0; breath_dir = 1; }
    }

    if (g_key_state_changed == true)
    {
      g_key_state_changed = false; // Reset flag

      // Re-read current states
      bool disp_b11 = g_key_b11_pressed;
      bool disp_b1 = g_key_b1_pressed;
      unsigned int disp_counter = g_ir_counter; // snapshot of the counter
      uint32_t disp_timer = g_timer_seconds; // snapshot of timer
      char buf_timer[24];
      char buf1[16];
      char buf2[16];
      char buf3[20];
      snprintf(buf_timer, sizeof(buf_timer), "Time: %lus", (unsigned long)disp_timer);
      snprintf(buf1, sizeof(buf1), "B11: %s", disp_b11 ? "ON" : "OFF");
      snprintf(buf2, sizeof(buf2), "B1:  %s", disp_b1 ? "ON" : "OFF");
      snprintf(buf3, sizeof(buf3), "Count: %u", disp_counter);

      ssd1306_Fill(Black);
      // Timer at the top in a larger font
      ssd1306_SetCursor(0, 0);
      ssd1306_WriteString(buf_timer, Font_11x18, White);
      // Status lines
      ssd1306_SetCursor(0, 30);
      ssd1306_WriteString(buf1, Font_6x8, White);
      ssd1306_SetCursor(64, 30);
      ssd1306_WriteString(buf2, Font_6x8, White);
      ssd1306_SetCursor(0, 48);
      ssd1306_WriteString(buf3, Font_6x8, White);
      ssd1306_UpdateScreen();

  // LED on PA0 now runs breathing PWM; keep keys just for display
    }

  HAL_Delay(5);
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
  else if (GPIO_Pin == GPIO_PIN_14)
  {
    // IR sensor triggered on PB14. Assume active-low transition indicates beam broken.
    // Increment counter on the falling edge (pin reads LOW). For simplicity we
    // increment on any transition with a check to only count falling edges.
    if (HAL_GPIO_ReadPin(IRSENSOR_GPIO_PORT, IRSENSOR_PIN) == GPIO_PIN_RESET)
    {
      g_ir_counter++;
      g_key_state_changed = true; // reuse flag to refresh OLED
    }
  }
}

// HAL timer period elapsed callback — called from 1 Hz timer (TIM3)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM3)
  {
    g_timer_seconds++;
    g_key_state_changed = true; // request OLED refresh in main loop
  }
}