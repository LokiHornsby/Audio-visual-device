#pragma once

#include <bits/stdc++.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "hardware/adc.h"
#include <stdint.h>

int binarytoint(std::bitset<8> a);

// https://www.analog.com/media/en/technical-documentation/data-sheets/max7219-max7221.pdf
// https://www.friendlywire.com/tutorials/max7219/
// https://github.com/raspberrypi/pico-examples/blob/master/spi/max7219_32x8_spi/max7219_32x8_spi.c
// LED matrix 
namespace digishuo {
    // VCC - VBUS/VSYS
    // GND - GND
    // DIN - GP11
    // CS - GP13
    // CLK - GP10
    enum pin {
        CLK = 10,
        DIN = 11,
        CS = 13
    };

    class MAX7219 {
        public:
            #define MATRIX_DISPLAYS 16
            #define MATRIX_WIDTH 8
            #define MATRIX_HEIGHT 8
            
            int brightness [16] { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
            
            void init();
            void write(uint8_t reg, uint8_t data, bool block); // (define first set of rows - draw set of rows to each display - move onto next row)
            void update(); 
            void clear();
            std::bitset<MATRIX_WIDTH> columns;
    };
}

// Microphone
namespace adafruit {
    // GND - AGND
	// VDD - VREF
	// OUT - GP26

    class MAX981 {
        public:
            void init();
            float getVoltage();
            float getPeak();
    };
}

// https://shop.pimoroni.com/products/pico-rgb-keypad-base?variant=32369517166675
// RGB keypad
namespace pimoroni {
    enum pin {
        SDA       =  4,
        SCL       =  5,
        CS        = 17,
        SCK       = 18,
        MOSI      = 19
    };

    class PicoRGBKeypad {
        private:
            static const uint8_t KEYPAD_ADDRESS = 0x20;
        public:
            static const int WIDTH = 4;
            static const int HEIGHT = 4;
            static const int NUM_PADS = WIDTH * HEIGHT;

        private:
            uint8_t buffer[(NUM_PADS * 4) + 8];
            uint8_t *led_data;
        
        public:
            void init();
            void update();
            void set_brightness(float brightness);
            void illuminate(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
            void illuminate(uint8_t i, uint8_t r, uint8_t g, uint8_t b);

            void clear();
            uint16_t get_button_states();
    };
}