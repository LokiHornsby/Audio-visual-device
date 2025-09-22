#pragma once

// https://forum.arduino.cc/t/max7219-and-8x8led-matrix-all-leds-are-constantly-on/123887/2
// https://www.analog.com/media/en/technical-documentation/data-sheets/max7219-max7221.pdf
// https://www.dropbox.com/scl/fi/priug31ipifdky06i5uw9/libraries.rar?dl=0&e=1&file_subpath=%2FMD_MAX72XX%2Fsrc%2FMD_MAX72xx.cpp&rlkey=mebs7nk8my03sc72g9kvqzgg3
// https://youngkin.github.io/post/spidotmatrixmodule/#controlling-the-max7219---main-program
// https://github.com/raspberrypi/pico-examples/blob/master/spi/max7219_32x8_spi/max7219_32x8_spi.c
// https://www.friendlywire.com/tutorials/max7219/
namespace digishuo {
    class max7219 {
        public:
            int DISPLAYS = 4;
            max7219();
            void write(uint8_t reg, uint8_t data);
            void update();
    };
}

namespace pimoroni {
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
            PicoRGBKeypad();
            void update();
            void set_brightness(float brightness);
            void illuminate(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
            void illuminate(uint8_t i, uint8_t r, uint8_t g, uint8_t b);

            void clear();
            uint16_t get_button_states();
    };
}