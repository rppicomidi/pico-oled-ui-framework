/**
 * @file ssd1306.cpp
 * @brief see ssd1306.h for a description
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
#include "ssd1306.h"
#include "assert.h"
// From the SSD1306 datasheet
#define SET_MEM_ADDR_MODE 0x20 /* follow this byte by one of the following values */
#define ADDR_MODE_HORIZONTAL 0
#define ADDR_MODE_VERTICAL   1
#define ADDR_MODE_PAGE       2

// Only for horizontal or vertical mode
#define SET_COL_ADDR 0x21   /* follow this byte by 2 bytes: the 1st column number & the last column number (0-127)*/
#define SET_PAGE_ADDR 0x22  /* follow this byte by 2 bytes: the 1st page number & the last page number (0-7)*/

#define SET_DISP_START_LINE(first) (0x40+(first)) /* where first is 0-63 */

#define SET_CONTRAST 0x81   /* follow this byte by 1 byte: the contrast value 0-0xFF */
#define SET_SEGMENT_REMAP(remap) ((remap)?0xA1:0xA0) /* remap is true to map col 127 to SEG0, false to map col 0 to SEG0 */
#define SET_ENTIRE_ON 0xA4  /* make the pixels follow the display memory */
#define FORCE_ENTIRE_ON 0xA5 /* light up every pixel no matter what is in the display memory */
#define SET_DISP_NORM 0xA6   /* display RAM bit 1 corresponds to pixel on, 0 corresponds to off */
#define SET_DISP_INV  0xA7   /* display RAM bit 0 corresponds to pixel on, 1 corresponds to off */
#define SET_MUX_RATIO 0xA8   /* follow this byte by 1 byte: the MUX ratio-1, where MUX ratio is 16-64 */

#define SET_DISP_ON 0xAF /*wake OLED from sleep*/
#define SET_DISP_OFF 0xAE /*put OLED to sleep*/
#define SET_COM_OUT_DIR(forward) ((forward)?0xC0:0xC8) /* forware=true scans COM from COM0-COM[N-1], false from COM[N-1]-COM[0] */
#define SET_DISP_OFFSET 0xD3 /* follow this command by one byte: the vertical shift of the display, 0-63 */

#define SET_COM_PIN_CFG 0xDA /* follow this command by one byte described by one of the combinations below */

#define SET_DISP_CLK_DIV_FREQ 0xD5   /* follow this command by one byte described by the macro below */
/*divider is the divide ratio-1; frequency is the osc frequency */
#define DISP_CLK_DIV_FREQ(divider, frequency) (((divider)&0xF) | (((frequency)&0xF)<<4))

#define SET_PRECHARGE 0xD9 /* follow this command by one byte described by the macro below */
/* phase 1 period, phase 2 period. ph1 and ph2 1-15 (0 illegal) */
#define PRECHARGE_PERIOD(ph1, ph2) (((ph1)&0xF) | (((ph2)&0xF)<<4))

#define SET_VCOM_DESEL 0xDB /* follow this command by one byte described by the macro below */
#define VCOM_DESEL(level) (((level)&0x7)<<4) // see datasheet for level values; default level is 2
#define SET_CHARGE_PUMP 0x8D /* follow this command by one byte described by the macro below */
#define CHARGE_PUMP_CTRL(enable) ((enable)?0x14:0x10)


rppicomidi::Ssd1306::Ssd1306(Ssd1306hw* port_, Com_pin_cfg com_pin_cfg_, uint8_t landscape_width_, uint8_t landscape_height_, uint8_t first_column_, uint8_t first_page_)
    : port{port_}, com_pin_cfg{com_pin_cfg_}, landscape_width{landscape_width_}, landscape_height{landscape_height_},
  first_column{first_column_}, first_page{first_page_}, num_pages{static_cast<uint8_t>(landscape_height_/8)}, contrast{255}
{
}


void rppicomidi::Ssd1306::get_rotation_constants(uint8_t& remap_cmd, uint8_t& com_dir_cmd, uint8_t& addr_mode)
{
        switch (rotation) {
        default:
        case Display_rotation::Landscape0:
            remap_cmd = SET_SEGMENT_REMAP(false);
            com_dir_cmd = SET_COM_OUT_DIR(true);
            addr_mode = ADDR_MODE_HORIZONTAL;
            is_portrait = false;
            break;
        case Display_rotation::Portrait90:
            remap_cmd = SET_SEGMENT_REMAP(true);
            com_dir_cmd = SET_COM_OUT_DIR(true);
            addr_mode = ADDR_MODE_VERTICAL;
            is_portrait = true;
            break;
        case Display_rotation::Landscape180:
            remap_cmd = SET_SEGMENT_REMAP(true);
            com_dir_cmd = SET_COM_OUT_DIR(false);
            addr_mode = ADDR_MODE_HORIZONTAL;
            is_portrait = false;
            break;            
        case Display_rotation::Portrait270:
            remap_cmd = SET_SEGMENT_REMAP(false);
            com_dir_cmd = SET_COM_OUT_DIR(false);
            addr_mode = ADDR_MODE_VERTICAL;
            is_portrait = true;
            break;
    }
}

bool rppicomidi::Ssd1306::write_command_list(const uint8_t* cmd_list, size_t cmd_list_len)
{
    bool success = true;
    uint8_t nbytes = 0;
    for (int idx=0; success && idx < cmd_list_len; idx+=nbytes) {
        nbytes = cmd_list[idx++];
        success = port->write_command(cmd_list+idx, nbytes);
    }
    return success;
}

bool rppicomidi::Ssd1306::init(Display_rotation rotation_)
{
    bool success = true;
    rotation = rotation_;
    uint8_t remap_cmd, com_dir_cmd, addr_mode;
    get_rotation_constants(remap_cmd, com_dir_cmd, addr_mode);

    uint8_t init_commands[] = {
        // total number of bytes in command followed by all of the bytes
        1, SET_DISP_OFF,
        2, SET_MEM_ADDR_MODE, addr_mode,
        1, SET_DISP_START_LINE(0),
        1, remap_cmd,
        2, SET_MUX_RATIO, (uint8_t)(landscape_height-1),
        1, com_dir_cmd,
        2, SET_DISP_OFFSET, 0,
        2, SET_DISP_CLK_DIV_FREQ, DISP_CLK_DIV_FREQ(0,8),
        2, SET_PRECHARGE, PRECHARGE_PERIOD(2,2),
        2, SET_COM_PIN_CFG, static_cast<uint8_t>(com_pin_cfg),
        2, SET_VCOM_DESEL, VCOM_DESEL(4),
        2, SET_CONTRAST, contrast,
        1, SET_DISP_NORM,
        2, SET_CHARGE_PUMP, CHARGE_PUMP_CTRL(true),
        1, SET_ENTIRE_ON,
        1, SET_DISP_ON,
    };
    uint8_t nbytes = 0;
    for (int idx=0; success && idx < sizeof(init_commands); idx+=nbytes) {
        nbytes = init_commands[idx++];
        success = port->write_command(init_commands+idx, nbytes);
    }
    return success;
}

bool rppicomidi::Ssd1306::set_contrast(uint8_t contrast_)
{
    contrast = contrast_;
    uint8_t cmd[] = {SET_CONTRAST, contrast};
    return port->write_command(cmd, sizeof(cmd));
}

bool rppicomidi::Ssd1306::set_display_rotation(rppicomidi::Display_rotation rotation_)
{
    rotation = rotation_;
    uint8_t remap_cmd, com_dir_cmd, addr_mode;
    get_rotation_constants(remap_cmd, com_dir_cmd, addr_mode);
    const uint8_t cmd_list[] = {
        2, SET_MEM_ADDR_MODE, addr_mode,
        1, remap_cmd,
        1, com_dir_cmd,
    };
    return write_command_list(cmd_list, sizeof(cmd_list));
}

bool rppicomidi::Ssd1306::write_display_mem(const uint8_t* buffer, size_t nbytes, uint8_t col, uint8_t page)
{
    assert(buffer);
    assert(nbytes);
    const uint8_t cmd_list[] = {
        3, SET_PAGE_ADDR, page, (uint8_t)((num_pages)-1),
        3, SET_COL_ADDR, col, (uint8_t)(landscape_width-1),
    };
    static const uint8_t reset_page_cmd[] = {SET_PAGE_ADDR, page, (uint8_t)((num_pages)-1)};
    bool success = write_command_list(cmd_list, sizeof(cmd_list));
    if (success) {
        success = port->write_data(buffer, nbytes);
    }
    return success;
}

bool rppicomidi::Ssd1306::clear_display_mem()
{
    uint8_t canvas[num_pages * landscape_width];
    memset(canvas, 0, sizeof(canvas));
    return write_display_mem(canvas, sizeof(canvas), first_column, first_page);
}

void rppicomidi::Ssd1306::set_pixel_on_canvas(uint8_t* canvas, size_t nbytes_in_canvas, uint8_t x, uint8_t y, Pixel_state value)
{
    assert(canvas);
    assert(nbytes_in_canvas);
    if (value == Pixel_state::PIXEL_TRANSPARENT)
        return; // nothing to do
    size_t idx;
    int bit;
    if (is_portrait) {
        // then bytes go left to right LSB to MSB in rows of num_pages bytes
        assert(x < landscape_height);
        assert(y < landscape_width);
        int page = x/8;
        idx = page + y*num_pages;
        assert(idx < nbytes_in_canvas);
        bit = x % 8;
    }
    else {
        // then bytes go top to bottom LSB to MSB in columns of num_pages bytes
        assert(y < landscape_height);
        assert(x < landscape_width);
        int page = y/8;
        idx = (page*landscape_width) + x;
        assert(idx < nbytes_in_canvas);
        bit = (y % 8);
    }
    uint8_t mask = 1 << bit;
    switch (value) {
        default:
            assert(false); // illegal value
            break;
        case Pixel_state::PIXEL_ZERO:
            // clear the bit
            canvas[idx] &= ~mask;
            break;
        case Pixel_state::PIXEL_ONE:
            // set the bit
            canvas[idx] |= mask;
            break;
        case Pixel_state::PIXEL_XOR:
            // flip the bit
            canvas[idx] ^= mask;
            break;
    }
}
