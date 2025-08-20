#include "board.h"
/* Include I2C HAL definitions */
#include "stm32f1xx_hal_i2c.h"
#include "stm32f1xx_hal_tim.h"

I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim2;

static void I2C1_Init(void);

static void LED_Init(void);
static void Keys_Init(void);
static void Buzzer_Init(void);
static void Light_Init(void);
static void IR_Init(void);
static void Timer2_Init(void);

void Board_Init(void)
{
  HAL_Init();                 // init HAL & SysTick
  SystemClock_Config();       // set system clocks
  LED_Init();                 // init LED GPIOs
  Keys_Init();                // init user keys (B11, B1)
  Buzzer_Init();              // init buzzer on PB12
  Light_Init();               // init digital light sensor on PB13
  IR_Init();                  // init through-beam IR sensor on PB14
  Timer2_Init();              // init TIM2 as 1 Hz timer (updates display)
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

static void LED_Init(void)
{
  // Enable GPIOA clock for LEDs. If you add more ports, enable them here.
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin   = LED_PIN;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
}

// Initialize keys/buttons on GPIOB pins B11 and B1.
// Assumption: buttons are wired to ground when pressed and need internal pull-up.
static void Keys_Init(void)
{
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* Configure keys as input with pull-up and enable EXTI on both
    edges so presses (falling) and releases (rising) generate
    interrupts. This ensures the shared button state is updated on
    both transitions. */
  GPIO_InitStruct.Pin  = KEY_PINS;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP; // use pull-up; pressed = LOW
  HAL_GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStruct);

  /* Enable and set EXTI interrupt priority for lines used by the keys */
  HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

// Initialize buzzer GPIO (push-pull output)
static void Buzzer_Init(void)
{
  // Ensure GPIOB clock is enabled (Keys_Init also enables it, but do here to be safe)
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin   = BUZZER_PIN;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUZZER_GPIO_PORT, &GPIO_InitStruct);

  // Default to buzzer off
#if BUZZER_ACTIVE_LOW
  HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, GPIO_PIN_SET);
#else
  HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, GPIO_PIN_RESET);
#endif
}

// Control buzzer (true = on, false = off)
void Board_Buzzer_Set(bool on)
{
#if BUZZER_ACTIVE_LOW
  HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, on ? GPIO_PIN_RESET : GPIO_PIN_SET);
#else
  HAL_GPIO_WritePin(BUZZER_GPIO_PORT, BUZZER_PIN, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
#endif
}

// Initialize digital light sensor input on PB13
static void Light_Init(void)
{
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin  = LIGHT_SENSOR_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  // Use no pull by default; change to GPIO_PULLDOWN or GPIO_PULLUP as needed
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(LIGHT_SENSOR_GPIO_PORT, &GPIO_InitStruct);
}

// Initialize through-beam IR sensor input on PB14 as EXTI line 14.
static void IR_Init(void)
{
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = IRSENSOR_PIN;
  /* Assume sensor output is active-low when beam is broken; use pull-up */
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(IRSENSOR_GPIO_PORT, &GPIO_InitStruct);

  /* EXTI15_10_IRQn already enabled in Keys_Init; line 14 will be handled there */
}

// Configure TIM2 to generate update interrupt at 1 Hz
static void Timer2_Init(void)
{
  __HAL_RCC_TIM2_CLK_ENABLE();

  htim2.Instance = TIM2;
  // With HSI 8 MHz and APB1 prescaler = 1, TIM2 clock = 8 MHz
  // Prescaler 8000-1 -> 1 kHz counter clock; Period 1000-1 -> 1 Hz update
  htim2.Init.Prescaler = 8000 - 1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1000 - 1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
    Error_Handler();
  }

  HAL_NVIC_SetPriority(TIM2_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);

  if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK) {
    Error_Handler();
  }
}

// Read digital light sensor: return true when sensor reports light present
bool Board_Light_Read(void)
{
#if LIGHT_SENSOR_ACTIVE_HIGH
  return HAL_GPIO_ReadPin(LIGHT_SENSOR_GPIO_PORT, LIGHT_SENSOR_PIN) == GPIO_PIN_SET;
#else
  return HAL_GPIO_ReadPin(LIGHT_SENSOR_GPIO_PORT, LIGHT_SENSOR_PIN) == GPIO_PIN_RESET;
#endif
}

void Board_SetLed(bool on)
{
#if LED_ACTIVE_LOW
  HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, on ? GPIO_PIN_RESET : GPIO_PIN_SET);
#else
  HAL_GPIO_WritePin(LED_GPIO_PORT, LED_PIN, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
#endif
}

// Set specific LED pin to low (on) or high (off)
void Board_SetLedPin(uint16_t pin, bool on)
{
#if LED_ACTIVE_LOW
  HAL_GPIO_WritePin(LED_GPIO_PORT, pin, on ? GPIO_PIN_RESET : GPIO_PIN_SET);
#else
  HAL_GPIO_WritePin(LED_GPIO_PORT, pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
#endif
}

// Make HAL_Delay() advance
void SysTick_Handler(void) {
  HAL_IncTick();
}

void Error_Handler(void) {
  __disable_irq();
  while (1) { }
}