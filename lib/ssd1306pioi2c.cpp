/**
 * @file ssd1306pioi2c.cpp
 * @brief This class implements I2C communication between the Raspberry Pi
 * RP2040 chip and the SSD1306 using a PIO module instead of one of the I2C
 * interface blocks. It is a C++ port of of the PIO I2C example in pico-examples.
 * Uses BSD 3-clause license from the code it uses. See
 * https://github.com/raspberrypi/pico-examples/tree/master/pio/i2c for original
 * code and https://github.com/raspberrypi/pico-examples/blob/master/LICENSE.TXT
 * for the license.
 *
 * Copyright (c) 2022 rppicomid
 */
#include "ssd1306pioi2c.h"
#include "assert.h"
#include "hardware/i2c.h" // for assertion check

rppicomidi::Ssd1306pio_i2c::Ssd1306pio_i2c(pio_hw_t* pio_instance_, uint state_machine_, uint8_t i2c_addr_, uint8_t sda_gpio, uint8_t scl_gpio) :
    pio_instance{pio_instance_}, state_machine{state_machine_}, i2c_addr{i2c_addr_}
{
    uint offset = pio_add_program(pio_instance_, &i2c_program);
    i2c_program_init(pio_instance_, state_machine_, offset, sda_gpio, scl_gpio);
}

bool rppicomidi::Ssd1306pio_i2c::write_command(const uint8_t* command_bytes, uint8_t nbytes)
{
    assert(command_bytes);
    assert(nbytes);
    bool success = false;
    success = (write_blocking(i2c_addr, 0x00, command_bytes, nbytes) == nbytes+1);
    return success;
}

bool rppicomidi::Ssd1306pio_i2c::write_data(const uint8_t* data, size_t nbytes)
{
    assert(data);
    assert(nbytes);
    return (write_blocking(i2c_addr, 0x40, data, nbytes) == nbytes+1);
}

// If I2C is ok, block and push data. Otherwise fall straight through.
void rppicomidi::Ssd1306pio_i2c::pio_i2c_put_or_err(uint16_t data) {
    while (pio_sm_is_tx_fifo_full(pio_instance, state_machine))
        if (pio_i2c_check_error())
            return;
    if (pio_i2c_check_error())
        return;
    // some versions of GCC dislike this
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
    *(io_rw_16 *)&pio_instance->txf[state_machine] = data;
#pragma GCC diagnostic pop
}

uint8_t rppicomidi::Ssd1306pio_i2c::pio_i2c_get() {
    return (uint8_t)pio_sm_get(pio_instance, state_machine);
}

void rppicomidi::Ssd1306pio_i2c::pio_i2c_start() {
    pio_i2c_put_or_err(1u << PIO_I2C_ICOUNT_LSB); // Escape code for 2 instruction sequence
    pio_i2c_put_or_err(set_scl_sda_program_instructions[I2C_SC1_SD0]);    // We are already in idle state, just pull SDA low
    pio_i2c_put_or_err(set_scl_sda_program_instructions[I2C_SC0_SD0]);    // Also pull clock low so we can present data
}

void rppicomidi::Ssd1306pio_i2c::pio_i2c_stop() {
    pio_i2c_put_or_err(2u << PIO_I2C_ICOUNT_LSB);
    pio_i2c_put_or_err(set_scl_sda_program_instructions[I2C_SC0_SD0]);    // SDA is unknown; pull it down
    pio_i2c_put_or_err(set_scl_sda_program_instructions[I2C_SC1_SD0]);    // Release clock
    pio_i2c_put_or_err(set_scl_sda_program_instructions[I2C_SC1_SD1]);    // Release SDA to return to idle state
};

void rppicomidi::Ssd1306pio_i2c::pio_i2c_repstart() {
    pio_i2c_put_or_err(3u << PIO_I2C_ICOUNT_LSB);
    pio_i2c_put_or_err(set_scl_sda_program_instructions[I2C_SC0_SD1]);
    pio_i2c_put_or_err(set_scl_sda_program_instructions[I2C_SC1_SD1]);
    pio_i2c_put_or_err(set_scl_sda_program_instructions[I2C_SC1_SD0]);
    pio_i2c_put_or_err(set_scl_sda_program_instructions[I2C_SC0_SD0]);
}

int rppicomidi::Ssd1306pio_i2c::write_blocking(uint8_t addr, uint8_t regbyte, const uint8_t *src, size_t len)
{
    invalid_params_if(I2C, addr >= 0x80); // 7-bit addresses
    invalid_params_if(I2C, i2c_reserved_addr(addr));
    int bytes_sent = 0;
    pio_i2c_start();
    pio_i2c_rx_enable(false);
    pio_i2c_put16((addr << 2) | 1u);
    
    while (!pio_i2c_check_error()) {
        if (!pio_sm_is_tx_fifo_full(pio_instance, state_machine)) {
            pio_i2c_put_or_err((regbyte << PIO_I2C_DATA_LSB) | 1u);
            ++bytes_sent;
            break;
        }
    }
    while (len && !pio_i2c_check_error()) {
        if (!pio_sm_is_tx_fifo_full(pio_instance, state_machine)) {
            --len;
            ++bytes_sent;
            pio_i2c_put_or_err((*src++ << PIO_I2C_DATA_LSB) | ((len == 0) << PIO_I2C_FINAL_LSB) | 1u);
        }
    }
    pio_i2c_stop();
    pio_i2c_wait_idle();
    if (pio_i2c_check_error()) {
        bytes_sent = -1; // signal an error
        pio_i2c_resume_after_error();
        pio_i2c_stop();
    }
    return bytes_sent;
}

bool rppicomidi::Ssd1306pio_i2c::pio_i2c_check_error() {
    return !!(pio_instance->irq & (1u << state_machine));
}

void rppicomidi::Ssd1306pio_i2c::pio_i2c_resume_after_error() {
    pio_sm_drain_tx_fifo(pio_instance, state_machine);
    pio_sm_exec(pio_instance, state_machine, (pio_instance->sm[state_machine].execctrl & PIO_SM0_EXECCTRL_WRAP_BOTTOM_BITS) >> PIO_SM0_EXECCTRL_WRAP_BOTTOM_LSB);
    pio_instance->irq = 1u << state_machine;
}

void rppicomidi::Ssd1306pio_i2c::pio_i2c_rx_enable(bool en) {
    if (en)
        hw_set_bits(&pio_instance->sm[state_machine].shiftctrl, PIO_SM0_SHIFTCTRL_AUTOPUSH_BITS);
    else
        hw_clear_bits(&pio_instance->sm[state_machine].shiftctrl, PIO_SM0_SHIFTCTRL_AUTOPUSH_BITS);
}
