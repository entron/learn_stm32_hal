#include "oled.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "stm32f1xx_hal_i2c.h"
#include <stdio.h>
#include <stdarg.h>

// I2C handle for OLED communication
I2C_HandleTypeDef hi2c1;

// Static function to initialize I2C for OLED
static void OLED_I2C_Init(void);

void OLED_Init(void)
{
    // Initialize I2C first, then the display
    OLED_I2C_Init();
    ssd1306_Init();
}

static void OLED_I2C_Init(void)
{
    // Enable clocks
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();

    // Remap I2C1 to PB8/PB9 (STM32F1 remap)
    __HAL_AFIO_REMAP_I2C1_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    // PB8 -> SCL, PB9 -> SDA
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
        // Error_Handler();  // We could add error handling here
    }
}

void OLED_Clear(void)
{
    ssd1306_Fill(Black);
}

void OLED_Update(void)
{
    ssd1306_UpdateScreen();
}

void OLED_SetCursor(uint8_t x, uint8_t y)
{
    ssd1306_SetCursor(x, y);
}

void OLED_WriteString(const char* str, uint8_t font_size, bool invert)
{
    SSD1306_COLOR color = invert ? Black : White;
    
    switch(font_size) {
        case OLED_FONT_SMALL:
            ssd1306_WriteString((char*)str, Font_7x10, color);
            break;
        case OLED_FONT_MEDIUM:
            ssd1306_WriteString((char*)str, Font_11x18, color);
            break;
        case OLED_FONT_LARGE:
            ssd1306_WriteString((char*)str, Font_16x26, color);
            break;
        default:
            ssd1306_WriteString((char*)str, Font_11x18, color);
            break;
    }
}

void OLED_Printf(uint8_t x, uint8_t y, uint8_t font_size, bool invert, const char* format, ...)
{
    char buffer[64];  // Adjust size as needed
    va_list args;
    
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    OLED_SetCursor(x, y);
    OLED_WriteString(buffer, font_size, invert);
}

void OLED_DrawPixel(uint8_t x, uint8_t y, bool on)
{
    SSD1306_COLOR color = on ? White : Black;
    ssd1306_DrawPixel(x, y, color);
}

void OLED_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool on)
{
    SSD1306_COLOR color = on ? White : Black;
    ssd1306_Line(x0, y0, x1, y1, color);
}

void OLED_DrawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, bool filled, bool on)
{
    SSD1306_COLOR color = on ? White : Black;
    
    if (filled) {
        ssd1306_FillRectangle(x, y, x + width - 1, y + height - 1, color);
    } else {
        ssd1306_DrawRectangle(x, y, x + width - 1, y + height - 1, color);
    }
}

void OLED_DrawCircle(uint8_t x, uint8_t y, uint8_t radius, bool on)
{
    SSD1306_COLOR color = on ? White : Black;
    ssd1306_DrawCircle(x, y, radius, color);
}
