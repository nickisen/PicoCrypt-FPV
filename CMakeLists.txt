# PicoCrypt FPV - CMake Build Configuration
# Raspberry Pi Pico (RP2040) FPV Video Encryption System

cmake_minimum_required(VERSION 3.13)

# Initialize Pico SDK
include(pico_sdk_import.cmake)

project(picocrypt-fpv C CXX ASM)

# Set Pico SDK path (can be overridden with -DPICO_SDK_PATH=)
if(NOT DEFINED PICO_SDK_PATH)
    set(PICO_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../pico-sdk")
endif()

# Initialize the SDK
pico_sdk_init()

# Project version
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)

# Compiler flags
add_compile_options(-Wall -Wextra -Wno-unused-parameter)
add_compile_options(-O3 -DNDEBUG)  # Optimized release build

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ===== PIO PROGRAMS =====
# Video sync detection PIO program
pico_generate_pio_header(picocrypt-fpv 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/video_sync_detect.pio
)

# Video output PIO program  
pico_generate_pio_header(picocrypt-fpv
    ${CMAKE_CURRENT_SOURCE_DIR}/src/video_output.pio
)

# ===== SENDER FIRMWARE =====
add_executable(picocrypt_sender
    src/picocrypt_sender.c
   
 src/crypto.c
    src/video_input.c
    src/video_output.c
    src/pio_programs.c
)

# Target properties
set_target_properties(picocrypt_sender PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
)

# Include directories for sender
target_include_directories(picocrypt_sender PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Link libraries for sender
target_link_libraries(picocrypt_sender 
    pico_stdlib
    pico_multicore
    hardware_pio
    hardware_dma
    hardware_adc
    hardware_clocks
    hardware_irq
    hardware_sync
)

# Enable USB output (for debugging)
pico_enable_stdio_usb(picocrypt_sender 1)
pico_enable_stdio_uart(picocrypt_sender 0)

# ===== RECEIVER FIRMWARE =====
add_executable(picocrypt_receiver
    src/picocrypt_receiver.c
    src/crypto.c
    src/video_input.c
 
   src/video_output.c
    src/pio_programs.c
)

# Target properties
set_target_properties(picocrypt_receiver PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
)

# Include directories for receiver
target_include_directories(picocrypt_receiver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Link libraries for receiver
target_link_libraries(picocrypt_receiver
    pico_stdlib
    pico_multicore
    hardware_pio
    hardware_dma
    hardware_adc
    hardware_clocks
    hardware_irq
    hardware_sync
)

# Enable USB output (for debugging)
pico_enable_stdio_usb(picocrypt_receiver 1)
pico_enable_stdio_uart(picocrypt_receiver 0)

# ===== TEST PROGRAMS =====
# Test pattern generator
add_executable(test_pattern_gen
    src/test_pattern_gen.c
    src/video_output.c
    src/pio_programs.c
)

target_link_libraries(test_pattern_gen
    pico_stdlib
 
   hardware_pio
    hardware_dma
    hardware_clocks
)

pico_enable_stdio_usb(test_pattern_gen 1)
pico_enable_stdio_uart(test_pattern_gen 0)

# Encryption test
add_executable(crypto_test
    src/crypto_test.c
    src/crypto.c
)

target_link_libraries(crypto_test
    pico_stdlib
)

pico_enable_stdio_usb(crypto_test 1)
pico_enable_stdio_uart(crypto_test 0)

# ===== UTILITY PROGRAMS =====
# Performance monitor
add_executable(perf_monitor
    src/perf_monitor.c
)

target_link_libraries(perf_monitor
    pico_stdlib
)

pico_enable_stdio_usb(perf_monitor 1)
pico_enable_stdio_uart(perf_monitor 0)

# ===== INSTALL TARGETS =====
# Create UF2 files for easy flashing
pico_add_extra_outputs(picocrypt_sender)
pico_add_extra_outputs(picocrypt_receiver)
pico_add_extra_outputs(test_pattern_gen)
pico_add_extra_outputs(crypto_test)
pico_add_extra_outputs(perf_monitor)

# ===== INSTALL RULES =====
install(TARGETS 
    picocrypt_sender 
    picocrypt_receiver
    test_pattern_gen
    crypto_test
    perf_monitor
    
    DESTINATION bin
)

# ===== CUSTOM TARGETS =====
# Flash sender firmware
add_custom_target(flash_sender
   
 COMMAND ${CMAKE_COMMAND} -E copy 
        ${CMAKE_CURRENT_BINARY_DIR}/picocrypt_sender.uf2
        /media/$ENV{USER}/RP2040/
    DEPENDS picocrypt_sender
    COMMENT "Flashing sender firmware..."
)

# Flash receiver firmware
add_custom_target(flash_receiver
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/picocrypt_receiver.uf2
        /media/$ENV{USER}/RP2040/
    DEPENDS picocrypt_receiver
    COMMENT "Flashing receiver firmware..."
)

# Clean all generated files
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Cleaning all build artifacts..."
)

# 
===== PRINT BUILD INFO =====
message(STATUS "========================================")
message(STATUS "PicoCrypt FPV Build Configuration")
message(STATUS "========================================")
message(STATUS "PICO_SDK_PATH: ${PICO_SDK_PATH}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "PIO programs:")
message(STATUS "  - video_sync_detect.pio")
message(STATUS "  - video_output.pio")
message(STATUS "Targets:")
message(STATUS "  - picocrypt_sender (main sender firmware)")
message(STATUS "  - picocrypt_receiver (main receiver firmware)")
message(STATUS "  - test_pattern_gen (test pattern generator)")
message(STATUS "  - crypto_test (encryption test)")
message(STATUS "  - perf_monitor (performance monitor)")
message(STATUS "========================================")

# ===== DEVELOPMENT OPTIONS =====
# Option to enable debug mode
option(ENABLE_DEBUG "Enable debug mode with extra logging" OFF)
if(ENABLE_DEBUG)
    add_compile_definitions(DEBUG_MODE=1)
    message(STATUS "Debug mode enabled")
else()
    add_compile_definitions(DEBUG_MODE=0)
endif()

# Option to enable test mode
option(ENABLE_TEST_MODE "Enable 
test mode with known patterns" OFF)
if(ENABLE_TEST_MODE)
    add_compile_definitions(TEST_MODE=1)
    message(STATUS "Test mode enabled")
else()
    add_compile_definitions(TEST_MODE=0)
endif()

# Option to change pre-shared key
set(PRESHARED_KEY "0x123456789ABCDEF0ULL" CACHE STRING "64-bit pre-shared key")
add_compile_definitions(PRESHARED_KEY=${PRESHARED_KEY})
message(STATUS "Pre-shared key: ${PRESHARED_KEY}")

# ===== DOCUMENTATION GENERATION =====
find_program(DOXYGEN doxygen)
if(DOXYGEN)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/docs/Doxyfile)
    
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    
    add_custom_target(doc
        COMMAND ${DOXYGEN} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs
        COMMENT "Generating API documentation with Doxygen"
 
       VERBATIM
    )
endif()