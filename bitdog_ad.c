#include <stdio.h>
#include <stdlib.h> 
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

// Pin definitions
#define LED_RED_PIN 11
#define LED_GREEN_PIN 12
#define LED_BLUE_PIN 13
#define JOYSTICK_BTN_PIN 22
#define BUTTON_A_PIN 5
#define JOYSTICK_X_PIN 26
#define JOYSTICK_Y_PIN 27
#define I2C_SDA_PIN 14
#define I2C_SCL_PIN 15


// Display constants
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define SQUARE_SIZE 8

// Global variables
volatile bool led_pwm_enabled = true;
volatile bool green_led_state = false;
volatile uint8_t border_style = 0;
volatile uint32_t last_btn_time = 0;
const uint32_t DEBOUNCE_DELAY = 200000; // 200ms in microseconds

// Structure for square position
typedef struct {
    int x;
    int y;
} Position;

Position square_pos = {
    .x = (DISPLAY_WIDTH - SQUARE_SIZE) / 2,
    .y = (DISPLAY_HEIGHT - SQUARE_SIZE) / 2
};

// Function prototypes
void setup_gpio(void);
void setup_pwm(void);
void setup_adc(void);
void setup_interrupts(void);
void update_leds(uint16_t x_val, uint16_t y_val);
void update_display(void);
void joystick_btn_callback(uint gpio, uint32_t events);
void button_a_callback(uint gpio, uint32_t events);

// Interrupt callbacks with debouncing
void joystick_btn_callback(uint gpio, uint32_t events) {
    uint32_t current_time = time_us_32();
    if (current_time - last_btn_time < DEBOUNCE_DELAY) {
        return;
    }
    last_btn_time = current_time;

    // Toggle green LED
    green_led_state = !green_led_state;
    gpio_put(LED_GREEN_PIN, green_led_state);

    // Cycle border style
    border_style = (border_style + 1) % 3;
    update_display();
}

void button_a_callback(uint gpio, uint32_t events) {
    uint32_t current_time = time_us_32();
    if (current_time - last_btn_time < DEBOUNCE_DELAY) {
        return;
    }
    last_btn_time = current_time;

    // Toggle PWM LED enable state
    led_pwm_enabled = !led_pwm_enabled;
    if (!led_pwm_enabled) {
        pwm_set_gpio_level(LED_RED_PIN, 0);
        pwm_set_gpio_level(LED_BLUE_PIN, 0);
    }
}

void setup_gpio(void) {
    // Initialize LED pins
    gpio_init(LED_RED_PIN);
    gpio_init(LED_GREEN_PIN);
    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);

    // Initialize button pins with pull-ups
    gpio_init(JOYSTICK_BTN_PIN);
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(JOYSTICK_BTN_PIN, GPIO_IN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_BTN_PIN);
    gpio_pull_up(BUTTON_A_PIN);
}

void setup_pwm(void) {
    // Configure PWM for RED and BLUE LEDs
    gpio_set_function(LED_RED_PIN, GPIO_FUNC_PWM);
    gpio_set_function(LED_BLUE_PIN, GPIO_FUNC_PWM);

    uint slice_red = pwm_gpio_to_slice_num(LED_RED_PIN);
    uint slice_blue = pwm_gpio_to_slice_num(LED_BLUE_PIN);

    pwm_set_wrap(slice_red, 255);
    pwm_set_wrap(slice_blue, 255);

    pwm_set_enabled(slice_red, true);
    pwm_set_enabled(slice_blue, true);
}

void setup_adc(void) {
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);
}

void setup_interrupts(void) {
    gpio_set_irq_enabled_with_callback(JOYSTICK_BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &joystick_btn_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &button_a_callback);
}

void update_leds(uint16_t x_val, uint16_t y_val) {
    if (!led_pwm_enabled) {
        return;
    }

    // Calculate LED intensity based on joystick position
    uint8_t red_intensity = abs(x_val - 2048) * 255 / 2048;
    uint8_t blue_intensity = abs(y_val - 2048) * 255 / 2048;

    pwm_set_gpio_level(LED_RED_PIN, red_intensity);
    pwm_set_gpio_level(LED_BLUE_PIN, blue_intensity);
}

void update_display(void) {
    // Clear display
    ssd1306_clear();

    // Draw border based on current style
    switch (border_style) {
        case 0:
            ssd1306_draw_rectangle(0, 0, DISPLAY_WIDTH-1, DISPLAY_HEIGHT-1, 1);
            break;
        case 1:
            ssd1306_draw_dashed_rectangle(0, 0, DISPLAY_WIDTH-1, DISPLAY_HEIGHT-1, 1);
            break;
        case 2:
            ssd1306_draw_dotted_rectangle(0, 0, DISPLAY_WIDTH-1, DISPLAY_HEIGHT-1, 1);
            break;
    }

    // Draw square at current position
    ssd1306_fill_rectangle(square_pos.x, square_pos.y, SQUARE_SIZE, SQUARE_SIZE, 1);
    
    // Update display
    ssd1306_show();
}

int main() {
    stdio_init_all();

    // Initialize all subsystems
    setup_gpio();
    setup_pwm();
    setup_adc();
    setup_interrupts();

    // Initialize I2C for display
    i2c_init(i2c0, 400000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    // Initialize display
    ssd1306_init();
    ssd1306_clear();
    update_display();

    while (1) {
        // Read joystick values
        adc_select_input(0);
        uint16_t x_val = adc_read();
        adc_select_input(1);
        uint16_t y_val = adc_read();

        // Update LED intensity
        update_leds(x_val, y_val);

        // Update square position
        square_pos.x = ((DISPLAY_WIDTH - SQUARE_SIZE) * x_val) / 4095;
        square_pos.y = ((DISPLAY_HEIGHT - SQUARE_SIZE) * y_val) / 4095;

        // Update display
        update_display();

        sleep_ms(20); // Small delay to prevent display flickering
    }

    return 0;
}