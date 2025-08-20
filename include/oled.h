#ifndef __OLED_H
#define __OLED_H

#include "stm32f1xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// OLED display configuration
// The SSD1306 library uses I2C which is configured in board.h
// Default I2C address is usually 0x3C or 0x3D (handled by ssd1306 library)

// Initialize OLED display
void OLED_Init(void);

// Clear the entire display
void OLED_Clear(void);

// Update/refresh the display (send buffer to display)
void OLED_Update(void);

// Set cursor position for text
void OLED_SetCursor(uint8_t x, uint8_t y);

// Write a string at current cursor position
void OLED_WriteString(const char* str, uint8_t font_size, bool invert);

// Write a formatted string (like printf)
void OLED_Printf(uint8_t x, uint8_t y, uint8_t font_size, bool invert, const char* format, ...);

// Draw a pixel
void OLED_DrawPixel(uint8_t x, uint8_t y, bool on);

// Draw a line
void OLED_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool on);

// Draw a rectangle
void OLED_DrawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, bool filled, bool on);

// Draw a circle
void OLED_DrawCircle(uint8_t x, uint8_t y, uint8_t radius, bool on);

// Font size options
typedef enum {
    OLED_FONT_SMALL = 0,   // 7x10 font
    OLED_FONT_MEDIUM = 1,  // 11x18 font
    OLED_FONT_LARGE = 2    // 16x26 font (if available)
} OLED_FontSize_t;

// Display dimensions (128x64 for SSD1306)
#define OLED_WIDTH  128
#define OLED_HEIGHT 64

#ifdef __cplusplus
}
#endif

#endif // __OLED_H
