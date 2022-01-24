/**
 * @file ssd1306.h
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
 *
 * This template class provides the functionality required to set up the display,
 * adjust the contrast, and write to display memory. This class does not perform
 * graphics operations such drawing lines or text. A higher level graphics
 * driver class should inherit from this class to implement that. This template
 * also does not implement the physical interface to the SSD1306. That is the
 * domain of the display I2C class or SPI class that serves as an argument
 * to this template.
 * 
 * The SSD1306 chip internal display memory is set up as 8 pages of 128 columns
 * of bytes. Pages are numbered 0-7 and columns are numbered 0-127. Each byte at
 * address (column, page) maps to 8 pixels on the screen. How those bytes are
 * displayed depends on whether the screen is in landscape mode (128 x 64) or
 * portrait mode, (64 x 128).
 * 
 * In landscape mode, define the raster coordinates such that the screen upper
 * left (x,y)==(0,0) and the screen lower right (x,y)==(127,63). The byte at
 * (column, page) represents 8 veritical pixels from (x,y)=(column, page*8 bit 0)
 * to (column, page*8 bit 7). The least significant bit of the page byte is lowest y.
 * For example, the pixel at (x,y)==(9,21) corresponds to the byte at column 9,
 * page 2, bit 5.
 * 
 * In portrait mode, define the raster coordinates such that the screen upper
 * left (x,y)==(0,0) and the screen lower right (x,y)==(63,127). The byte at
 * (column, page) represents 8 horizontal pixels from (x,y)=(page*8 bit 0, column)
 * to (page*8 bit 7, column). The least significant bit corresponds to the
 * lowest x. For example, the pixel at (x,y)==(9,21) corresponds to the byte at
 * column 21, page 1, bit 1.
 * 
 * As long as the higher level driver writes to display memory using these
 * rules, the SSD1306 hardware will support clockwise display rotations 0 degrees,
 * 90 degrees, 180 degrees and 270 degrees. The higher level driver will always
 * have to rewrite memory if the screen rotation changes.
 */
#pragma once
#include <cstdint>
#include <cstring>
#include "ssd1306hw.h"
namespace rppicomidi {
/**
 * @brief Describes the orientation of the display
 * 
 */
enum class Display_rotation {
    Landscape0,        //!< Default orientation, display is 128x64
    Portrait90=90,     //!< Rotated 90 degrees clockwise from default, display is 64,128
    Landscape180=180,  //!< Rotated 180 degrees clockwise from default, display is 128x64
    Portrait270=270,   //!< Rotated 270 degrees clockwise from default, display is 64x128
};

/**
 * @brief this enum is more descriptive than color
 * 
 * When the memory bit is 0, then the pixel
 * will be off in normal display mode and on
 * in inverted display mode.
 * 
 * When the memory bit is 1, then the pixel
 * will be on in normal display mode and off
 * in inverted display mode.
 */
enum class Pixel_state {
    PIXEL_ZERO, //!< set the pixel memory bit to 0
    PIXEL_ONE,  //!< set the pixel memory bit to 1
    PIXEL_XOR,  //!< flip the pixel memory bit
    PIXEL_TRANSPARENT, //!< don't change the memory bit
};


class Ssd1306 {
public:
    /**
     * @enum class Com_pin_cfg defines the value for the COM PIN CFG
     * command 0xDA. See the SSD1309 datasheet and comments below
     */
    enum class Com_pin_cfg {
        SEQ_DIS=0x02, /* one 128x32 display uses this. sequential COM pin configuration & disable COM Left/Right Remp */
        ALT_DIS=0x12, /* one 128x64 display uses this. alternate COM pin configuration & disable COM Left/Right Remp */
        SEQ_EN=0x22,  /* sequential COM pin configuration & enable COM Left/Right Remp */
        ALT_EN=0x32,  /* alternate COM pin configuration & enable COM Left/Right Remp */
    };

    /**
     * @brief Construct a new Ssd1306_common object
     * 
     * @param port_ The physicial interface for writing commands and data
     * @param com_pin_config the COM pin configuration; see enum class Com_pin_cfg
     * @param landscape_x_max_ the number of horizontal pixels if the screen is in landscape mode
     * @param landscape_y_max_ the number of vertical pixels if the screen is in landscape mode
     * @param first_column_ the first display RAM column number for a non-rotated display
     * @param first_page_  the first display RAM page number for a non-rotated display
     */
    Ssd1306(Ssd1306hw* port_, Com_pin_cfg com_pin_config=Com_pin_cfg::ALT_DIS, 
            uint8_t landscape_x_max_ = 128, uint8_t landscape_y_max_=64,
            uint8_t first_column_=0, uint8_t first_page_=0);

    //-----------------------------------------------------------------------------
    // Class API interface functions
    //-----------------------------------------------------------------------------

    /**
     * @brief write a buffer of bytes to the display in the rectangle of bytes
     * defined by (col, page) and (last_col, last_page).
     * 
     * If you have a region of the screen that updates frequently, you may
     * get better performance if you update only that section of the screen.
     * @param buffer a buffer that represents the sequential bytes in the
     * screen rectangle defined by (col, page) and (last_col, last_page).
     * @param nbytes the number of bytes to write. Must be (last_col-col+1) *
     * (last_page-page+1).
     * @param col the column of the first byte of the display memory to write
     * @param page the page of the first byte of display memory to write
     * @param last_col the column of the last byte of the display memory to write.
     * If it is 0, then it is set to landscape_width-1
     * @param last_page the page of the last byte of the display memory to write.
     * If it is 0, then it is set to num_pages-1
     * @return true if the write was successful
     * @return false if the write failed
     * @note to write to the whole display memory, omit row and col parameters
     * and set nbytes to the full display size
     */
    bool write_display_mem(const uint8_t* buffer, size_t nbytes, 
        uint8_t col=0, uint8_t page=0, uint8_t last_page=0, uint8_t last_col=0);

    /**
     * @brief set every byte of the display memory to 0
     *
     * If the display is set up for normal, this will blank the display.
     * If the display is set up for inverse video, this will turn on every pixel.
     *
     * @return true if the write was successful
     * @return false if the write failed
     */
    bool clear_display_mem();

    /**
     * @brief perform the initialization sequence for the display
     * @param rotation the initial display orientation.
     * 
     * @return true if successful, false otherwise
     */
    bool init(Display_rotation rotation);

    /**
     * @brief perform the shutdown sequence for the display
     * 
     * @return true if successful, false otherwise
     */
    bool deinit();

    /**
     * @brief Set the display contrast
     * 
     * @param contrast the display contrast 0-255
     * @return true 
     * @return false 
     */
    bool set_contrast(uint8_t contrast=255);

    /**
     * @brief set the display rotation
     * 
     * The SSD1306 supports changing the segment scanning
     * direction, the dot row scanning direction, and the
     * memory write address autoincrement mode for portrait
     * and landscape modes. You must completely rewrite the
     * display memory after changing the display rotation.
     * 
     * @param rotation_ the display rotation constant
     */
    bool set_display_rotation(rppicomidi::Display_rotation rotation_=rppicomidi::Display_rotation::Landscape0);

    /**
     * @brief Set the pixel at location x, y, on the memory buffer
     * canvas[0:nbytes_in_canvas-1] as specified by the Pixel_state.
     * 
     * This function uses the screen dimensions and the current screen rotation
     * to correctly. The reason this function is in this class is the canvas
     * memory organization is a property of the display chip and not a higher
     * level graphics driver.
     */
    void set_pixel_on_canvas(uint8_t* canvas, size_t nbytes_in_canvas, uint8_t x, uint8_t y, Pixel_state value);

    /**
     * @brief Get the display rotation object value
     * 
     * @return Display_rotation 
     */
    inline Display_rotation get_display_rotation() {return rotation;}

    /**
     * @brief Get the screen width for the current screen rotation
     * 
     */
    inline uint8_t get_screen_width() {return is_portrait?landscape_height:landscape_width; }

    /**
     * @brief Get the screen height for the current screen rotation
     * 
     */
    inline uint8_t get_screen_height() {return is_portrait?landscape_width:landscape_height; }

    inline size_t get_minimum_canvas_size() {return num_pages * landscape_width; }
protected: // protected and not private because you may want to base SH1106 on this class
    Ssd1306hw* port;
    Com_pin_cfg com_pin_cfg;
    uint8_t landscape_width;
    uint8_t landscape_height;
    uint8_t first_column;
    uint8_t first_page;
    uint8_t num_pages;
    uint8_t contrast;
    Display_rotation rotation;
    bool is_portrait;
    void get_rotation_constants(uint8_t& remap_cmd, uint8_t& com_dir_cmd, uint8_t& addr_mode);
    bool write_command_list(const uint8_t* cmd_list, size_t cmd_list_len);
};
}
