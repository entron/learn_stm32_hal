#ifndef __LIGHT_SENSOR_H
#define __LIGHT_SENSOR_H

#include "stm32f1xx_hal.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Light sensor (digital output) on PB13
#define LIGHT_SENSOR_GPIO_PORT GPIOB
#define LIGHT_SENSOR_PIN GPIO_PIN_13
// Set to 1 if sensor DO is active high (DO = HIGH when light detected)
#define LIGHT_SENSOR_ACTIVE_HIGH 0

// Initialize digital light sensor input
void Light_Init(void);

// Read digital light sensor (true = light detected)
bool Board_Light_Read(void);

#ifdef __cplusplus
}
#endif

#endif // __LIGHT_SENSOR_H
