cmake_minimum_required(VERSION 3.12)
project(BareMetal_Research C ASM)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

set(CPU_FLAGS "-mcpu=cortex-m0plus -mthumb")
set(CMAKE_C_FLAGS "${CPU_FLAGS} -std=c11 -Wall -O0 -g -ffreestanding -nostdlib -ffunction-sections -fdata-sections")
set(CMAKE_ASM_FLAGS "${CPU_FLAGS} -g")

set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/linker/memmap.ld")
set(CMAKE_EXE_LINKER_FLAGS "-T${LINKER_SCRIPT} -nostdlib -Wl,--gc-sections")

# ---- boot2 pipeline ----
set(BOOT2_SRC ${CMAKE_SOURCE_DIR}/src/boot2_w25q080.S)
set(BOOT2_OBJ ${CMAKE_CURRENT_BINARY_DIR}/boot2.o)
set(BOOT2_BIN ${CMAKE_CURRENT_BINARY_DIR}/boot2.bin)
set(BOOT2_GEN ${CMAKE_CURRENT_BINARY_DIR}/boot2_gerado.S)

add_custom_command(
    OUTPUT ${BOOT2_GEN}
    BYPRODUCTS ${BOOT2_OBJ} ${BOOT2_BIN}
    COMMAND ${CMAKE_ASM_COMPILER} -mcpu=cortex-m0plus -mthumb -g -c ${BOOT2_SRC} -o ${BOOT2_OBJ}
    COMMAND arm-none-eabi-objcopy -O binary ${BOOT2_OBJ} ${BOOT2_BIN}
    COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/maker.py ${BOOT2_BIN} ${BOOT2_GEN}
    DEPENDS ${BOOT2_SRC} ${CMAKE_SOURCE_DIR}/tools/maker.py
    COMMENT ">>> Gerando boot2 (padding + CRC32) <<<"
)

set_source_files_properties(${BOOT2_GEN} PROPERTIES GENERATED TRUE)

add_custom_target(generate_boot2 DEPENDS ${BOOT2_GEN})

# ---- firmware ----
add_executable(firmware.elf
    ${BOOT2_GEN}
    ${CMAKE_SOURCE_DIR}/src/start.s
    ${CMAKE_SOURCE_DIR}/src/main.c
)

add_dependencies(firmware.elf generate_boot2)
target_link_libraries(firmware.elf -lgcc)

add_custom_command(TARGET firmware.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary
    $<TARGET_FILE:firmware.elf>
    ${CMAKE_CURRENT_BINARY_DIR}/firmware.bin
    COMMENT ">>> Gerando firmware.bin <<<"
)