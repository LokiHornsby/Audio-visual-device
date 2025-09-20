#pragma once

namespace digishuo {
    class max7219 {
        public:
            void init();
            void write(const uint16_t *buffer);
            void test(bool x);
            void write_register_all(uint8_t reg, uint8_t data);
            void cs_select();
            void cs_deselect();
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