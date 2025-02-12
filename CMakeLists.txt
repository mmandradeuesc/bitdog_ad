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
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# == Configuração do ambiente para Raspberry Pi Pico ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define a placa como Raspberry Pi Pico W
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Importa o SDK do Raspberry Pi Pico
include(pico_sdk_import.cmake)

# Define o nome do projeto
project(bitdog_ad C CXX ASM)

# Inicializa o SDK do Pico
pico_sdk_init()

# Adiciona o executável ao projeto com os arquivos necessários
add_executable(bitdog_ad 
    bitdog_ad.c 
    ssd1306.c
)

# Define nome e versão do programa
pico_set_program_name(bitdog_ad "bitdog_ad")
pico_set_program_version(bitdog_ad "0.1")

# Configura a saída do programa (desativa UART e ativa USB)
pico_enable_stdio_uart(bitdog_ad 1)
pico_enable_stdio_usb(bitdog_ad 1)

# Gera cabeçalhos para PIO
pico_generate_pio_header(bitdog_ad ${CMAKE_CURRENT_LIST_DIR}/bitdog_ad.pio)

# Adiciona diretórios de cabeçalhos
target_include_directories(bitdog_ad PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR}  # Inclui arquivos no diretório principal
)

# Adiciona bibliotecas necessárias para o funcionamento
target_link_libraries(bitdog_ad PRIVATE
    pico_stdlib
    hardware_adc
    hardware_pwm
    hardware_i2c
)

# Gera os arquivos binários necessários para gravação no Pico
pico_add_extra_outputs(bitdog_ad)
