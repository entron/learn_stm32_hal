// Servo and Motor demo with button control and light sensor
// B1 controls servo angle, B11 controls motor speed
// Light sensor readings are sent via USART1 (PA9=TX, PA10=RX) at 115200 baud
#include "board.h"
#include <stdio.h>

// Include only the components we actually use
#include "keys.h"
#include "servo.h"
#include "motor.h"
#include "oled.h"
#include "light_sensor.h"
#include "usart.h"

// Global variables for control
static volatile int servo_angle = 0;
static volatile int8_t motor_speed = 0;
static volatile bool button_b1_pressed = false;
static volatile bool button_b11_pressed = false;

static void OLED_ShowStatus(int angle, int8_t speed) {
  uint16_t light_raw = Light_ReadAnalog();
  uint8_t light_percent = Light_ReadPercent();
  
  OLED_Clear();
  OLED_Printf(0, 0, OLED_FONT_MEDIUM, false, "Servo: %d°", angle);
  OLED_Printf(0, 15, OLED_FONT_MEDIUM, false, "Motor: %d%%", speed);
  OLED_Printf(0, 30, OLED_FONT_SMALL, false, "Light: %d%% (%d)", light_percent, light_raw);
  OLED_Printf(0, 45, OLED_FONT_SMALL, false, "B1:Servo B11:Motor");
  OLED_Update();
}

// GPIO EXTI callback function - called when button is pressed/released
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  static uint32_t last_interrupt_time_b1 = 0;
  static uint32_t last_interrupt_time_b11 = 0;
  uint32_t current_time = HAL_GetTick();
  
  if (GPIO_Pin == GPIO_PIN_1) {
    // Debounce: ignore interrupts that occur within 200ms of the last one
    if (current_time - last_interrupt_time_b1 < 200) {
      return;
    }
    
    // Check if button is actually pressed (pin goes LOW when pressed due to pull-up)
    if (HAL_GPIO_ReadPin(KEY_GPIO_PORT, GPIO_PIN_1) == GPIO_PIN_RESET) {
      button_b1_pressed = true;
    }
    last_interrupt_time_b1 = current_time;
  }
  
  if (GPIO_Pin == GPIO_PIN_11) {
    // Debounce: ignore interrupts that occur within 200ms of the last one
    if (current_time - last_interrupt_time_b11 < 200) {
      return;
    }
    
    // Check if button is actually pressed (pin goes LOW when pressed due to pull-up)
    if (HAL_GPIO_ReadPin(KEY_GPIO_PORT, GPIO_PIN_11) == GPIO_PIN_RESET) {
      button_b11_pressed = true;
    }
    last_interrupt_time_b11 = current_time;
  }
}

int main(void)
{
  Board_Init();  // Initialize core system (HAL, clocks)

  // Initialize only the components we need
  Keys_Init();       // Initialize buttons (B1, B11)
  Servo_Init();      // Initialize servo on PA1
  Motor_Init();      // Initialize motor on PA2/PA4/PA5
  OLED_Init();       // Initialize OLED display
  Light_Init();      // Initialize light sensor ADC on PA7
  USART_Init();      // Initialize USART1 for serial communication

  // Send startup message
  USART_Printf("STM32F103C8 Light Sensor Monitor\r\n");
  USART_Printf("Sending light sensor readings every 1 second\r\n");
  USART_Printf("Format: timestamp_ms,light_percent,light_raw\r\n");
  USART_Printf("---\r\n");

  // Initialize servo and motor to initial positions
  Servo_WriteDegrees((float)servo_angle);
  Motor_SetSpeed(motor_speed);
  OLED_ShowStatus(servo_angle, motor_speed);

  // Main loop - check for button presses and update servo/motor
  uint32_t last_display_update = 0;
  uint32_t last_usart_send = 0;
  while (1) {
    uint32_t current_time = HAL_GetTick();
    
    if (button_b1_pressed) {
      button_b1_pressed = false;  // Clear the flag
      
      // Increase servo angle by 30 degrees
      servo_angle += 30;
      
      // Wrap around when 180 is reached
      if (servo_angle > 180) {
        servo_angle = 0;
      }
      
      // Update servo position and display
      Servo_WriteDegrees((float)servo_angle);
      OLED_ShowStatus(servo_angle, motor_speed);
      last_display_update = current_time;
    }
    
    if (button_b11_pressed) {
      button_b11_pressed = false;  // Clear the flag
      
      // Cycle through motor speeds: 0 -> 50 -> 100 -> -50 -> -100 -> 0
      if (motor_speed == 0) {
        motor_speed = 20;
      } else if (motor_speed == 20) {
        motor_speed = 50;
      } else if (motor_speed == 50) {
        motor_speed = -20;
      } else if (motor_speed == -20) {
        motor_speed = -50;
      } else {
        motor_speed = 0;
      }
      
      // Update motor speed and display
      Motor_SetSpeed(motor_speed);
      OLED_ShowStatus(servo_angle, motor_speed);
      last_display_update = current_time;
    }
    
    // Update display every 500ms to show current light sensor readings
    if (current_time - last_display_update >= 500) {
      OLED_ShowStatus(servo_angle, motor_speed);
      last_display_update = current_time;
    }
    
    // Send light sensor data via USART every 1000ms (1 second)
    if (current_time - last_usart_send >= 1000) {
      uint16_t light_raw = Light_ReadAnalog();
      uint8_t light_percent = Light_ReadPercent();
      
      // Send CSV format: timestamp,light_percent,light_raw
      USART_Printf("%lu,%u,%u\r\n", current_time, light_percent, light_raw);
      
      last_usart_send = current_time;
    }
    
    // Small delay to prevent busy waiting
    HAL_Delay(10);
  }
}