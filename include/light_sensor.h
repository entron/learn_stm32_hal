#ifndef __LIGHT_SENSOR_H
#define __LIGHT_SENSOR_H

#include "stm32f1xx_hal.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Light sensor ADC configuration
// PA7 is connected to ADC1 Channel 7
#define LIGHT_SENSOR_ADC_PORT GPIOA
#define LIGHT_SENSOR_ADC_PIN GPIO_PIN_7
#define LIGHT_SENSOR_ADC_CHANNEL ADC_CHANNEL_7

// Light sensor (digital output) on PB13 (legacy, kept for compatibility)
#define LIGHT_SENSOR_GPIO_PORT GPIOB
#define LIGHT_SENSOR_PIN GPIO_PIN_13
// Set to 1 if sensor DO is active high (DO = HIGH when light detected)
#define LIGHT_SENSOR_ACTIVE_HIGH 0

// Initialize light sensor (both digital and ADC)
void Light_Init(void);

// Read digital light sensor (true = light detected)
bool Board_Light_Read(void);

// Read analog light sensor value (0-4095, 12-bit ADC)
uint16_t Light_ReadAnalog(void);

// Read light intensity as percentage (0-100%)
// Inverted for typical photoresistor: 0% = dark, 100% = bright
uint8_t Light_ReadPercent(void);

// Read raw percentage without inversion (0-100%)
// Direct mapping: higher voltage = higher percentage
uint8_t Light_ReadPercentRaw(void);

#ifdef __cplusplus
}
#endif

#endif // __LIGHT_SENSOR_H
