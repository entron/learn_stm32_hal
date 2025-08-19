#include "board.h"

static void LED_Init(void);
static void Keys_Init(void);
static void Buzzer_Init(void);
static void Light_Init(void);

void Board_Init(void)
{
  HAL_Init();                 // init HAL & SysTick
  SystemClock_Config();       // set system clocks
  LED_Init();                 // init LED GPIOs
  Keys_Init();                // init user keys (B11, B1)
  Buzzer_Init();              // init buzzer on PB12
  Light_Init();               // init digital light sensor on PB13
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
  GPIO_InitStruct.Pin  = KEY_PINS;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP; // use pull-up; pressed = LOW
  HAL_GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStruct);
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