/**
 * @file mono_graphics_lib.h
 * @brief This class implements drawing monochrome graphics.
 * 
 * This class is a C++ Raspberry Pi Pico port of the LibDriver 
 * C SSD1306 driver code found here: https://github.com/hepingood/ssd1306
 * MIT License for those files is in the project directories
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
 * SOFTWARE.
 */
#pragma once
#include <cstdint>
#include <cstring>
#include "ssd1306_common.h"
namespace rppicomidi {

/**
 * @brief Monospace Monochrome font
 *
 * MonoMonoFont is designed to display fixed width printable ASCII
 * characters. Each letter is represented by a bitmap image stored
 * in several consecutive bytes. The array font_bytes contains
 * the bitmaps required to render all ASCII characters from first_char
 * to last_char.
 *
 * Character bitmaps are stored one bit per pixel packed in bytes. A
 * byte can represent up to 8 pixels. The first pixel in a byte represents
 * the lowest vertical raster coordinate. After enough pixels for rendering
 * one vertical column of pixels are packed into successive bytes,
 * 
 */
class MonoMonoFont
{
public:
    MonoMonoFont(uint8_t height_, uint8_t width_, const uint8_t* font_bytes_, size_t num_font_bytes_, 
        char first_char_=' ', char last_char_='~', bool msb_is_top_=true) : height{height_}, 
        width{width_}, font_bytes{font_bytes_}, num_font_bytes{num_font_bytes_},
        first_char{first_char_}, last_char{last_char_}, msb_is_top{msb_is_top_}
    {}

    uint8_t height;         //!< the font height in pixels
    uint8_t width;          //!< the font width in pixels
    const uint8_t* font_bytes;    //!< font byte array formatted as described above
    const size_t num_font_bytes;  //!< number of bytes in the font_bytes array
    const char first_char;        //!< the first character in the font (often ' ')
    const char last_char;         //!< the last character in the font (often '~')
    bool msb_is_top;        //!< see description above
};

template <class HwInterfaceClass, class HwInterfaceClassConstructorArgs, class HwInterfaceInstance>
class Mono_graphics 
{
public:
    /**
     * @brief Construct a new Mono_graphics object
     * 
     * @param display the interface to the display
     */
    Mono_graphics(Ssd1306_common<HwInterfaceClass, HwInterfaceClassConstructorArgs, HwInterfaceInstance>& display_, Display_rotation initial_rotation_);

    /**
     * @brief draw a single dot on the screen at raster coordinates (x,y)
     * using the writing mode described by fg_color.
     * 
     */
    void draw_dot(uint8_t x, uint8_t y, Pixel_state fg_color);

    /**
     * @brief draw a line from (x0,y0) to (x1,y1)
     * using the writing mode described by fg_color.
     * 
     */
    void draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, Pixel_state fg_color);

    /**
     * @brief draw an empty rectangle width x height with upper left corner at (x0,y0)
     * using the writing mode described by fg_color.
     * 
     * To draw a hollow rectangle around existing drawing use bg_color=Pixel_state::PIXEL_TRANSPARENT.
     * To draw a filled rectangle over existing drawing use bg_color=Pixel_state::PIXEL_ONE or
     * Pixel_state::PIXEL_ZERO
     */
    void draw_rectangle(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height, Pixel_state fg_color, Pixel_state bg_color);

    /**
     * @brief draw a circle with upper left corner at (x0,y0) and the given diameter
     * using the writing mode described by fg_color and fill_color.
     * 
     * This function translates coordinates and calls draw_centered_circle(). See that
     * function for license comments.
     * 
     * @param x0 the horizontal coordiate of the upper left corner of the circle's square bounding box
     * @param y0 the vertical coordiate of the upper left corner of the circle's square bounding box
     * @param diameter is the circle diameter. The circle radius has to be an integer
     * number of pixels, so if diameter is odd, the radius will be smaller than expected.
     * @param fg_color is how to draw the pixel outside the circle
     * @param fill_color is how to draw the inside of the circle
     */
    void draw_circle(uint8_t x0, uint8_t y0, uint8_t diameter, Pixel_state fg_color, Pixel_state fill_color=Pixel_state::PIXEL_TRANSPARENT);

    /**
     * @brief draw a circle with center at (x_center, y_center) and the given radius
     * using the writing mode described by fg_color and fill_color.
     * 
     * This algorithm taken from https://groups.csail.mit.edu/graphics/classes/6.837/F98/Lecture6/circle.html
     * License for this function is not known. I modified the it to support filled circles too.
     * 
     * @param x_center the horizontal coordiate of the circle's center
     * @param y_center the vertical coordiate of the circle's center
     * @param radius is the circle's radius.
     * @param fg_color is how to draw the pixel outside the circle
     * @param fill_color is how to draw the inside of the circle
     */
    void draw_centered_circle(uint8_t x_center, uint8_t y_center, uint8_t radius, Pixel_state fg_color, Pixel_state fill_color=Pixel_state::PIXEL_TRANSPARENT);

    /**
     * @brief draw a single character to the screen based on the pixel_state.
     * 
     * @param font the font object to use when drawing the character
     * @param x horizontal position for current screen rotation of upper left pixel of the character
     * @param y vertical position for current screen rotation of upper left pixel of the character
     * @param chr the character to draw
     * @param fg_color how to render the "1" pixels of the font bitmap
     * @param bg_color how to render the "0" pixels of the font bitmap
     * @return true if successful, false otherwise
     */
    void draw_character(MonoMonoFont& font, uint8_t x, uint8_t y, char chr, Pixel_state fg_color, Pixel_state bg_color);

    /**
     * @brief draw a single character to the screen based on the pixel_state.
     * 
     * @param font the font object to use when drawing the character
     * @param x horizontal position for current screen rotation of upper left pixel of the character
     * @param y vertical position for current screen rotation of upper left pixel of the character
     * @param str the NULL terminated C string to draw
     * @param len the number of characters to draw; must be <= strlen(str)
     * @param fg_color how to render the "1" pixels of the font bitmap
     * @param bg_color how to render the "0" pixels of the font bitmap
     * @return true if successful, false otherwise
     */
    void draw_string(MonoMonoFont& font, uint8_t x, uint8_t y, const char* str, size_t len, Pixel_state fg_color, Pixel_state bg_color) {
        assert(strlen(str) <= len);
        while (len--) {
            draw_character(font, x, y, *str++, fg_color, bg_color);
            x+=font.width;
        }
    }

    inline bool render() {
        return display.write_display_mem(canvas, canvas_nbytes);
    }
    Display_rotation get_display_rotation() {return display.get_display_rotation(); }
private:
    Ssd1306_common<HwInterfaceClass, HwInterfaceClassConstructorArgs, HwInterfaceInstance> display;
    uint8_t* canvas;
    size_t canvas_nbytes;
    void circle_points(int cx, int cy, int x, int y, Pixel_state bg_color, Pixel_state fill_color);
};

}

#include "mono_graphics_lib_impl.h"
