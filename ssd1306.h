// ssd1306.h
#ifndef SSD1306_H
#define SSD1306_H

#include "hardware/i2c.h"
#include <stdint.h>

// Display constants
#define SSD1306_HEIGHT 64
#define SSD1306_WIDTH 128
#define SSD1306_ADDRESS 0x3C

// Function declarations
void ssd1306_init(void);
void ssd1306_clear(void);
void ssd1306_show(void);
void ssd1306_draw_pixel(int x, int y, int color);
void ssd1306_draw_rectangle(int x, int y, int w, int h, int color);
void ssd1306_fill_rectangle(int x, int y, int w, int h, int color);
void ssd1306_draw_dashed_rectangle(int x, int y, int w, int h, int color);
void ssd1306_draw_dotted_rectangle(int x, int y, int w, int h, int color);

#endif // SSD1306_H