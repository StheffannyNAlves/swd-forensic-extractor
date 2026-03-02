cmake_minimum_required(VERSION 3.12)

project(BareMetal_Research C ASM)



set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

set(CPU_FLAGS "-mcpu=cortex-m0plus -mthumb")

set(CMAKE_C_FLAGS "${CPU_FLAGS} -std=c11 -Wall -O0 -g -ffreestanding -nostdlib")
set(CMAKE_ASM_FLAGS "${CPU_FLAGS} -g")

# Linker

set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/linker/memmap.ld")
set(CMAKE_EXE_LINKER_FLAGS
    "-T${LINKER_SCRIPT} -nostdlib -Wl,--gc-sections"
)


# Geração do boot2


set(BOOT2_SRC ${CMAKE_SOURCE_DIR}/src/boot2_w25q080.S)
set(BOOT2_OBJ ${CMAKE_CURRENT_BINARY_DIR}/boot2.o)
set(BOOT2_BIN ${CMAKE_CURRENT_BINARY_DIR}/boot2.bin)
set(BOOT2_GER ${CMAKE_CURRENT_BINARY_DIR}/boot2_gerado.S)

add_custom_command(
    OUTPUT ${BOOT2_GER}

    # Compila o assembly original
    COMMAND ${CMAKE_ASM_COMPILER}
        -mcpu=cortex-m0plus
        -mthumb
        -g
        -c ${BOOT2_SRC}
        -o ${BOOT2_OBJ}
        
    # Extrai binário cru
    COMMAND arm-none-eabi-objcopy -O binary
            ${BOOT2_OBJ}
            ${BOOT2_BIN}

    # Roda script Python (padding + CRC)
    COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/maker.py
            ${BOOT2_BIN}
            ${BOOT2_GER}

    DEPENDS ${BOOT2_SRC} ${CMAKE_SOURCE_DIR}/tools/maker.py

    COMMENT ">>> Gerando boot2 com CRC32 válido <<<"
)

# Cria target intermediário
add_custom_target(generate_boot2
    DEPENDS ${BOOT2_GER}
)

# Firmware principal

set(SOURCES
    ${BOOT2_GEN}
    ${CMAKE_SOURCE_DIR}/src/start.s
    ${CMAKE_SOURCE_DIR}/src/main.c
)

add_executable(firmware.elf ${SOURCES})

# Garante ordem correta
add_dependencies(firmware.elf generate_boot2)

target_link_libraries(firmware.elf -lgcc)

# Geração do BIN e UF2

add_custom_command(TARGET firmware.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary
            firmware.elf
            firmware.bin

    COMMAND picotool uf2 convert
            firmware.bin
            firmware.uf2

    COMMENT ">>> Gerando firmware.bin e firmware.uf2 <<<"
)