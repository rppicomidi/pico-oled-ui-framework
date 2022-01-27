/**
 * @file button_led.h
 * @brief This class implements drawing centered text surrounded by a box
 * @todo rename it and make it describe what it does
 * 
 * Copyright (c) 2022 rppicomid
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. * 
 */

#pragma once
#include "mono_graphics_lib.h"
#include <cstring>
namespace rppicomidi {
class Button_led {
public:
    /**
     * @brief Construct a new Button_led object
     * 
     * @param screen_ The screen object to render the button
     * @param x_ horizontal coordinate of the upper left corner
     * @param y_ vertical coordinate of the upper left corner
     * @param width_ width of the button bounding box
     * @param height_ height of the button bounding box
     * @param text_ a null-terminated C character string
     * @param font_ the font to render the button text
     * @param is_on_ the initial button state
     */
    Button_led(Mono_graphics& screen_, uint8_t x_, uint8_t y_, uint8_t width_, uint8_t height_, 
            const char* text_, const MonoMonoFont& font_, bool is_on_);

    /**
     * @brief Set the state object
     * 
     * @param is_on_ draw the button to reflect the text
     */
    void set_state(bool is_on_);
protected:
    Mono_graphics& screen;
    uint8_t x;
    uint8_t y;
    uint8_t width;
    uint8_t height;
    const char* text;
    const MonoMonoFont& font;
    bool is_on;
    uint8_t text_len;
    uint8_t x_centered_text;
};
}