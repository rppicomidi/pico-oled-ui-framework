#include <cstring>
#include "mc_channel_text.h"
#include "pico/assert.h"

rppicomidi::Mc_channel_text::Mc_channel_text(Mono_graphics& screen_, uint8_t x_, uint8_t y_, uint8_t channel_, const MonoMonoFont& font_) :
    screen{screen_}, x{x_}, y{y_}, channel{channel_}, font{font_}
{
    // pad with ' '  1234567
    strcpy(text[0], "       ");
    strcpy(text[1], "       ");
    draw();
}

void rppicomidi::Mc_channel_text::draw()
{
    for (int idx = 0; idx < 2; idx++) {
        screen.draw_string(font, x, y + idx* font.height, text[idx], 7, Pixel_state::PIXEL_ONE, Pixel_state::PIXEL_ZERO);
    }
}

void rppicomidi::Mc_channel_text::set_text(uint8_t line, uint8_t offset, const char* text_)
{
    assert(line < 2);
    assert(offset < 7);
    // non-destructive (don't add null termination) strncpy
    for (int idx=offset; idx < 7 && text_[idx] != '\0'; idx++)
        text[line][idx] =  text_[idx];
    draw();
}

void rppicomidi::Mc_channel_text::set_text_by_mc_sysex(const uint8_t* sysex_message, uint8_t num_chars)
{
    assert(num_chars >= 1);
    assert(sysex_message);
    uint8_t offset= sysex_message[0];
    uint8_t line_offset = channel *7;
    for (int line = 0; line < 2; line++) {
        if (line_offset >= offset) {
            // then there may be characters for this field
            uint8_t message_char_idx = 1+line_offset-offset;
            for (int ch_idx = 0; ch_idx < 7 && message_char_idx <= num_chars; ch_idx++) {
                text[line][ch_idx] = sysex_message[message_char_idx++];
            }
        }
        line_offset+=56;
    }
    draw();
}