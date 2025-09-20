#include <string.h>
#include <math.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"

#include "hardware.hpp"
#include <cstdint>
#include <bitset>

namespace digishuo {
    enum pin {
        CLK = 10,
        DIN = 11,
        CS = 13
    };

    // https://github.com/raspberrypi/pico-examples/blob/master/spi/max7219_32x8_spi/max7219_32x8_spi.c
    void max7219::init(){
        /*gpio_init(pin::CLK);
        gpio_init(pin::DIN); 
        gpio_init(pin::CS); 

        spi_init(spi1, 4000000); //4000000 / 8000000 / 8000 * 1000);

        spi_set_format( spi1,   // SPI instance
                    16,         // Number of bits per transfer
                    SPI_CPOL_0,      // Polarity (CPOL)
                    SPI_CPHA_0,      // Phase (CPHA)
                    SPI_MSB_FIRST);

        gpio_set_function(pin::CLK, GPIO_FUNC_SPI);
        gpio_set_function(pin::DIN, GPIO_FUNC_SPI);
        gpio_set_function(pin::CS, GPIO_FUNC_SPI);
        /*gpio_set_function(pin::CS, GPIO_FUNC_SIO);
        gpio_set_dir(pin::CS, GPIO_OUT);
        gpio_put(pin::CS, true);

        sleep_ms(50);*/

        stdio_init_all();

        spi_init(spi1, 10 * 1000 * 1000);

        gpio_set_function(pin::CLK, GPIO_FUNC_SPI);
        gpio_set_function(pin::DIN, GPIO_FUNC_SPI);

        gpio_init(pin::CS);
        gpio_set_dir(pin::CS, GPIO_OUT);
        gpio_put(pin::CS, 1);
    }

    void max7219::cs_select() {
        //asm volatile("nop \n nop \n nop");
        gpio_put(pin::CS, 0);  // Active low
        //asm volatile("nop \n nop \n nop");
    }

    void max7219::cs_deselect() {
        //asm volatile("nop \n nop \n nop");
        gpio_put(pin::CS, 1);
        //asm volatile("nop \n nop \n nop");
    }

    void max7219::write_register_all(uint8_t reg, uint8_t data) {
        uint8_t buf[2];
        buf[0] = reg;
        buf[1] = data;

        cs_select();

        for (int i = 0; i< 1;i++) { // num of matrixes
            spi_write_blocking(spi1, buf, 2);
        }

        cs_deselect();
    }

    void max7219::write(const uint16_t *buffer){
        gpio_put(pin::CS, false);
        
        spi_write16_blocking(spi1, buffer, sizeof(buffer));
        
        gpio_put(pin::CS, true);
    }

    // https://github.com/pweb002/LED-Matrix-Clock/blob/main/max7219.cpp
    void max7219::test(bool x) {
        gpio_put(pin::CS, false); //set Load Pin to low

        bool address [8] = {false, false, false, false, true, true, true, true};
        bool data [8] = {false, false, false, false, false, false, false, x};

        for (int j = 3; j >= 0; --j) {
            for (int i = 0; i < 8; ++i) {
                gpio_put(pin::CLK, false); //Disable Read on the max7219
                gpio_put(pin::DIN, address[i]); //set the Data Pin to value
                gpio_put(pin::CS, true); //Enable Read on the max7219

                busy_wait_us(1);
            }

            for (int i = 0; i < 8; ++i) {
                gpio_put(pin::CLK, false); //Disable Read on the max7219
                gpio_put(pin::DIN, data[i]); //set the Data Pin to value
                gpio_put(pin::CS, true); //Enable Read on the max7219

                busy_wait_us(1);
            }
        }

        gpio_put(pin::CS, true); //Load the Bits into the max7219
    }
}

namespace pimoroni {
    // pins
    enum pin {
        SDA       =  4,
        SCL       =  5,
        CS        = 17,
        SCK       = 18,
        MOSI      = 19
    };

    // constructor
    PicoRGBKeypad::PicoRGBKeypad() {
        memset(buffer, 0, sizeof(buffer));
        led_data = buffer + 4;

        set_brightness(1); //Must be called to init each LED frame

        // setup i2c interface
        i2c_init(i2c0, 400000);
        gpio_set_function(pin::SDA, GPIO_FUNC_I2C); gpio_pull_up(pin::SDA);
        gpio_set_function(pin::SCL, GPIO_FUNC_I2C); gpio_pull_up(pin::SCL);

        spi_init(spi0, 4 * 1024 * 1024);
        gpio_set_function(pin::CS, GPIO_FUNC_SIO);
        gpio_set_dir(pin::CS, GPIO_OUT);
        gpio_put(pin::CS, 1);
        gpio_set_function(pin::SCK, GPIO_FUNC_SPI);
        gpio_set_function(pin::MOSI, GPIO_FUNC_SPI);

        update();
    }

    void PicoRGBKeypad::update() {
      gpio_put(pin::CS, 0);
      spi_write_blocking(spi0, buffer, sizeof(buffer));
      gpio_put(pin::CS, 1);
    }

    void PicoRGBKeypad::set_brightness(float brightness) {
      if(brightness < 0.0f || brightness > 1.0f) {
        return;
      }

      for(uint16_t i = 0; i < NUM_PADS; i++)
          led_data[i * 4] = 0b11100000 | (uint8_t)(brightness * (float)0b11111);
    }

    void PicoRGBKeypad::illuminate(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
      if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
        return;
      }

      uint16_t offset = (x + (y * WIDTH)) * 4;
      //led_data[offset + 0] = 0xff;  //Not needed as set at init
      led_data[offset + 1] = b;
      led_data[offset + 2] = g;
      led_data[offset + 3] = r;
    }

    void PicoRGBKeypad::illuminate(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
      if(i < 0 || i >= NUM_PADS) {
        return;
      }

      uint16_t offset = i * 4;
      //led_data[offset + 0] = 0xff;  //Not needed as set at init
      led_data[offset + 1] = b;
      led_data[offset + 2] = g;
      led_data[offset + 3] = r;
    }

    void PicoRGBKeypad::clear() {
      for(uint16_t i = 0; i < NUM_PADS; i++)
        illuminate(i, 0, 0, 0);
    }

    uint16_t PicoRGBKeypad::get_button_states() {
      uint8_t i2c_read_buffer[2];
      uint8_t reg = 0;
      i2c_write_blocking(i2c0, KEYPAD_ADDRESS, &reg, 1, true);
      i2c_read_blocking(i2c0, KEYPAD_ADDRESS, i2c_read_buffer, 2, false);
      return ~((i2c_read_buffer[0]) | (i2c_read_buffer[1] << 8));
    }

}