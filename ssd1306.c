#include "ssd1306.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <string.h>

// Display buffer
uint8_t buffer[DISPLAY_HEIGHT * DISPLAY_WIDTH / 8];

// Function to send commands to SSD1306
void ssd1306_send_command(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd}; // Control byte (0x00 for command)
    i2c_write_blocking(I2C_PORT, endereco, buf, 2, false);
}

// Function to send data to SSD1306
void ssd1306_send_data(uint8_t *data, size_t len) {
    uint8_t control_byte = 0x40; // Control byte (0x40 for data)
    i2c_write_blocking(I2C_PORT, endereco, &control_byte, 1, true); // Send control byte
    i2c_write_blocking(I2C_PORT, endereco, data, len, false);       // Send data
}

// Function to initialize SSD1306
void ssd1306_init() {
    sleep_ms(100); // Wait for display to initialize

    // Initialize I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Initialize display with necessary commands
    ssd1306_send_command(0xAE); // Display OFF
    ssd1306_send_command(0xD5); // Set display clock divide ratio/oscillator frequency
    ssd1306_send_command(0x80);
    ssd1306_send_command(0xA8); // Set multiplex ratio
    ssd1306_send_command(0x3F);
    ssd1306_send_command(0xD3); // Set display offset
    ssd1306_send_command(0x00);
    ssd1306_send_command(0x40); // Set display start line
    ssd1306_send_command(0x8D); // Charge pump
    ssd1306_send_command(0x14); // Enable charge pump
    ssd1306_send_command(0x20); // Set memory mode
    ssd1306_send_command(0x00); // Horizontal addressing mode
    ssd1306_send_command(0xA1); // Set segment remap
    ssd1306_send_command(0xC8); // Set COM output scan direction
    ssd1306_send_command(0xDA); // Set COM pins hardware configuration
    ssd1306_send_command(0x12);
    ssd1306_send_command(0x81); // Set contrast control
    ssd1306_send_command(0xCF);
    ssd1306_send_command(0xD9); // Set pre-charge period
    ssd1306_send_command(0xF1);
    ssd1306_send_command(0xDB); // Set VCOMH deselect level
    ssd1306_send_command(0x40);
    ssd1306_send_command(0xA4); // Entire display ON
    ssd1306_send_command(0xA6); // Set normal/inverse display
    ssd1306_send_command(0xAF); // Display ON

    // Clear display buffer
    memset(buffer, 0, sizeof(buffer));
    ssd1306_send_data(buffer, sizeof(buffer));
}

// Function to clear the display
void ssd1306_clear() {
    memset(buffer, 0, sizeof(buffer));
}

// Function to draw a pixel on the display
void ssd1306_draw_pixel(int x, int y, bool color) {
    if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT) return;
    if (color) {
        buffer[x + (y / 8) * DISPLAY_WIDTH] |= (1 << (y % 8));
    } else {
        buffer[x + (y / 8) * DISPLAY_WIDTH] &= ~(1 << (y % 8));
    }
}

// Function to update the display
void ssd1306_update() {
    ssd1306_send_data(buffer, sizeof(buffer));
}