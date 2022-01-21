/**
 * @file i2c_ssd1306.h
 * @brief This class implements I2C communication between the Raspberry Pi
 *  RP2040 chipand the SSD1306. It is inteded to operate as a template class
 *  argument
 * 
 * This class is a C++ Raspberry Pi Pico port of the LibDriver 
 * C SSD1306 driver code found here: https://github.com/hepingood/ssd1306
 * Functions copied based on the Raspberry Pi Pico SDK are noted in the
 * comments below. MIT License for those files are in their respective 
 * project directories
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
#include "hardware/i2c.h"
#include "pico/timeout_helper.h"
#include "ssd1306hw.h"
namespace rppicomidi {
class Ssd1306i2c : public Ssd1306hw
{
public:
    /**
     * @brief Construct a new i2c ssd1306 object
     * 
     * @param i2c_port the hardware handle for the I2C port in a struct
     * @param i2c_addr the I2C address of the display
     * @param sda_gpio the GPIO number of the I2C SDA signal
     * @param scl_gpio //the GPIO number of the I2C SCL signal
     */
    Ssd1306i2c(i2c_inst_t* i2c_port, uint8_t i2c_addr, uint8_t sda_gpio, uint8_t scl_gpio);

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
    i2c_inst_t* i2c_port;
    uint8_t i2c_addr;

    /**
     * @brief test if the address is reserved (copied from pico-sdk)
     */
    inline bool i2c_reserved_addr(uint8_t addr) {
        return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
    }

    /**
     * @brief This is the same as pico-sdk i2c_write_blocking_internal except this function
     * sends the regbyte byte before sending all of the data and the return value is len+1 on success
     * 
     * @param regbyte 8-bit register byte; either 0 for SSD1306 commands or 0x40 for display data
     */
    int write_blocking_internal(i2c_inst_t *i2c, uint8_t addr, uint8_t regbyte, const uint8_t *src, size_t len, bool nostop,
                                       ::check_timeout_fn timeout_check, struct ::timeout_state *ts);

    /**
     * @brief This function is the same as pico-sdk i2c_write_blocking except this function
     * sends the regbyte byte before sending all of the data and the return value is len+1 on success
     * 
     * @param regbyte 8-bit register byte; either 0 for SSD1306 commands or 0x40 for display data
     */
    inline int write_blocking(i2c_inst_t *i2c, uint8_t addr, uint8_t regbyte, const uint8_t *src, size_t len, bool nostop) {
        return write_blocking_internal(i2c, addr, regbyte, src, len, nostop, NULL, NULL);
    }   

    /**
     * @brief This function is the same as pico-sdk i2c_write_blocking_until except this function
     * sends the regbyte byte before sending all of the data and the return value is len+1 on success
     * 
     * @param regbyte 8-bit register byte; either 0 for SSD1306 commands or 0x40 for display data
     */
    inline int write_blocking_until(i2c_inst_t *i2c, uint8_t regbyte, uint8_t addr, const uint8_t *src, size_t len, bool nostop,
                                absolute_time_t until) {
        timeout_state_t ts;
        return write_blocking_internal(i2c, addr, regbyte, src, len, nostop, init_single_timeout_until(&ts, until), &ts);
    }
};
}