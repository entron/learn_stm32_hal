#include "usart.h"
#include "board.h"
#include <stdarg.h>
#include <ctype.h>

// USART handle (global so IRQ handler in stm32f1xx_it.c can access it)
UART_HandleTypeDef huart1;

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

    // Enable USART1 interrupt in NVIC (required for HAL_UART_Receive_IT)
    HAL_NVIC_SetPriority(USART1_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
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


static uint8_t rx_byte;

// Line buffer (simple single-buffer queue)
#define USART_LINE_BUF_SIZE 64
static char line_buf[USART_LINE_BUF_SIZE];
static volatile uint8_t line_len = 0;        // current building length
static volatile uint8_t line_ready = 0;      // flag when a full line is ready

int USART_LineAvailable(void) {
    return line_ready; // 1 if a line is ready
}

void USART_GetLine(char *dest, size_t maxlen) {
    if (!line_ready) { if (maxlen) dest[0] = '\0'; return; }
    size_t n = line_len;
    if (n >= maxlen) n = maxlen - 1;
    memcpy(dest, line_buf, n);
    dest[n] = '\0';
    line_len = 0;
    line_ready = 0;
}

void USART_Start(void) {
    HAL_UART_Receive_IT(&huart1, &rx_byte, 1); // start RX
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *h) {
    if (h->Instance == USART1) {
        uint8_t c = rx_byte;
        if (!line_ready) {
            if (c == '\r' || c == '\n') {
                if (line_len > 0) { // only finalize non-empty lines
                    line_buf[line_len] = '\0';
                    line_ready = 1; // make available
                } // else ignore stray line endings
            } else {
                if (line_len < (USART_LINE_BUF_SIZE - 1)) {
                    line_buf[line_len++] = (char)c;
                } else {
                    // overflow: flush buffer
                    line_len = 0;
                }
            }
        }
        HAL_UART_Receive_IT(&huart1, &rx_byte, 1); // re-arm reception quickly
    }
}