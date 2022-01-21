cmake_minimum_required(VERSION 3.13)

add_library(ssd1306i2c INTERFACE)
target_sources(ssd1306i2c INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/ssd1306i2c.cpp
)
target_include_directories(ssd1306i2c INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(ssd1306i2c INTERFACE pico_stdlib hardware_i2c hardware_gpio)

add_library(ssd1306 INTERFACE)
target_sources(ssd1306 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/ssd1306.cpp
)
target_include_directories(ssd1306 INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(ssd1306 INTERFACE pico_stdlib)

add_library(mono_graphics_lib INTERFACE)
target_sources(mono_graphics_lib INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/mono_graphics_lib.cpp
)
target_include_directories(mono_graphics_lib INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(mono_graphics_lib INTERFACE pico_stdlib)
