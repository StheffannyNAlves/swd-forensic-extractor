cmake_minimum_required(VERSION 3.12)
project(BareMetal_Research C ASM)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

set(CPU_FLAGS "-mcpu=cortex-m0plus -mthumb")

set(CMAKE_C_FLAGS
    "${CPU_FLAGS} -std=c11 -Wall -O0 -g -ffreestanding -nostdlib -ffunction-sections -fdata-sections"
)

# ASM flags
set(CMAKE_ASM_FLAGS "${CPU_FLAGS} -g")

set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/linker/memmap.ld")
set(CMAKE_EXE_LINKER_FLAGS "-T${LINKER_SCRIPT} -nostdlib -Wl,--gc-sections")

# ---- firmware ----
add_executable(firmware.elf
    ${CMAKE_SOURCE_DIR}/src/boot2_final.S
    ${CMAKE_SOURCE_DIR}/src/start.s
    ${CMAKE_SOURCE_DIR}/src/main.c
)

# libgcc (div/mod helpers etc.)
target_link_libraries(firmware.elf -lgcc)

# Generate flat binary
add_custom_command(TARGET firmware.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary
            $<TARGET_FILE:firmware.elf>
            ${CMAKE_CURRENT_BINARY_DIR}/firmware.bin
    COMMENT ">>> Gerando firmware.bin <<<"
)