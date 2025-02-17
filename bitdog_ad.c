#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "ssd1306.h"

// Definição GPIO
#define LED_RED_PIN 13
#define LED_GREEN_PIN 11
#define LED_BLUE_PIN 12
#define JOYSTICK_X_PIN 26
#define JOYSTICK_Y_PIN 27
#define JOYSTICK_PB 22
#define Botao_A 5

// Variáveis globais
volatile int border_style = 0;         
volatile bool pwm_enabled = true;      
volatile bool green_led_state = false; 
volatile int prev_square_x = -1;
volatile int prev_square_y = -1;
volatile int prev_border_style = -1;

// Posição quadrada (iniciar no centro)
volatile int square_x = 60;  // (128/2 - 4)
volatile int square_y = 28;  // (64/2 - 4)

// Manipulador de interrupção genérico para todos os botões
void gpio_callback(uint gpio, uint32_t events) {
    static uint32_t last_time = 0;
    uint32_t current_time = time_us_32();
    
    // Debounce checagem
    if (current_time - last_time < 200000) return; // 200ms debounce
    last_time = current_time;

    if (gpio == JOYSTICK_PB) {
        // Alternar LED verde
        green_led_state = !green_led_state;
        gpio_put(LED_GREEN_PIN, green_led_state);
        //Atualizar estilo de borda
        border_style = (border_style + 1) % 3;
    } 
    else if (gpio == Botao_A) {
        pwm_enabled = !pwm_enabled;
        if (!pwm_enabled) {
            pwm_set_gpio_level(LED_RED_PIN, 0);
            pwm_set_gpio_level(LED_BLUE_PIN, 0);
        }
    }
}

// Função intensidade dos leds joystick 
uint8_t map_joystick_to_led(uint16_t val) {
    if (val < 1948) {
        return (uint8_t)((1948 - val) * 255 / 1948);
    } else if (val > 2148) {
        return (uint8_t)((val - 2148) * 255 / 1947);
    }
    return 0;
}

// Mapeamento do josysttick X para o quadrado
int map_to_display(uint16_t val, int max_pos, bool is_x) {
    // Remove a inversão do valor do ADC para o eixo X
    if (!is_x) {
        val = 4095 - val; // Mantém a inversão apenas para o eixo Y
    }
    
    // Calcula o centro e range
    int center = is_x ? (DISPLAY_WIDTH - 8) / 2 : (DISPLAY_HEIGHT - 8) / 2;
    int range = is_x ? (DISPLAY_WIDTH - 8) / 2 : (DISPLAY_HEIGHT - 8) / 2;
    
    if (val < 1948) {
        return center - ((1948 - val) * range / 1948);
    } else if (val > 2148) {
        return center + ((val - 2148) * range / 1947);
    }
    return center;
}

int main() {
    stdio_init_all();
    ssd1306_init();
    ssd1306_clear();
    ssd1306_update();

    // Inicializando o display
    ssd1306_init();
    
    // Inicializando o  ADC
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    // Initialize LEDs
    // Led Verde (normal GPIO)
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    
    // Lendo led Azul (PWM)
    gpio_set_function(LED_RED_PIN, GPIO_FUNC_PWM);
    gpio_set_function(LED_BLUE_PIN, GPIO_FUNC_PWM);
    
    uint slice_red = pwm_gpio_to_slice_num(LED_RED_PIN);
    uint slice_blue = pwm_gpio_to_slice_num(LED_BLUE_PIN);
    
    pwm_set_wrap(slice_red, 255);
    pwm_set_wrap(slice_blue, 255);
    
    pwm_set_enabled(slice_red, true);
    pwm_set_enabled(slice_blue, true);

    // Inicialização botões
    gpio_init(JOYSTICK_PB);
    gpio_set_dir(JOYSTICK_PB, GPIO_IN);
    gpio_pull_up(JOYSTICK_PB);
    
    gpio_init(Botao_A);
    gpio_set_dir(Botao_A, GPIO_IN);
    gpio_pull_up(Botao_A);

    // Interrupções
    gpio_set_irq_enabled_with_callback(JOYSTICK_PB, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled(Botao_A, GPIO_IRQ_EDGE_FALL, true);

    while (true) {
        // Leitura do joystick
        adc_select_input(0);
        uint16_t y_val = adc_read();
        adc_select_input(1);
        uint16_t x_val = adc_read();
    
        // Atualização dos LEDs
        if (pwm_enabled) {
            pwm_set_gpio_level(LED_RED_PIN, map_joystick_to_led(x_val));
            pwm_set_gpio_level(LED_BLUE_PIN, map_joystick_to_led(y_val));
        }
    
        // Calcula as novas posições
        int new_square_x = map_to_display(x_val, DISPLAY_WIDTH, true);
        int new_square_y = map_to_display(y_val, DISPLAY_HEIGHT, false);
    
        // Só atualiza o display se houver mudança na posição ou no estilo da borda
        if (new_square_x != prev_square_x || 
            new_square_y != prev_square_y || 
            border_style != prev_border_style) {
            
            ssd1306_clear();
    
            // Desenho da borda
            for (int i = 0; i < DISPLAY_WIDTH; i++) {
                if (border_style == 0 || 
                    (border_style == 1 && i % 4 == 0) || 
                    (border_style == 2 && i % 8 == 0)) {
                    ssd1306_draw_pixel(i, 0, true);
                    ssd1306_draw_pixel(i, 1, true);
                    ssd1306_draw_pixel(i, DISPLAY_HEIGHT - 1, true);
                    ssd1306_draw_pixel(i, DISPLAY_HEIGHT - 2, true);
                }
            }
            for (int i = 0; i < DISPLAY_HEIGHT; i++) {
                if (border_style == 0 || 
                    (border_style == 1 && i % 4 == 0) ||
                    (border_style == 2 && i % 8 == 0)) {
                    ssd1306_draw_pixel(0, i, true);
                    ssd1306_draw_pixel(1, i, true);
                    ssd1306_draw_pixel(DISPLAY_WIDTH - 1, i, true);
                    ssd1306_draw_pixel(DISPLAY_WIDTH - 2, i, true);
                }
            }
    
            // Desenho do quadrado
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    if (new_square_x + i >= 0 && new_square_x + i < DISPLAY_WIDTH &&
                        new_square_y + j >= 0 && new_square_y + j < DISPLAY_HEIGHT) {
                        ssd1306_draw_pixel(new_square_x + i, new_square_y + j, true);
                    }
                }
            }
    
            // Atualiza o display
            ssd1306_update();
    
            // Armazena as posições atuais
            prev_square_x = new_square_x;
            prev_square_y = new_square_y;
            prev_border_style = border_style;
        }
    
        sleep_ms(20);
    }

    return 0;
}