#include "oled.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>
#include <stdarg.h>

void OLED_Init(void)
{
    // Initialize the SSD1306 OLED display
    // The I2C peripheral should already be initialized by Board_Init()
    ssd1306_Init();
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
