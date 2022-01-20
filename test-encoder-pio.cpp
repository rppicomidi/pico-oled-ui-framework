/**
 * @file test-encoder-pio.cpp
 * @brief This program tests the encoder-pio driver
 * 
 */
#include <cstdio>
#include <cstring>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "encoder.hpp"

// The following are the GPIO numbers for the encoder pins. They can be any
// GPIO Pin
#define PINA 2
#define PINB 3
#define PIN_PRESS 4

#include "i2c_ssd1306.h"
#include "ssd1306_common.h"
#include "mono_graphics_lib.h"
#include "ext_lib/ssd1306/src/driver_ssd1306_font.h"
int main()
{
    using namespace rppicomidi;
    stdio_init_all();

    I2c_ssd1306::InterfaceInstance i2c_port = {i2c1};
    I2c_ssd1306::ConstructorArgs i2c_args = {
        i2c_port, 0x3c, 2, 3};
    Ssd1306_common<I2c_ssd1306, I2c_ssd1306::ConstructorArgs, I2c_ssd1306::InterfaceInstance> ssd1306(i2c_args);

    Mono_graphics<I2c_ssd1306, I2c_ssd1306::ConstructorArgs, I2c_ssd1306::InterfaceInstance> screen(ssd1306, Display_rotation::Portrait270);

    MonoMonoFont font(12, 6, gsc_ssd1306_ascii_1206, sizeof(gsc_ssd1306_ascii_1206));

    #if 1
    //screen.draw_string(font, 0, 0, "Hello!", 6, Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
    //screen.draw_line(32,32, 32, 127,Pixel_state::PIXEL_ONE);
    //screen.draw_rectangle(10,30, 20, 50, Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ONE);
    screen.draw_circle(16,16,32, Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_TRANSPARENT);
    #else
    for (uint8_t xy=0; xy<64; xy++)
        screen.draw_dot(xy,xy,Pixel_state::PIXEL_ONE);
    #endif
    screen.render();
    printf("screen should say hello!\r\n");
    for (;;) ;
#if 0
    using namespace pimoroni;
    Encoder encoder(pio0, PINA, PINB);
    encoder.init();
    stdio_init_all();
    encoder.zero_count();
    gpio_init(PIN_PRESS);
    gpio_set_dir(PIN_PRESS, false);
    bool not_pressed = gpio_get(PIN_PRESS);
    bool prev_not_pressed = not_pressed;
    printf("testing rotary encoder-pio\n");
    int32_t count = 0;
    while (1) {
        // 
        Capture capture{encoder.perform_capture()};
        count = capture.get_count_change();
        if (count != 0) {
            printf("%d %f\n\r", count, capture.get_revolutions_per_minute());
        }
        not_pressed = gpio_get(PIN_PRESS);
        if (not_pressed != prev_not_pressed) {
            printf("encoder is %s\r\n", not_pressed ? "not pressed": "pressed");
            prev_not_pressed = not_pressed;
        }
    }
#endif
    return 0;
}
