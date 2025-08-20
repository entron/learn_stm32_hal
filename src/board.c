#include "board.h"
/* Include I2C HAL definitions */
#include "stm32f1xx_hal_i2c.h"

I2C_HandleTypeDef hi2c1;

static void I2C1_Init(void);

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
  I2C1_Init();               // init I2C1 used for external peripherals
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

// Initialize I2C1 peripheral on PB8 (SCL) and PB9 (SDA) — your wiring
static void I2C1_Init(void)
{
  // Enable clocks
  __HAL_RCC_I2C1_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_AFIO_CLK_ENABLE();

  // Remap I2C1 to PB8/PB9 (STM32F1 remap)
  __HAL_AFIO_REMAP_I2C1_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  // PB8 -> SCL, PB9 -> SDA
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
    Error_Handler();
  }
}

void Board_I2C_Init(void)
{
  I2C1_Init();
}

// Make HAL_Delay() advance
void SysTick_Handler(void) {
  HAL_IncTick();
}

void Error_Handler(void) {
  __disable_irq();
  while (1) { }
}