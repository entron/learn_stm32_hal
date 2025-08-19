#include "board.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>

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
  for (;;) {
    // Keys are configured with pull-up; pressed == LOW (GPIO_PIN_RESET)
    bool key_b11_pressed = (HAL_GPIO_ReadPin(KEY_GPIO_PORT, GPIO_PIN_11) == GPIO_PIN_RESET);
    bool key_b1_pressed  = (HAL_GPIO_ReadPin(KEY_GPIO_PORT, GPIO_PIN_1)  == GPIO_PIN_RESET);

    // Update LEDs: when button pressed -> LED on; when released -> LED off
    Board_SetLedPin(GPIO_PIN_1, key_b11_pressed);
    Board_SetLedPin(GPIO_PIN_2, key_b1_pressed);

    // Read light sensor and control buzzer: if there is NO light, activate buzzer
    bool light_present = Board_Light_Read();
    Board_Buzzer_Set(!light_present);

    // Update OLED with button states (simple, redraw whole buffer)
    char buf1[16];
    char buf2[16];
    snprintf(buf1, sizeof(buf1), "B11: %s", key_b11_pressed ? "ON" : "OFF");
    snprintf(buf2, sizeof(buf2),  "B1:  %s", key_b1_pressed  ? "ON" : "OFF");

    ssd1306_Fill(Black);
    ssd1306_SetCursor(10, 0);
    ssd1306_WriteString(title, Font_11x18, White);
    ssd1306_SetCursor(0, 30);
    ssd1306_WriteString(buf1, Font_6x8, White);
    ssd1306_SetCursor(64, 30);
    ssd1306_WriteString(buf2, Font_6x8, White);
    ssd1306_UpdateScreen();

    HAL_Delay(200);
  }
}