#include "pico/stdlib.h"
#include <string.h>
#include <math.h>

// Pimoroni RGB keypad lib
#include "pico_rgb_keypad.cpp"
#include "pico_rgb_keypad.hpp"
using namespace pimoroni;
PicoRGBKeypad pico_rgb_keypad;
uint16_t current_buttons;

int main() {
  	// LED
  	gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	gpio_put(PICO_DEFAULT_LED_PIN, true);
	sleep_ms(250);
	gpio_put(PICO_DEFAULT_LED_PIN, false);

	// Keypad
    pico_rgb_keypad.init(); // Set up GPIO
    pico_rgb_keypad.set_brightness(0.5f);

    int g = 0;

    while (true){
        current_buttons = pico_rgb_keypad.get_button_states();

        for (size_t i = 0; i < 16; i++){
            if (current_buttons & (1 << i)) {
                pico_rgb_keypad.illuminate(i, 255, 255, 255);
            } else {
                pico_rgb_keypad.illuminate(i, 0, 0, 0);
            }
        }

        pico_rgb_keypad.update();
    }
}
