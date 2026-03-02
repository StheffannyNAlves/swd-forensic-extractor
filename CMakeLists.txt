cmake_minimum_required(VERSION 3.12)
project(BareMetal_Research C ASM)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CPU_FLAGS -mcpu=cortex-m0plus -mthumb)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/linker/memmap.ld)

# Boot2 generation
set(BOOT2_SRC ${CMAKE_SOURCE_DIR}/src/boot2_w25q080.S)
set(BOOT2_GER ${CMAKE_CURRENT_BINARY_DIR}/boot2_gerado.S)

add_custom_command(
    OUTPUT ${BOOT2_GER}
    COMMAND arm-none-eabi-gcc ${CPU_FLAGS} -c ${BOOT2_SRC} -o boot2.o
    COMMAND arm-none-eabi-objcopy -O binary boot2.o boot2.bin
    COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/maker.py boot2.bin ${BOOT2_GER}
    DEPENDS ${BOOT2_SRC} ${CMAKE_SOURCE_DIR}/tools/maker.py
)

add_custom_target(generate_boot2 DEPENDS ${BOOT2_GER})

add_executable(firmware.elf
    ${BOOT2_GER}
    src/start.s
    src/main.c
)

add_dependencies(firmware.elf generate_boot2)

target_compile_options(firmware.elf PRIVATE
    ${CPU_FLAGS}
    -std=c11
    -Wall
    -O0
    -g
    -ffreestanding
)

target_link_options(firmware.elf PRIVATE
    -T${LINKER_SCRIPT}
    -nostdlib
    -Wl,--gc-sections
)

target_link_libraries(firmware.elf gcc)

add_custom_command(TARGET firmware.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary firmware.elf firmware.bin
    COMMAND picotool uf2 convert firmware.bin firmware.uf2
)