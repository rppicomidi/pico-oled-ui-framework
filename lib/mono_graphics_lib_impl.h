/**
 * @file ssd1306_common.h
 * @brief This class is a C++ Raspberry Pi Pico port of the LibDriver 
 * C SSD1306 driver code found here: https://github.com/hepingood/ssd1306
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
#include <cstdlib>
#include <cstring>
template <class HwInterfaceClass, class HwInterfaceClassConstructorArgs, class HwInterfaceInstance>
rppicomidi::Mono_graphics<HwInterfaceClass, HwInterfaceClassConstructorArgs, HwInterfaceInstance>::
Mono_graphics(rppicomidi::Ssd1306_common<HwInterfaceClass, HwInterfaceClassConstructorArgs, HwInterfaceInstance>& display_, Display_rotation initial_rotation_) :
    display{display_}
{
    canvas_nbytes = display.get_minimum_canvas_size();
    canvas = reinterpret_cast<uint8_t*>(malloc(canvas_nbytes));
    memset(canvas, 0, canvas_nbytes);
    assert(canvas);
    display.init(initial_rotation_);
}

template <class HwInterfaceClass, class HwInterfaceClassConstructorArgs, class HwInterfaceInstance>
void rppicomidi::Mono_graphics<HwInterfaceClass, HwInterfaceClassConstructorArgs, HwInterfaceInstance>::
draw_dot(uint8_t x, uint8_t y, Pixel_state fg_color)
{
    display.set_pixel_on_canvas(canvas, canvas_nbytes, x, y, fg_color);
}

template <class HwInterfaceClass, class HwInterfaceClassConstructorArgs, class HwInterfaceInstance>
void rppicomidi::Mono_graphics<HwInterfaceClass, HwInterfaceClassConstructorArgs, HwInterfaceInstance>::
draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, Pixel_state fg_color)
{
	// Uses Bresenham's line algorithm as described in Wikipedia
	int dx = abs(x1-x0);
	int sx = (x0<x1) ? 1 : -1;
	int dy = -abs(y1-y0);
	int sy = (y0<y1) ? 1 : -1;
	int err = dx+dy; // error value e_xy
	while(true) {
		display.set_pixel_on_canvas(canvas, canvas_nbytes, x0, y0, fg_color);
		if (x0 == x1 && y0 == y1) {
			break; //done
		}
		int e2 = 2*err;
		if (e2 >= dy) { //e_exy+e_x > 0
			err += dy;
			x0 += sx;
		}
		if (e2 < dx) { // e_xy+e_y < 0
			err += dx;
			y0 += sy;
		}
	}
}

template <class HwInterfaceClass, class HwInterfaceClassConstructorArgs, class HwInterfaceInstance>
void rppicomidi::Mono_graphics<HwInterfaceClass, HwInterfaceClassConstructorArgs, HwInterfaceInstance>::
draw_rectangle(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height, Pixel_state fg_color, Pixel_state bg_color)
{
	uint8_t x1 = x0 + width - 1;
	uint8_t y1 = y0 + height - 1;
	draw_line(x0,y0, x1, y0, fg_color); // top of the rectangle
	draw_line(x0,y1, x1, y1, fg_color); // bottom of the rectangle
	draw_line(x0,y0, x0, y1, fg_color); // left edge
	draw_line(x1,y0, x1, y1, fg_color); // right edge
	if (bg_color != Pixel_state::PIXEL_TRANSPARENT) {
		// fill the rectangle using horizontal lines
		++x0;
		--x1;
		if (x0 <= x1) {
			++y0;
			for (uint8_t y = y0; y < y1; y++)
				draw_line(x0,y, x1, y, bg_color);
		}
	}
}

template <class HwInterfaceClass, class HwInterfaceClassConstructorArgs, class HwInterfaceInstance>
void rppicomidi::Mono_graphics<HwInterfaceClass, HwInterfaceClassConstructorArgs, HwInterfaceInstance>::
draw_character(MonoMonoFont& font, uint8_t x, uint8_t y, char chr,  Pixel_state fg_color, Pixel_state bg_color)
{
	assert(chr <= font.last_char && chr >= font.first_char);

	uint8_t nrows = font.height;
	uint8_t ncols = font.width;
	uint8_t nbytes_per_col = (nrows / 8) + (((nrows % 8)==0) ? 0:1);
	const uint8_t* pixels = font.font_bytes; // pixel array for the font starts after the nrows & ncols bytes
	// pixel array is stored columnwise in ASCII character order
	size_t idx = (size_t)(chr - font.first_char) * ncols * nbytes_per_col;	// the index of the first byte of pixel data for the character
    assert(idx < font.num_font_bytes);
	for (uint8_t col=0; col<ncols; col++) {
		uint8_t rowbits = pixels[idx++];
		uint8_t mask = font.msb_is_top ? 0x80 : 0x01;
		uint8_t xpixel = x+col;
		uint8_t ypixel = y;
		uint8_t column_byte = 0;
		for (uint8_t row = 0; row < nrows; row++) {
			display.set_pixel_on_canvas(canvas, canvas_nbytes, xpixel, ypixel, (rowbits & mask)!= 0 ? fg_color : bg_color);
            if (font.msb_is_top)
			    mask >>= 1;
            else
                mask <<= 1;
			++ypixel;
			if (mask == 0) {
				// then displayed all 8 pixels for the byte in the font; 
				// If more bytes in this column, init mask again get &
				// get the next 8 pixels to display
				++column_byte;
				if (column_byte < nbytes_per_col) {
					mask = font.msb_is_top ? 0x80 : 0x01;
					rowbits = pixels[idx++];
				}
			}
		}
    }
}