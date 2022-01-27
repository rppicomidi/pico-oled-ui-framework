/**
 * @file mono_graphics_lib.cpp
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

#include <cstdlib>
#include <cstring>
#include "mono_graphics_lib.h"

rppicomidi::Mono_graphics::Mono_graphics(rppicomidi::Ssd1306* display_, Display_rotation initial_rotation_) :
    display{display_}
{
    canvas_nbytes = display->get_minimum_canvas_size();
    canvas = reinterpret_cast<uint8_t*>(malloc(canvas_nbytes));
    assert(canvas);
    clear_canvas();
    display->init(initial_rotation_);
	set_clip_rect(0, 0, display->get_screen_width()-1, display->get_screen_height()-1);
}

void rppicomidi::Mono_graphics::draw_dot(uint8_t x, uint8_t y, Pixel_state fg_color)
{
	// only draw the dot if x and y are within the clipping rectangle
	if (x >= clip_rect.x_upper_left && x <= clip_rect.x_lower_right &&
		y >= clip_rect.y_upper_left && y <= clip_rect.y_lower_right) {
    	display->set_pixel_on_canvas(canvas, canvas_nbytes, x, y, fg_color);
	}
}

void rppicomidi::Mono_graphics::draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, Pixel_state fg_color)
{
	// Uses Bresenham's line algorithm as described in Wikipedia
	int dx = abs(x1-x0);
	int sx = (x0<x1) ? 1 : -1;
	int dy = -abs(y1-y0);
	int sy = (y0<y1) ? 1 : -1;
	int err = dx+dy; // error value e_xy
	while(true) {
		draw_dot(x0, y0, fg_color);
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

void rppicomidi::Mono_graphics::draw_rectangle(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height, Pixel_state fg_color, Pixel_state bg_color)
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

void rppicomidi::Mono_graphics::draw_character(const MonoMonoFont& font, uint8_t x, uint8_t y, char chr,  Pixel_state fg_color, Pixel_state bg_color)
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
			draw_dot(xpixel, ypixel, (rowbits & mask)!= 0 ? fg_color : bg_color);
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

void rppicomidi::Mono_graphics::circle_points(int cx, int cy, int x, int y, Pixel_state fg_color, Pixel_state fill_color)
{
	if (x == 0) {
		draw_dot(cx, cy + y, fg_color);
		draw_dot(cx, cy - y, fg_color);
		draw_dot(cx + y, cy, fg_color);
		draw_dot(cx - y, cy, fg_color);
		draw_line(cx-y+1,cy, cx+y-1, cy, fill_color);
	}
	else if (x == y) {
		draw_dot(cx + x, cy + y, fg_color);
		draw_dot(cx - x, cy + y, fg_color);
		draw_line(cx-x+1,cy+y, cx+x-1, cy+y, fill_color);
		draw_dot(cx + x, cy - y, fg_color);
		draw_dot(cx - x, cy - y, fg_color);
		draw_line(cx-x+1,cy-y, cx+x-1, cy-y, fill_color);
	}
	else if (x < y) {
		draw_dot(cx + x, cy + y, fg_color);
		draw_dot(cx - x, cy + y, fg_color);
		draw_line(cx-x+1,cy+y, cx+x-1, cy+y, fill_color);
		draw_dot(cx + x, cy - y, fg_color);
		draw_dot(cx - x, cy - y, fg_color);
		draw_line(cx-x+1,cy-y, cx+x-1, cy-y, fill_color);
		draw_dot(cx + y, cy + x, fg_color);
		draw_dot(cx - y, cy + x, fg_color);
		draw_line(cx-y+1,cy+x, cx+y-1, cy+x, fill_color);
		draw_dot(cx + y, cy - x, fg_color);
		draw_dot(cx - y, cy - x, fg_color);
		draw_line(cx-y+1,cy-x, cx+y-1, cy-x, fill_color);
	}
}

void rppicomidi::Mono_graphics::draw_circle(uint8_t x0, uint8_t y0, uint8_t diameter, Pixel_state fg_color, Pixel_state fill_color)
{
	int radius = diameter / 2;
	int x_center = x0 + radius;
	int y_center = y0 + radius;
	draw_centered_circle(x_center, y_center, radius, fg_color, fill_color);
}

void rppicomidi::Mono_graphics::draw_centered_circle(uint8_t x_center, uint8_t y_center, uint8_t radius, Pixel_state fg_color, Pixel_state fill_color)
{
	int x = 0;
	int y = radius;
	int p = (5 - radius*4)/4;

	circle_points(x_center, y_center, x, y, fg_color, fill_color);
	while (x < y) {
		x++;
		if (p < 0) {
			p += 2*x+1;
		} else {
			y--;
			p += 2*(x-y)+1;
		}
		circle_points(x_center, y_center, x, y, fg_color, fill_color);
	}
	if (fill_color == Pixel_state::PIXEL_TRANSPARENT) {
		radius = 0;
	}
	else {
		--radius;
	}
}
