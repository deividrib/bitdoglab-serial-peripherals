# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if(EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define a placa como Pico W
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Importa o SDK do Raspberry Pi Pico
include(pico_sdk_import.cmake)

project(bitdoglab-serial-peripherals C CXX ASM)

# Inicializa o SDK do Raspberry Pi Pico
pico_sdk_init()

# Adiciona o executável e os arquivos fonte
add_executable(bitdoglab-serial-peripherals
    bitdoglab-serial-peripherals.c
    include/matriz_led_control.c
    include/ssd1306.c
)

pico_set_program_name(bitdoglab-serial-peripherals "bitdoglab-serial-peripherals")
pico_set_program_version(bitdoglab-serial-peripherals "0.1")

# Gera o header para o PIO
pico_generate_pio_header(bitdoglab-serial-peripherals ${CMAKE_CURRENT_LIST_DIR}/pio_matrix.pio)

# Configura a saída via USB e desativa a saída UART
pico_enable_stdio_uart(bitdoglab-serial-peripherals 1)
pico_enable_stdio_usb(bitdoglab-serial-peripherals 1)

# Adiciona a biblioteca padrão do Pico
target_link_libraries(bitdoglab-serial-peripherals pico_stdlib)

# Adiciona o diretório de includes
target_include_directories(bitdoglab-serial-peripherals PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# Linka as bibliotecas adicionais necessárias
target_link_libraries(bitdoglab-serial-peripherals
    hardware_spi
    hardware_i2c
    hardware_dma
    hardware_pio
    hardware_interp
    hardware_timer
    hardware_watchdog
    hardware_clocks
    hardware_pwm
    hardware_adc
    pico_bootrom
)

# Gera os arquivos de saída adicionais (UF2, etc)
pico_add_extra_outputs(bitdoglab-serial-peripherals)
