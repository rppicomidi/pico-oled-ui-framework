/**
 * @file ssd1306pioi2c.h
 * @brief This class implements I2C communication between the Raspberry Pi
 * RP2040 chip and the SSD1306 using a PIO module instead of one of the I2C
 * interface blocks. It is a C++ port of of the PIO I2C example in pico-examples.
 * Uses BSD 3-clause license from the code it uses. See
 * https://github.com/raspberrypi/pico-examples/tree/master/pio/i2c for original
 * code and https://github.com/raspberrypi/pico-examples/blob/master/LICENSE.TXT
 * for the license.
 *
 * Original source
 * Copyright (c) 2021 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * This source
 * Copyright (c) 2022 rppicomidi
 * same license
 */
#pragma once
#include <cstdint>
#include "i2c.pio.h"
#include "hardware/pio.h"
#include "ssd1306hw.h"
namespace rppicomidi {
class Ssd1306pio_i2c : public Ssd1306hw
{
public:
    /**
     * @brief Construct a new i2c ssd1306 object
     * 
     * @param pio_instance_ the hardware handle for the PIO
     * @param state_machine_ the state machine to execute the I2C
     * @param i2c_addr the I2C address of the display
     * @param sda_gpio the GPIO number of the I2C SDA signal
     * @param scl_gpio //the GPIO number of the I2C SCL signal
     */
    Ssd1306pio_i2c(pio_hw_t* pio_instance_, uint state_machine_, uint8_t i2c_addr, uint8_t sda_gpio, uint8_t scl_gpio);

    /**
     * @brief Write a command byte followed by 0 or more argument bytes to the SSD1306
     * 
     * @param command a pointer to a uint8_t array containing the command and command data
     * @param nbytes the number of bytes in the command array.
     * @return true if the write was successful
     * @return false if the write failed
     */
    bool write_command(const uint8_t* command, uint8_t nbytes) final;

    /**
     * @brief Write display memory bytes to the SSD1306
     * 
     * @param data a pointer to a uint8_t array containing the data
     * @param nbytes the number of bytes in the data array.
     * @return true if the write was successful
     * @return false if the write failed
     */
    bool write_data(const uint8_t* data, size_t nbytes) final;
private:
    pio_hw_t* pio_instance;
    uint state_machine;
    uint8_t i2c_addr;

    /**
     * @brief test if the address is reserved (copied from pico-sdk)
     */
    inline bool i2c_reserved_addr(uint8_t addr) {
        return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
    }

    /**
     * @brief This is the similar to the pico-example for PIO I2C i2c_write_blocking_internal except
     *  this function sends the regbyte byte before sending all of the data and the return value is
     * len+1 on success, not 0.
     * 
     * @param regbyte 8-bit register byte; either 0 for SSD1306 commands or 0x40 for display data
     */
    int write_blocking(uint8_t addr, uint8_t regbyte, const uint8_t *src, size_t len);


    // ----------------------------------------------------------------------------
    // Low-level functions and data copied from the PIO I2C example
    const int PIO_I2C_ICOUNT_LSB = 10;
    const int PIO_I2C_FINAL_LSB  = 9;
    const int PIO_I2C_DATA_LSB   = 1;
    const int PIO_I2C_NAK_LSB    = 0;
    void pio_i2c_rx_enable(bool en);

    void pio_i2c_start();
    void pio_i2c_stop();
    void pio_i2c_repstart();

    bool pio_i2c_check_error();
    void pio_i2c_resume_after_error();

    // If I2C is ok, block and push data. Otherwise fall straight through.
    void pio_i2c_put_or_err(uint16_t data);
    uint8_t pio_i2c_get();
    inline void pio_i2c_put16(uint16_t data) {
        while (pio_sm_is_tx_fifo_full(pio_instance, state_machine))
            ;
        // some versions of GCC dislike this
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstrict-aliasing"
        *(io_rw_16 *)&pio_instance->txf[state_machine] = data;
    #pragma GCC diagnostic pop
    }

    void pio_i2c_wait_idle() {
        // Finished when TX runs dry or SM hits an IRQ
        pio_instance->fdebug = 1u << (PIO_FDEBUG_TXSTALL_LSB + state_machine);
        while (!(pio_instance->fdebug & 1u << (PIO_FDEBUG_TXSTALL_LSB + state_machine) || pio_i2c_check_error()))
            tight_loop_contents();
    }
};
}