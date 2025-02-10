# Projeto de Comunicação Serial com RP2040 e BitDogLab

Este projeto demonstra a integração de diversas interfaces de comunicação e controle de hardware utilizando o microcontrolador RP2040 e a placa BitDogLab. O sistema utiliza UART, I2C, controle via PIO de uma matriz de LEDs WS2812, display SSD1306 e botões com tratamento de interrupções e debounce.

## Funcionalidades

- **Comunicação UART:**  
  Recebe caracteres via UART/USB e exibe-os no display SSD1306.

- **Matriz WS2812:**  
  Ao receber um número (0-9) via UART, exibe um padrão correspondente na matriz 5x5 de LEDs.

- **Display SSD1306 (I2C):**  
  Exibe:
  - O caractere recebido via UART.
  - O número (ou símbolo) exibido na matriz WS2812.
  - O estado dos LEDs RGB.

- **Botões com Interrupções e Debounce:**  
  - **Botão A (GPIO 5):** Alterna o LED RGB Verde e atualiza o display e o Serial Monitor.
  - **Botão B (GPIO 6):** Alterna o LED RGB Azul e atualiza o display e o Serial Monitor.

- **Adição de Caracteres Minúsculos:**  
  A biblioteca `font.h` foi modificada para incluir caracteres minúsculos, permitindo a exibição de fontes completas no display.

## Requisitos de Hardware

- **Placa:** RP2040 / BitDogLab
- **Matriz de LEDs WS2812 (endereçáveis):** Conectada à GPIO 7
- **LED RGB:**  
  - Verde: GPIO 11  
  - Azul: GPIO 12 (ou GPIO 13, conforme sua implementação)
- **Botões:**  
  - Botão A: GPIO 5  
  - Botão B: GPIO 6
- **Display SSD1306:** Conectado via I2C (GPIO 14 e GPIO 15)

## Requisitos de Software

- SDK do Raspberry Pi Pico (RP2040)
- Bibliotecas para controle de UART, I2C, display SSD1306 e PIO para a matriz WS2812
- Compilador compatível com C/C++

## Estrutura do Projeto

```plaintext
.
├── main.c                      # Código-fonte principal do projeto
├── include
│   ├── font.h                  # Biblioteca de fontes (inclui caracteres)
│   ├── ssd1306.h               # Biblioteca para o display OLED SSD1306
│   ├── matriz_led_control.h    # Funções de controle da matriz WS2812
│   └── animacoesnumero.h       # Padrões dos números 0-9 para a matriz
├── pio_matrix.pio              # Código PIO para controle da matriz WS2812
└── README.md                   # Este arquivo
```

## Vídeo de Demonstração

-https://www.youtube.com/shorts/mJDqeERANdk

## Considerações Finais

Este projeto integra diversas técnicas de comunicação e controle em microcontroladores, proporcionando uma excelente oportunidade para aprender sobre:

- **Comunicação UART e I2C;**
- **Controle de LEDs comuns e endereçáveis (WS2812);**
- **Uso de interrupções e tratamento de debounce;**
- **Manipulação de displays OLED com fontes customizadas.**

## Autor

**Deividson Ribeiro Silva**






