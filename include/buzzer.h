#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32f1xx_hal.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Buzzer configuration: PB12
#define BUZZER_GPIO_PORT GPIOB
#define BUZZER_PIN GPIO_PIN_12
// Set to 1 if the buzzer is active low (MCU pin low turns buzzer on)
#define BUZZER_ACTIVE_LOW 1

// Initialize buzzer GPIO
void Buzzer_Init(void);

// Control buzzer (true = on, false = off)
void Board_Buzzer_Set(bool on);

#ifdef __cplusplus
}
#endif

#endif // __BUZZER_H
