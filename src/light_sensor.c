#include "light_sensor.h"

// Initialize digital light sensor input on PB13
void Light_Init(void)
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
