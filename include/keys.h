#ifndef __KEYS_H
#define __KEYS_H

#include "stm32f1xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

// Keys/buttons configuration: B11 and B1
#define KEY_GPIO_PORT GPIOB
#define KEY_PINS (GPIO_PIN_11 | GPIO_PIN_1)

// Initialize keys/buttons GPIO and interrupts
void Keys_Init(void);

#ifdef __cplusplus
}
#endif

#endif // __KEYS_H
