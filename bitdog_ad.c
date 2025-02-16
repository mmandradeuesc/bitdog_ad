#include <stdio.h>
#include <string.h> // Inclui a biblioteca para memset
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "ssd1306.h" // Inclui o cabeçalho do SSD1306

// Pin definitions
#define LED_RED_PIN 13
#define LED_BLUE_PIN 12
#define JOYSTICK_X_PIN 26
#define JOYSTICK_Y_PIN 27
#define JOYSTICK_PB 22
#define Botao_A 5

// Global variables for button states
volatile int border_style = 0;         // Border style: 0 = solid, 1 = dashed, 2 = dotted
volatile bool pwm_enabled = true;      // PWM enabled/disabled state

// Initial position of the square
int square_x = DISPLAY_WIDTH / 2 - 4;
int square_y = DISPLAY_HEIGHT / 2 - 4;

// Function to map ADC values (0-4095) to PWM range (0-255)
uint8_t map_adc_to_pwm(uint16_t adc_value) {
    if (adc_value < 2048) {
        return (uint8_t)((adc_value * 255) / 2048); // Scale down for lower half
    } else {
        return (uint8_t)(((adc_value - 2048) * 255) / 2048); // Scale up for upper half
    }
}

// Function to initialize PWM on a GPIO pin
void pwm_init_pin(uint gpio_pin) {
    gpio_set_function(gpio_pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio_pin);
    pwm_set_wrap(slice_num, 255); // Set PWM period to 255
    pwm_set_enabled(slice_num, true);
}

// Debounce function
bool debounce(uint gpio_pin) {
    static uint32_t last_time = 0;
    uint32_t current_time = time_us_32();
    if (current_time - last_time < 200000) { // 200ms debounce time
        return false;
    }
    last_time = current_time;
    return true;
}

// Interrupt handler for joystick button
void joystick_button_handler(uint gpio, uint32_t events) {
    if (!debounce(gpio)) return;

    // Cycle through border styles
    border_style = (border_style + 1) % 3;
}

// Interrupt handler for button A
void button_a_handler(uint gpio, uint32_t events) {
    if (!debounce(gpio)) return;

    // Toggle PWM enabled state
    pwm_enabled = !pwm_enabled;
    if (!pwm_enabled) {
        pwm_set_gpio_level(LED_RED_PIN, 0);
        pwm_set_gpio_level(LED_BLUE_PIN, 0);
    }
}

int main() {
    // Initialize stdio for debugging
    stdio_init_all();

    // Initialize SSD1306 display
    ssd1306_init();

    // Initialize ADC for joystick
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    // Initialize GPIOs for buttons
    gpio_init(JOYSTICK_PB);
    gpio_set_dir(JOYSTICK_PB, GPIO_IN);
    gpio_pull_up(JOYSTICK_PB);
    gpio_init(Botao_A);
    gpio_set_dir(Botao_A, GPIO_IN);
    gpio_pull_up(Botao_A);

    // Enable interrupts for buttons
    gpio_set_irq_enabled_with_callback(JOYSTICK_PB, GPIO_IRQ_EDGE_FALL, true, &joystick_button_handler);
    gpio_set_irq_enabled_with_callback(Botao_A, GPIO_IRQ_EDGE_FALL, true, &button_a_handler);

    // Initialize PWM for LEDs
    pwm_init_pin(LED_RED_PIN);
    pwm_init_pin(LED_BLUE_PIN);

    while (true) {
        // Read joystick X and Y values
        adc_select_input(0); // X-axis
        uint16_t x_val = adc_read();
        adc_select_input(1); // Y-axis
        uint16_t y_val = adc_read();

        // Update LED intensities
        if (pwm_enabled) {
            uint slice_red = pwm_gpio_to_slice_num(LED_RED_PIN);
            uint slice_blue = pwm_gpio_to_slice_num(LED_BLUE_PIN);
            pwm_set_gpio_level(LED_RED_PIN, map_adc_to_pwm(x_val));  // Red LED controlled by X-axis
            pwm_set_gpio_level(LED_BLUE_PIN, map_adc_to_pwm(y_val)); // Blue LED controlled by Y-axis
        }

        // Map joystick values to display coordinates
        int new_x = (x_val * (DISPLAY_WIDTH - 8)) / 4095; // Scale X to fit display width
        int new_y = (y_val * (DISPLAY_HEIGHT - 8)) / 4095; // Scale Y to fit display height

        // Update square position if it has changed
        if (new_x != square_x || new_y != square_y) {
            square_x = new_x;
            square_y = new_y;
        }

        // Clear display buffer
        ssd1306_clear();

        // Draw border based on current style
        switch (border_style) {
            case 0: // Solid border
                for (int i = 0; i < DISPLAY_WIDTH; i++) {
                    ssd1306_draw_pixel(i, 0, true);
                    ssd1306_draw_pixel(i, DISPLAY_HEIGHT - 1, true);
                }
                for (int i = 0; i < DISPLAY_HEIGHT; i++) {
                    ssd1306_draw_pixel(0, i, true);
                    ssd1306_draw_pixel(DISPLAY_WIDTH - 1, i, true);
                }
                break;
            case 1: // Dashed border
                for (int i = 0; i < DISPLAY_WIDTH; i += 4) {
                    ssd1306_draw_pixel(i, 0, true);
                    ssd1306_draw_pixel(i, DISPLAY_HEIGHT - 1, true);
                }
                for (int i = 0; i < DISPLAY_HEIGHT; i += 4) {
                    ssd1306_draw_pixel(0, i, true);
                    ssd1306_draw_pixel(DISPLAY_WIDTH - 1, i, true);
                }
                break;
            case 2: // Dotted border
                for (int i = 0; i < DISPLAY_WIDTH; i += 8) {
                    ssd1306_draw_pixel(i, 0, true);
                    ssd1306_draw_pixel(i, DISPLAY_HEIGHT - 1, true);
                }
                for (int i = 0; i < DISPLAY_HEIGHT; i += 8) {
                    ssd1306_draw_pixel(0, i, true);
                    ssd1306_draw_pixel(DISPLAY_WIDTH - 1, i, true);
                }
                break;
        }

        // Draw square
        for (int i = square_x; i < square_x + 8; i++) {
            for (int j = square_y; j < square_y + 8; j++) {
                ssd1306_draw_pixel(i, j, true);
            }
        }

        // Update display
        ssd1306_update();

        // Small delay to avoid excessive CPU usage
        sleep_ms(50);
    }
}