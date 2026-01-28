cmake_minimum_required(VERSION 3.12)

# Definimos que é um projeto C e Assembly
project(BareMetal_Research C ASM)

# Configuração do Cross-Compiler (Forçamos o uso do GCC para ARM)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

# Flags da CPU (Cortex-M0+)
set(CPU_FLAGS "-mcpu=cortex-m0plus -mthumb")

# Flags de Compilação
# -nostdlib: Não use bibliotecas padrão do C (estamos bare-metal)
# -O0 -g: Sem otimização e com símbolos de debug (bom para GDB)
set(CMAKE_C_FLAGS "${CPU_FLAGS} -std=c11 -Wall -O0 -g -nostdlib")
set(CMAKE_ASM_FLAGS "${CPU_FLAGS} -g")

# Flags do Linker
# -T: Aponta para o nosso script de memória
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/linker/memmap.ld")
set(CMAKE_EXE_LINKER_FLAGS "-T${LINKER_SCRIPT} -nostdlib -Wl,--gc-sections")

# Fontes do Projeto
set(SOURCES 
    src/main.c 
    src/start.s
)

# Criar o Executável (.elf)
add_executable(firmware.elf ${SOURCES})

# Passo Pós-Build 1: Converter ELF para BIN (Binário Cru)
add_custom_command(TARGET firmware.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary firmware.elf firmware.bin
    COMMENT "Gerando firmware.bin a partir do ELF..."
)

# Passo Pós-Build 2: Gerar UF2 usando seu script python
# (Assume que o python está no PATH)
add_custom_command(TARGET firmware.elf POST_BUILD
    COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/maker.py firmware.bin firmware.uf2
    COMMENT "Empacotando UF2 com Boot2..."
)