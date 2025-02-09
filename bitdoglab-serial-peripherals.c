/**
 * main.c - Projeto: Comunicação Serial com RP2040 e BitDogLab
 *
 * Funcionalidades:
 *  - Recebe caracteres via UART/USB e os exibe no display SSD1306.
 *  - Ao digitar um número (0-9), exibe o símbolo correspondente na matriz 5x5 de LEDs WS2812.
 *  - Botão A: alterna o LED RGB Verde e registra a operação no display e no Serial Monitor.
 *  - Botão B: alterna o LED RGB Azul e registra a operação no display e no Serial Monitor.
 *  - Implementa interrupções e tratamento de debounce para os botões.
 *
 * Requisitos:
 *  - Comunicação UART para entrada de caracteres.
 *  - Comunicação I2C para o display SSD1306.
 *  - Controle de LED WS2812 via PIO.
 *  - Uso de interrupções para botões com debounce via software.
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"   // Necessário para clock_get_hz e CLOCKS_CLK_SYS
#include "hardware/i2c.h"
#include "hardware/uart.h"

#include "pio_matrix.pio.h"            // Rotina PIO para a matriz WS2812
#include "include/matriz_led_control.h" // Funções de controle da matriz WS2812
#include "include/animacoesnumero.h"    // Padrões dos números 0-9
#include "include/font.h"               // Font com caracteres (maiúsculas e minúsculas)
#include "include/ssd1306.h"            // Biblioteca para o display SSD1306

// ---------------------------------------------------------------------------
// Definições de pinos e configurações
// ---------------------------------------------------------------------------
#define OUT_PIN       7    // Pino da matriz WS2812
#define LED_G_PIN     11   // LED RGB – canal Verde
#define LED_B_PIN     12   // LED RGB – canal Azul
#define BUTTON_A      5    // Botão A
#define BUTTON_B      6    // Botão B
#define I2C_SDA       14   // Pino SDA do I2C
#define I2C_SCL       15   // Pino SCL do I2C

#define UART_ID       uart0
#define UART_TX_PIN   0
#define UART_RX_PIN   1

#define DEBOUNCE_TIME 300000  // 300 ms (em microsegundos)

// ---------------------------------------------------------------------------
// Variáveis globais e flags de atualização
// ---------------------------------------------------------------------------
volatile int numero_atual = 0;                   // Número atual para a matriz WS2812
static volatile uint32_t last_time_button_a = 0;   // Último acionamento do botão A
static volatile uint32_t last_time_button_b = 0;   // Último acionamento do botão B

bool led_verde_estado = false;  // Estado do LED RGB Verde
bool led_azul_estado  = false;  // Estado do LED RGB Azul

ssd1306_t display;  // Estrutura de controle do display SSD1306

// Estrutura para controle da matriz WS2812 via PIO
pio_t meu_pio = {
    .pio = pio0,
    .ok  = false,
    .i   = 0,
    .r   = 0.0,
    .g   = 0.0,
    .b   = 0.0,
    .sm  = 0
};

// Flags para atualização (usadas para evitar operações pesadas dentro das ISRs ou no loop)
volatile bool flag_atualiza_display = false;
volatile bool flag_atualiza_matriz  = false;
volatile char uart_char = 0; // Último caractere recebido via UART/USB

// ---------------------------------------------------------------------------
// Funções Auxiliares
// ---------------------------------------------------------------------------
/**
 * atualizar_matriz_led
 * ---------------------
 * Atualiza a matriz WS2812 com o padrão correspondente ao número atual.
 */
void atualizar_matriz_led() {
    printf("Atualizando matriz para: %d\n", numero_atual);
    switch (numero_atual) {
        case 0: desenho_pio(numero0, &meu_pio); break;
        case 1: desenho_pio(numero1, &meu_pio); break;
        case 2: desenho_pio(numero2, &meu_pio); break;
        case 3: desenho_pio(numero3, &meu_pio); break;
        case 4: desenho_pio(numero4, &meu_pio); break;
        case 5: desenho_pio(numero5, &meu_pio); break;
        case 6: desenho_pio(numero6, &meu_pio); break;
        case 7: desenho_pio(numero7, &meu_pio); break;
        case 8: desenho_pio(numero8, &meu_pio); break;
        case 9: desenho_pio(numero9, &meu_pio); break;
        default: break;
    }
}

/**
 * processa_display
 * ----------------
 * Atualiza o display OLED exibindo em uma única tela:
 *  - O caractere recebido via serial
 *  - O número exibido na matriz (se for um dígito, esse número é atualizado)
 *  - O status dos LEDs (Verde e Azul)
 */
void processa_display() {
    ssd1306_fill(&display, 0);  // Limpa o buffer do display
    char buf[50];

    // Exibe o caractere recebido (ou "-" se nenhum dado foi recebido)
    if (uart_char != 0) {
        snprintf(buf, sizeof(buf), "Serial: %c", uart_char);
    } else {
        snprintf(buf, sizeof(buf), "Serial: -");
    }
    ssd1306_draw_string(&display, buf, 0, 0);

    // Exibe o número exibido na matriz (se o caractere for dígito, ele atualiza o número)
    if (uart_char >= '0' && uart_char <= '9') {
        snprintf(buf, sizeof(buf), "Matriz: %c", uart_char);
    } else {
        snprintf(buf, sizeof(buf), "Matriz: %d", numero_atual);
    }
    ssd1306_draw_string(&display, buf, 0, 16);

    // Exibe o status dos LEDs
    snprintf(buf, sizeof(buf), "LED Verde: %s", led_verde_estado ? "ON" : "OFF");
    ssd1306_draw_string(&display, buf, 0, 32);
    snprintf(buf, sizeof(buf), "LED Azul: %s", led_azul_estado ? "ON" : "OFF");
    ssd1306_draw_string(&display, buf, 0, 48);

    ssd1306_send_data(&display);
}

/**
 * gpio_irq_handler
 * ----------------
 * Rotina de interrupção para os botões com debounce.
 * Registra a mudança de estado e seta a flag para atualização do display.
 */
void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    if (gpio == BUTTON_A && (current_time - last_time_button_a > DEBOUNCE_TIME)) {
        last_time_button_a = current_time;
        led_verde_estado = !led_verde_estado;
        gpio_put(LED_G_PIN, led_verde_estado);
        printf("Botao A: LED Verde %s\n", led_verde_estado ? "Ligado" : "Desligado");
        flag_atualiza_display = true;
    }
    else if (gpio == BUTTON_B && (current_time - last_time_button_b > DEBOUNCE_TIME)) {
        last_time_button_b = current_time;
        led_azul_estado = !led_azul_estado;
        gpio_put(LED_B_PIN, led_azul_estado);
        printf("Botao B: LED Azul %s\n", led_azul_estado ? "Ligado" : "Desligado");
        flag_atualiza_display = true;
    }
}

/**
 * Função Principal
 */
int main() {
    // Inicializa a saída serial (USB/STDIO) e outras interfaces
    stdio_init_all();
    sleep_ms(2000);
    printf("Iniciando programa...\n");

    // Inicializa a matriz WS2812 via PIO e exibe o padrão inicial (número 0)
    init_pio_routine(&meu_pio, OUT_PIN);
    atualizar_matriz_led();

    // Configura os pinos dos LEDs RGB como saída
    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);

    // Inicializa a interface I2C para o display OLED usando i2c1
    i2c_init(i2c1, 100 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display OLED (utilizando i2c1 como interface)
    ssd1306_init(&display, 128, 64, false, 0x3C, i2c1);
    ssd1306_config(&display);
    ssd1306_fill(&display, 0);
    ssd1306_draw_string(&display, "MINUSCULAS:", 0, 0);
    ssd1306_draw_string(&display, "a b c d e f g h i j k l m n o p q r s t u v w x y z", 0, 16);
    ssd1306_send_data(&display);
    printf("Display inicializado.\n");

    // Inicializa a UART para comunicação serial (usando polling via USB stdio)
    uart_init(UART_ID, 115200);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Configura os botões com pull-up interno
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);

    // Registra as interrupções para os botões (detecção na borda de descida)
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Loop principal: verifica via polling se há caracteres recebidos e atualiza display/matriz conforme as flags
    while (true) {
        int ch = getchar_timeout_us(10000);
        if (ch != PICO_ERROR_TIMEOUT && ch != EOF) {
            uart_char = (char)ch;
            flag_atualiza_display = true;
            if (uart_char >= '0' && uart_char <= '9') {
                numero_atual = uart_char - '0';
                flag_atualiza_matriz = true;
            }
        }
        if (flag_atualiza_display) {
            flag_atualiza_display = false;
            processa_display();
            uart_char = 0;  // Limpa o caractere para evitar reprocessamento
        }
        if (flag_atualiza_matriz) {
            flag_atualiza_matriz = false;
            atualizar_matriz_led();
        }
        tight_loop_contents();
    }
    
    return 0;
}
