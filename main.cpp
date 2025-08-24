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

// animation
int r [16] = { };
int g [16] = { };
int b [16] = { };

void setintensity(int in){
    // intensity graph
                
    // intensity : 1 
    r[0] = 255;
    
    // intensity : 2
    if (in > 0){
        r[1] = 200;
        r[4] = 200;
    }

    // intensity : 3
    if (in > 1){
        r[2] = 100;
        r[5] = 100;
        r[8] = 100;
    }
    
    // intensity : 4
    if (in > 2){
        r[3] = 50;
        r[6] = 50;
        r[9] = 50;
        r[12] = 50;
    }

    // intensity : 5
    if (in > 3){
        r[7] = 25;
        r[10] = 25;
        r[13] = 25;
    }

    // intensity : 6
    if (in > 4){
        r[11] = 15;
        r[14] = 15;
    }
    
    // intensity : 7
    if (in > 5){
        r[15] = 5;
    }
}

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

    while (true){
        current_buttons = pico_rgb_keypad.get_button_states();

        for (size_t i = 0; i < 16; i++){
            if (current_buttons & (1 << i)) {
                pico_rgb_keypad.set_brightness((float)(i + 1.00) / 16.00); // v (volume)
                setintensity(i); // e (energy)

                // highlight
                r[i] = 0;
                b[i] = 50;
                g[i] = 0;
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
