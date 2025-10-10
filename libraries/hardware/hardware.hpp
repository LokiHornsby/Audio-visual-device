#pragma once

#include <bits/stdc++.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "hardware/adc.h"

int binarytoint(std::bitset<8> a);

// https://www.analog.com/media/en/technical-documentation/data-sheets/max7219-max7221.pdf
// https://www.friendlywire.com/tutorials/max7219/
// https://github.com/raspberrypi/pico-examples/blob/master/spi/max7219_32x8_spi/max7219_32x8_spi.c
namespace digishuo {
    // VCC - VBUS
    // GND - AGND
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
            static const int DISPLAYS = 12;
            static const int WIDTH = 8;
            static const int HEIGHT = 8;
            void init();
            void write(uint8_t reg, uint8_t data, bool block);
            void update();
            void clear();
            std::bitset<digishuo::MAX7219::WIDTH> rows [digishuo::MAX7219::HEIGHT];
            void resetRows();
    };
}

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