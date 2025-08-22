#include "usart.h"
#include "board.h"
#include <stdarg.h>

// USART handle
static UART_HandleTypeDef huart1;

// Initialize USART1 for serial communication
// PA9  = TX, PA10 = RX, 115200 baud, 8N1
void USART_Init(void)
{
    // Enable clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();

    // Configure GPIO pins for USART1
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // PA9 - USART1_TX (Alternate function push-pull)
    GPIO_InitStruct.Pin = USART_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(USART_PORT, &GPIO_InitStruct);

    // PA10 - USART1_RX (Input floating)
    GPIO_InitStruct.Pin = USART_RX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(USART_PORT, &GPIO_InitStruct);

    // Configure USART1
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart1) != HAL_OK) {
        // Initialization Error
        Error_Handler();
    }
}

// Send a string via USART
void USART_SendString(const char *str)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

// Send raw data via USART
void USART_SendData(uint8_t *data, uint16_t size)
{
    HAL_UART_Transmit(&huart1, data, size, HAL_MAX_DELAY);
}

// Printf-style function for USART
void USART_Printf(const char *format, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    USART_SendString(buffer);
}

// Override printf to redirect to USART (optional, for standard printf support)
#ifdef __GNUC__
int _write(int file, char *ptr, int len)
{
    (void)file; // Unused parameter
    HAL_UART_Transmit(&huart1, (uint8_t*)ptr, len, HAL_MAX_DELAY);
    return len;
}
#endif
