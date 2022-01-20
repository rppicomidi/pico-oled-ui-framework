cmake_minimum_required(VERSION 3.13)

add_library(i2c_ssd1306 INTERFACE)
target_sources(i2c_ssd1306 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/i2c_ssd1306.cpp
)
target_include_directories(i2c_ssd1306 INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(i2c_ssd1306 INTERFACE pico_stdlib hardware_i2c hardware_gpio)

add_library(ssd1306_common INTERFACE)

target_include_directories(ssd1306_common INTERFACE ${CMAKE_CURRENT_LIST_DIR})

add_library(mono_graphics_lib INTERFACE)

target_include_directories(mono_graphics_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})