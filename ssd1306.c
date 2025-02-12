// ssd1306.c
#include "ssd1306.h"
#include "hardware/i2c.h"
#include <string.h>

// Display buffer
static uint8_t buffer[SSD1306_HEIGHT * SSD1306_WIDTH / 8];

// SSD1306 commands
#define SSD1306_DISPLAYOFF          0xAE
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5
#define SSD1306_SETMULTIPLEX        0xA8
#define SSD1306_SETDISPLAYOFFSET    0xD3
#define SSD1306_SETSTARTLINE        0x40
#define SSD1306_CHARGEPUMP          0x8D
#define SSD1306_MEMORYMODE          0x20
#define SSD1306_SEGREMAP            0xA0
#define SSD1306_COMSCANDEC          0xC8
#define SSD1306_SETCOMPINS          0xDA
#define SSD1306_SETCONTRAST         0x81
#define SSD1306_SETPRECHARGE        0xD9
#define SSD1306_SETVCOMDETECT       0xDB
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_NORMALDISPLAY       0xA6
#define SSD1306_DISPLAYON           0xAF

// Helper function to send command
static void send_command(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd}; // First byte is Control byte (0x00 for command)
    i2c_write_blocking(i2c0, SSD1306_ADDRESS, buf, 2, false);
}

void ssd1306_init(void) {
    sleep_ms(100);  // Wait for display to power up
    
    // Initialize display
    send_command(SSD1306_DISPLAYOFF);
    send_command(SSD1306_SETDISPLAYCLOCKDIV);
    send_command(0x80);
    send_command(SSD1306_SETMULTIPLEX);
    send_command(0x3F);
    send_command(SSD1306_SETDISPLAYOFFSET);
    send_command(0x00);
    send_command(SSD1306_SETSTARTLINE | 0x00);
    send_command(SSD1306_CHARGEPUMP);
    send_command(0x14);
    send_command(SSD1306_MEMORYMODE);
    send_command(0x00);
    send_command(SSD1306_SEGREMAP | 0x01);
    send_command(SSD1306_COMSCANDEC);
    send_command(SSD1306_SETCOMPINS);
    send_command(0x12);
    send_command(SSD1306_SETCONTRAST);
    send_command(0xCF);
    send_command(SSD1306_SETPRECHARGE);
    send_command(0xF1);
    send_command(SSD1306_SETVCOMDETECT);
    send_command(0x40);
    send_command(SSD1306_DISPLAYALLON_RESUME);
    send_command(SSD1306_NORMALDISPLAY);
    send_command(SSD1306_DISPLAYON);

    memset(buffer, 0, sizeof(buffer));
}

void ssd1306_clear(void) {
    memset(buffer, 0, sizeof(buffer));
}

void ssd1306_show(void) {
    uint8_t data[1025];
    data[0] = 0x40; // Data mode

    for (int i = 0; i < sizeof(buffer); i++) {
        data[i + 1] = buffer[i];
    }

    i2c_write_blocking(i2c0, SSD1306_ADDRESS, data, sizeof(buffer) + 1, false);
}

void ssd1306_draw_pixel(int x, int y, int color) {
    if (x < 0 || x >= SSD1306_WIDTH || y < 0 || y >= SSD1306_HEIGHT)
        return;

    if (color)
        buffer[x + (y/8)*SSD1306_WIDTH] |= (1 << (y&7));
    else
        buffer[x + (y/8)*SSD1306_WIDTH] &= ~(1 << (y&7));
}

void ssd1306_draw_rectangle(int x, int y, int w, int h, int color) {
    for (int i = x; i < x + w; i++) {
        ssd1306_draw_pixel(i, y, color);
        ssd1306_draw_pixel(i, y + h - 1, color);
    }
    for (int i = y; i < y + h; i++) {
        ssd1306_draw_pixel(x, i, color);
        ssd1306_draw_pixel(x + w - 1, i, color);
    }
}

void ssd1306_fill_rectangle(int x, int y, int w, int h, int color) {
    for (int i = x; i < x + w; i++) {
        for (int j = y; j < y + h; j++) {
            ssd1306_draw_pixel(i, j, color);
        }
    }
}

void ssd1306_draw_dashed_rectangle(int x, int y, int w, int h, int color) {
    for (int i = x; i < x + w; i += 2) {
        ssd1306_draw_pixel(i, y, color);
        ssd1306_draw_pixel(i, y + h - 1, color);
    }
    for (int i = y; i < y + h; i += 2) {
        ssd1306_draw_pixel(x, i, color);
        ssd1306_draw_pixel(x + w - 1, i, color);
    }
}

void ssd1306_draw_dotted_rectangle(int x, int y, int w, int h, int color) {
    for (int i = x; i < x + w; i += 3) {
        ssd1306_draw_pixel(i, y, color);
        ssd1306_draw_pixel(i, y + h - 1, color);
    }
    for (int i = y; i < y + h; i += 3) {
        ssd1306_draw_pixel(x, i, color);
        ssd1306_draw_pixel(x + w - 1, i, color);
    }
}