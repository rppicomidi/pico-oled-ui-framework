cmake_minimum_required(VERSION 3.13)

set(BOARD pico_sdk)
include(pico_sdk_import.cmake)

project(test_encoder_pio C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/ext_lib/encoder-pio)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib)

add_executable(test_encoder_pio
    test-encoder-pio.cpp
)

pico_enable_stdio_uart(test_encoder_pio 1)

target_include_directories(test_encoder_pio PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(test_encoder_pio encoder-pio i2c_ssd1306 ssd1306_common mono_graphics_lib pico_stdlib)

pico_add_extra_outputs(test_encoder_pio)
