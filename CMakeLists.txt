cmake_minimum_required(VERSION 3.13)

# ─── Pico SDK ────────────────────────────────────────────────────────────────
# Set PICO_SDK_PATH in your environment or pass via -DPICO_SDK_PATH=...
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(p1_forensic_probe C CXX ASM)
set(CMAKE_C_STANDARD   11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# ─── Compiler flags ──────────────────────────────────────────────────────────
add_compile_options(
    -Wall
    -Wextra
    -Werror
    -O2
    -ffunction-sections
    -fdata-sections
)

# ─── Source files ────────────────────────────────────────────────────────────
set(FIRMWARE_SOURCES
    src/main.c
    # src/phy/swd_phy.c
    #src/protocol/swd_protocol.c
    #src/protocol/dap.c
    #src/target/target_ctrl.c
    #src/target/qspi_init.c
    #src/acquisition/acquisition.c 
    src/transport/usb_transport.c
    #src/transport/session_log.c
)

# ─── Executable ──────────────────────────────────────────────────────────────
add_executable(p1_forensic_probe ${FIRMWARE_SOURCES})

target_include_directories(p1_forensic_probe PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ─── Pico SDK libraries ──────────────────────────────────────────────────────
target_link_libraries(p1_forensic_probe
    pico_stdlib
    pico_unique_id          # for probe serial number in session log
    hardware_gpio
    hardware_timer
    tinyusb_device          # USB CDC transport
    tinyusb_board
)

# ─── USB CDC (disable UART stdio, use USB) ───────────────────────────────────
pico_enable_stdio_uart(p1_forensic_probe 0)
# pico_enable_stdio_usb(p1_forensic_probe 0)   # we manage TinyUSB ourselves

# ─── Output: UF2 + BIN + HEX ─────────────────────────────────────────────────
pico_add_extra_outputs(p1_forensic_probe)

# ─── Optional: unit tests ────────────────────────────────────────────────────
# Uncomment when tests/CMakeLists.txt is ready
# add_subdirectory(tests)