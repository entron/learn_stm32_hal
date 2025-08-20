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
static void SERVO_PWM_TIM2_CH2_Init(void);
static void Servo_GPIO_Init(void);
static void Motor_GPIO_Init(void);
static void Motor_PWM_TIM2_CH3_Init(void);

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

static void LED_Init(void)
{
  // Enable GPIOA clock for LEDs. If you add more ports, enable them here.
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  // Configure PA0 as push-pull output for simple LED control
  GPIO_InitStruct.Pin   = LED_PIN; // PA0
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


static void SERVO_PWM_TIM2_CH2_Init(void)
{
  __HAL_RCC_TIM2_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  // PA1 -> TIM2_CH2
  GPIO_InitTypeDef g = {0};
  g.Pin   = GPIO_PIN_1;
  g.Mode  = GPIO_MODE_AF_PP;
  g.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &g);

  htim2.Instance = TIM2;
  htim2.Init.Prescaler         = SERVO_PWM_PSC - 1;   // 8-1 -> 1 MHz tick
  htim2.Init.CounterMode       = TIM_COUNTERMODE_UP;
  htim2.Init.Period            = SERVO_PWM_ARR - 1;   // 20000-1 -> 50 Hz
  htim2.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;  // <-- enable ARR preload
  HAL_TIM_Base_Init(&htim2);                                    // <-- (optional but clear)

  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK) Error_Handler();

  TIM_OC_InitTypeDef sConfigOC = {0};
  sConfigOC.OCMode     = TIM_OCMODE_PWM1;
  sConfigOC.Pulse      = SERVO_US_NEUTRAL;   // 1500 us
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) Error_Handler();

  if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2) != HAL_OK) Error_Handler();
}

static inline void servo_write_us(uint16_t us) {
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, us);  // 1 tick = 1 µs
}

// Public wrapper to set servo position in degrees (0..180)
void Servo_WriteDegrees(float deg) {
  if (deg < 0.0f) deg = 0.0f;
  if (deg > 180.0f) deg = 180.0f;
  uint16_t us = (uint16_t)(SERVO_US_MIN + ((SERVO_US_MAX - SERVO_US_MIN) * (deg / 180.0f)));
  servo_write_us(us);
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

// --- Public Servo API ---
void Servo_Init(void)
{
  // Initialize servo GPIO then configure TIM2 CH2 for servo PWM
  Servo_GPIO_Init();
  SERVO_PWM_TIM2_CH2_Init();
}

// --- Motor Control (TB6612FNG) API ---
void Motor_Init(void)
{
  // Initialize motor GPIO pins and configure TIM2 CH3 for motor PWM
  Motor_GPIO_Init();
  Motor_PWM_TIM2_CH3_Init();
  
  // Set initial state: motor stopped
  Motor_SetSpeed(0);
}

void Motor_SetSpeed(int8_t speed)
{
  // Clamp speed to valid range
  if (speed > 100) speed = 100;
  if (speed < -100) speed = -100;
  
  if (speed == 0) {
    // Stop motor: both AIN1 and AIN2 low
    HAL_GPIO_WritePin(MOTOR_AIN1_GPIO_PORT, MOTOR_AIN1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_AIN2_GPIO_PORT, MOTOR_AIN2_PIN, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
  } else if (speed > 0) {
    // Forward direction: AIN1 = HIGH, AIN2 = LOW
    HAL_GPIO_WritePin(MOTOR_AIN1_GPIO_PORT, MOTOR_AIN1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_AIN2_GPIO_PORT, MOTOR_AIN2_PIN, GPIO_PIN_RESET);
    // Set PWM duty cycle (0-20000 corresponds to 0-100%)
    uint16_t pwm_value = (uint16_t)((speed * 20000) / 100);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pwm_value);
  } else {
    // Reverse direction: AIN1 = LOW, AIN2 = HIGH
    HAL_GPIO_WritePin(MOTOR_AIN1_GPIO_PORT, MOTOR_AIN1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_AIN2_GPIO_PORT, MOTOR_AIN2_PIN, GPIO_PIN_SET);
    // Set PWM duty cycle for reverse speed (make speed positive)
    uint16_t pwm_value = (uint16_t)((-speed * 20000) / 100);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pwm_value);
  }
}

static void Motor_GPIO_Init(void)
{
  // Enable GPIOA clock and configure motor control pins
  __HAL_RCC_GPIOA_CLK_ENABLE();
  
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  // Configure PA2 as alternate function for TIM2_CH3 (PWM)
  GPIO_InitStruct.Pin = MOTOR_PWM_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(MOTOR_PWM_GPIO_PORT, &GPIO_InitStruct);
  
  // Configure PA4 (AIN1) and PA5 (AIN2) as push-pull outputs
  GPIO_InitStruct.Pin = MOTOR_AIN1_PIN | MOTOR_AIN2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MOTOR_AIN1_GPIO_PORT, &GPIO_InitStruct);
  
  // Set initial state: motor stopped
  HAL_GPIO_WritePin(MOTOR_AIN1_GPIO_PORT, MOTOR_AIN1_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(MOTOR_AIN2_GPIO_PORT, MOTOR_AIN2_PIN, GPIO_PIN_RESET);
}

static void Motor_PWM_TIM2_CH3_Init(void)
{
  // TIM2 is already initialized by servo, just need to configure CH3
  // Note: TIM2 is already running with 1MHz tick rate and 20ms period
  
  TIM_OC_InitTypeDef sConfigOC = {0};
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;  // Start with 0% duty cycle
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) {
    Error_Handler();
  }
  
  if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3) != HAL_OK) {
    Error_Handler();
  }
}

static void Servo_GPIO_Init(void)
{
  // Enable GPIOA clock and configure servo pin as AF push-pull
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = SERVO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SERVO_GPIO_PORT, &GPIO_InitStruct);
}

