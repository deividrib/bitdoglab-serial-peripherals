# 📡 Comunicação Serial com RP2040 e BitDogLab  

Este projeto implementa um sistema interativo utilizando a placa **RP2040 BitDogLab**, explorando **interfaces de comunicação serial (UART e I2C)**, **interrupções**, **debounce** e **controle de LEDs WS2812**.  

## 🎯 **Objetivos**  
✅ Receber caracteres via **UART** e exibi-los no **display SSD1306 (I2C)**  
✅ Exibir números na **matriz de LEDs WS2812 (5x5)** ao receber um dígito (0-9)  
✅ Alternar **LEDs RGB** com botões físicos (usando interrupções e debounce)  
✅ Exibir informações do sistema no **Serial Monitor** e no **display OLED**  

## 🛠 **Componentes Utilizados**  
- **RP2040 BitDogLab**  
- **Matriz de LEDs WS2812 (5x5)** – GPIO 7  
- **Display SSD1306 (I2C)** – GPIO 14 (SDA) e GPIO 15 (SCL)  
- **LED RGB** – GPIO 11 (Azul) e GPIO 12 (Verde)  
- **Botões**  
  - Botão A – GPIO 5 (Alterna LED Verde)  
  - Botão B – GPIO 6 (Alterna LED Azul)  

## ⚙ **Funcionamento**  

### 1️⃣ **Entrada de Caracteres via UART**  
- O usuário digita um **caractere no Serial Monitor** (VS Code, PuTTY, etc.).  
- O caractere recebido é exibido no **display SSD1306**.  
- Se for um número (0-9), a **matriz de LEDs WS2812** exibe um padrão correspondente.  

### 2️⃣ **Interação com Botões**  
- **Botão A (GPIO 5):** Liga/desliga o **LED Verde** e exibe uma mensagem no display e Serial Monitor.  
- **Botão B (GPIO 6):** Liga/desliga o **LED Azul** e exibe uma mensagem no display e Serial Monitor.  
- O **debounce** é tratado via software para evitar leituras falsas.  

### 3️⃣ **Exibição no Display SSD1306**  
- O display exibe:  
  - O último caractere recebido via UART  
  - O número exibido na matriz de LEDs  
  - O status dos LEDs Verde e Azul  

## 🔧 **Como Executar**  

### 📥 **1. Clonar o Repositório**  
```sh
git clone https://github.com/seuusuario/projeto-rp2040-bitdoglab.git
cd projeto-rp2040-bitdoglab
