cmake_minimum_required(VERSION 3.13)

add_library(ssd1306i2c INTERFACE)
target_sources(ssd1306i2c INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/ssd1306i2c.cpp
)
target_include_directories(ssd1306i2c INTERFACE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(ssd1306i2c INTERFACE pico_stdlib hardware_i2c hardware_gpio)

add_library(ssd1306pioi2c INTERFACE)
target_sources(ssd1306pioi2c INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/ssd1306pioi2c.cpp
)
pico_generate_pio_header(ssd1306pioi2c ${CMAKE_CURRENT_LIST_DIR}/i2c.pio)
target_include_directories(ssd1306pioi2c INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)
target_link_libraries(ssd1306pioi2c INTERFACE pico_stdlib hardware_pio hardware_i2c hardware_gpio)

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

add_library(button_led INTERFACE)
target_sources(button_led INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/button_led.cpp
)
target_include_directories(button_led INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(button_led INTERFACE mono_graphics_lib pico_stdlib)

add_library(vpot_display INTERFACE)
target_sources(vpot_display INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/vpot_display.cpp
)
target_include_directories(vpot_display INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(vpot_display INTERFACE mono_graphics_lib pico_stdlib)

add_library(mc_meter INTERFACE)
target_sources(mc_meter INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/mc_meter.cpp
)
target_include_directories(mc_meter INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(mc_meter INTERFACE mono_graphics_lib pico_stdlib)

add_library(mc_channel_text INTERFACE)
target_sources(mc_channel_text INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/mc_channel_text.cpp
)
target_include_directories(mc_channel_text INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(mc_channel_text INTERFACE mono_graphics_lib pico_stdlib)