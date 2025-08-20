#ifndef __IR_SENSOR_H
#define __IR_SENSOR_H

#include "stm32f1xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

// Through-beam IR sensor (digital output) on PB14 used as an external
// interrupt to count events (e.g. beam broken). Adjust pull/config
// depending on your sensor wiring. By default we assume active-low
// (pulled up, output goes LOW when beam is broken).
#define IRSENSOR_GPIO_PORT GPIOB
#define IRSENSOR_PIN GPIO_PIN_14

// Initialize through-beam IR sensor input
void IR_Init(void);

#ifdef __cplusplus
}
#endif

#endif // __IR_SENSOR_H
