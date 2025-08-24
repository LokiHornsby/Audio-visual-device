#include "pico/stdlib.h"

#include <string.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"

// Pimoroni RGB keypad lib
#include "pico_rgb_keypad.cpp"
#include "pico_rgb_keypad.hpp"
using namespace pimoroni;
PicoRGBKeypad pico_rgb_keypad;

int main() {
  	// LED
  	gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	gpio_put(PICO_DEFAULT_LED_PIN, true);
	sleep_ms(250);
	gpio_put(PICO_DEFAULT_LED_PIN, false);

	// Keypad
    pico_rgb_keypad.init(); // Set up GPIO
    pico_rgb_keypad.set_brightness(0.1f);

    for(auto y = 0u; y < pico_rgb_keypad.HEIGHT; y++) {
        for(auto x = 0u; x < pico_rgb_keypad.WIDTH; x++) {
            pico_rgb_keypad.illuminate(x, y, 255, 0, x * 5);
        }
    }

    pico_rgb_keypad.update();
}
