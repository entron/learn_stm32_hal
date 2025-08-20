// Minimal servo demo on PA1 (TIM2_CH2)
#include "board.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>


static void OLED_ShowAngle(int angle) {
  char buf[20];
  ssd1306_Fill(Black);
  ssd1306_SetCursor(0, 0);
  snprintf(buf, sizeof(buf), "Angle: %d", angle);
  ssd1306_WriteString(buf, Font_11x18, White);
  ssd1306_UpdateScreen();
}

int main(void)
{
  Board_Init();

  /* Initialize OLED after I2C is ready */
  ssd1306_Init();

  // Move to three positions with 3 s delay between and show angle on OLED
  while (1) {
    int angle = 0;
    Servo_WriteDegrees((float)angle); // ~0 deg
    OLED_ShowAngle(angle);
    HAL_Delay(3000);

    angle = 90;
    Servo_WriteDegrees((float)angle); // ~90 deg
    OLED_ShowAngle(angle);
    HAL_Delay(3000);

    angle = 180;
    Servo_WriteDegrees((float)angle); // ~180 deg
    OLED_ShowAngle(angle);
    HAL_Delay(3000);
  }
}