#include "board.h"

void Board_Init(void)
{
  HAL_Init();                 // init HAL & SysTick
  SystemClock_Config();       // set system clocks
  LED_Init();                 // init LED GPIOs (PA0 as simple output)
  Keys_Init();                // init user keys (B11, B1)
  Buzzer_Init();              // init buzzer on PB12
  Light_Init();               // init digital light sensor on PB13
  IR_Init();                  // init through-beam IR sensor on PB14
  Servo_Init();               // init servo GPIO + TIM2 CH2 for 50Hz servo on PA1
  Motor_Init();               // init motor GPIO + TIM2 CH3 for motor PWM on PA2
}

void SystemClock_Config(void)
{
  // Run from HSI 8 MHz (no PLL) — simple & reliable
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler();

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;   // 8 MHz
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) Error_Handler();
}

// Make HAL_Delay() advance
void SysTick_Handler(void) {
  HAL_IncTick();
}

void Error_Handler(void) {
  __disable_irq();
  while (1) { }
}