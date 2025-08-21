#include "light_sensor.h"

// ADC handle for light sensor
static ADC_HandleTypeDef hadc1;

// Initialize ADC for light sensor on PA7 and digital input on PB13
void Light_Init(void)
{
  // Initialize digital light sensor input on PB13 (legacy compatibility)
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin  = LIGHT_SENSOR_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  // Use no pull by default; change to GPIO_PULLDOWN or GPIO_PULLUP as needed
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(LIGHT_SENSOR_GPIO_PORT, &GPIO_InitStruct);

  // Initialize ADC for analog light sensor on PA7
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_ADC1_CLK_ENABLE();

  // Configure PA7 as analog input
  GPIO_InitStruct.Pin = LIGHT_SENSOR_ADC_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LIGHT_SENSOR_ADC_PORT, &GPIO_InitStruct);

  // Configure ADC1
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  
  if (HAL_ADC_Init(&hadc1) != HAL_OK) {
    // ADC initialization error
    return;
  }

  // Configure the specific channel for light sensor
  ADC_ChannelConfTypeDef sConfig = {0};
  sConfig.Channel = LIGHT_SENSOR_ADC_CHANNEL;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

  // Calibrate ADC
  HAL_ADCEx_Calibration_Start(&hadc1);
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

// Read analog light sensor value (0-4095, 12-bit ADC)
uint16_t Light_ReadAnalog(void)
{  
  // Start ADC conversion
  if (HAL_ADC_Start(&hadc1) != HAL_OK) {
    return 0;
  }
  
  // Wait for conversion to complete
  if (HAL_ADC_PollForConversion(&hadc1, 1000) != HAL_OK) {
    HAL_ADC_Stop(&hadc1);
    return 0;
  }
  
  // Get ADC value
  uint16_t adc_value = HAL_ADC_GetValue(&hadc1);
  
  // Stop ADC
  HAL_ADC_Stop(&hadc1);
  
  return adc_value;
}

// Read light intensity as percentage (0-100%)
uint8_t Light_ReadPercent(void)
{
  uint16_t adc_value = Light_ReadAnalog();
  
  // Convert 12-bit ADC value (0-4095) to percentage (0-100)
  // Inverted for typical photoresistor behavior: higher voltage = less light
  // So we invert the calculation to show 100% for bright light, 0% for dark
  return (uint8_t)(100 - ((adc_value * 100) / 4095));
}

// Read raw percentage without inversion (0-100%)
uint8_t Light_ReadPercentRaw(void)
{
  uint16_t adc_value = Light_ReadAnalog();
  
  // Direct mapping: higher voltage = higher percentage
  // Use this if your sensor outputs higher voltage for more light
  return (uint8_t)((adc_value * 100) / 4095);
}
