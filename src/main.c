// Minimal servo demo on PA1 (TIM2_CH2) with button control on PB1
#include "board.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>

// Global variables for servo control
static volatile int servo_angle = 0;
static volatile bool button_pressed = false;

static void OLED_ShowAngle(int angle) {
  char buf[20];
  ssd1306_Fill(Black);
  ssd1306_SetCursor(0, 0);
  snprintf(buf, sizeof(buf), "Angle: %d", angle);
  ssd1306_WriteString(buf, Font_11x18, White);
  ssd1306_UpdateScreen();
}

// GPIO EXTI callback function - called when button is pressed/released
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  static uint32_t last_interrupt_time = 0;
  uint32_t current_time = HAL_GetTick();
  
  // Debounce: ignore interrupts that occur within 200ms of the last one
  if (current_time - last_interrupt_time < 200) {
    return;
  }
  
  if (GPIO_Pin == GPIO_PIN_1) {
    // Check if button is actually pressed (pin goes LOW when pressed due to pull-up)
    if (HAL_GPIO_ReadPin(KEY_GPIO_PORT, GPIO_PIN_1) == GPIO_PIN_RESET) {
      button_pressed = true;
    }
  }
  
  last_interrupt_time = current_time;
}

int main(void)
{
  Board_Init();

  /* Initialize OLED after I2C is ready */
  ssd1306_Init();

  // Initialize servo and set initial position
  Servo_WriteDegrees((float)servo_angle);
  OLED_ShowAngle(servo_angle);

  // Main loop - check for button presses and update servo
  while (1) {
    if (button_pressed) {
      button_pressed = false;  // Clear the flag
      
      // Increase angle by 30 degrees
      servo_angle += 30;
      
      // Wrap around when 180 is reached
      if (servo_angle > 180) {
        servo_angle = 0;
      }
      
      // Update servo position and display
      Servo_WriteDegrees((float)servo_angle);
      OLED_ShowAngle(servo_angle);
    }
    
    // Small delay to prevent busy waiting
    HAL_Delay(10);
  }
}