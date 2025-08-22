#ifndef __USART_H
#define __USART_H

#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// USART1 definitions for STM32F103C8 (Blue Pill)
// PA9  - USART1_TX
// PA10 - USART1_RX
#define USART_PORT           GPIOA
#define USART_TX_PIN         GPIO_PIN_9
#define USART_RX_PIN         GPIO_PIN_10

// Function prototypes
void USART_Init(void);
void USART_SendString(const char *str);
void USART_SendData(uint8_t *data, uint16_t size);
void USART_Printf(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif // __USART_H
