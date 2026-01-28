cmake_minimum_required(VERSION 3.12)

project(BareMetal_Research C ASM)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

set(CPU_FLAGS "-mcpu=cortex-m0plus -mthumb")


set(CMAKE_C_FLAGS "${CPU_FLAGS} -std=c11 -Wall -O0 -g -nostdlib")
set(CMAKE_ASM_FLAGS "${CPU_FLAGS} -g")


set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/linker/memmap.ld")
set(CMAKE_EXE_LINKER_FLAGS "-T${LINKER_SCRIPT} -nostdlib -Wl,--gc-sections")

set(SOURCES 
    src/main.c 
    src/start.s
)

add_executable(firmware.elf ${SOURCES})

target_link_libraries(firmware.elf -lgcc)

add_custom_command(TARGET firmware.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O binary firmware.elf firmware.bin
    COMMENT "Gerando firmware.bin a partir do ELF..."
)


add_custom_command(TARGET firmware.elf POST_BUILD
    COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/maker.py firmware.bin firmware.uf2
    COMMENT "Empacotando UF2 com Boot2"
)