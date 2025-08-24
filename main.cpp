#include "pico/stdlib.h"
#include <string.h>
#include <math.h>

// Pimoroni RGB keypad lib
#include "pico_rgb_keypad.cpp"
#include "pico_rgb_keypad.hpp"
using namespace pimoroni;
PicoRGBKeypad pico_rgb_keypad;
uint16_t current_buttons;

// FFT
// https://www.fftw.org/
// https://www.fftw.org/fftw3_doc/Complex-One_002dDimensional-DFTs.html
// https://github.com/mborgerding/kissfft
// https://github.com/AlexFWulff/awulff-pico-playground/tree/main/adc_fft

// Input
int arr [64] = { };

int main() {
  	// onboard LED
  	gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	gpio_put(PICO_DEFAULT_LED_PIN, true);
	sleep_ms(250);
	gpio_put(PICO_DEFAULT_LED_PIN, false);

    // Input
    gpio_init(3);
    gpio_get(3);
    // get current frequency from GPIO pin (how? - gpio_get only returns LOW or HIGH) (can we record the LOW and HIGH as a sinewave?)
    // capture 64 (or more?) times and feed into array
    // feed array into FFT
    // use output frequency to change buttons animation

	// Keypad
    pico_rgb_keypad.init(); // Set up GPIO
    pico_rgb_keypad.set_brightness(0.5f);

    int r [16] = { };
    int g [16] = { };
    int b [16] = { };

    while (true){
        current_buttons = pico_rgb_keypad.get_button_states();

        for (size_t i = 0; i < 16; i++){

            if (current_buttons & (1 << i)) {
                r[i] = 255;
                g[i] = 255;
                b[i] = 255;
            } 

            if (r[i] > 0){
                r[i]--;
            }

            if (g[i] > 0){
                g[i]--;
            }

            if (b[i] > 0){
                b[i]--;
            }

            pico_rgb_keypad.illuminate(i, r[i], g[i], b[i]);
        }

        sleep_ms(1);   
       
        pico_rgb_keypad.update();
    }
}
