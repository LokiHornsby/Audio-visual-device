#include <math.h>
#include "pico/stdlib.h"
#include <stdio.h>

// include library
#include "Audio-visual-device.hpp"



/// @brief Display a bar graph on the keypad
/// @param v input voltage (as percentage)
/// @param r red
/// @param g green
/// @param b blue
void bar(float v, int r = 0, int g = 0, int b = 0){
    // def x and y
    int x = 0;
    int y = 3;

    // loop through each button
    for (int i = 0; i < 16; i++){
        // if the input voltage is higher than a divisor
        if (v > 0.0625f * i){
            // illuminate the key
            if (r == 0 && g == 0 && b == 0){
                int rv = 0;
                int gv = 0;
                if (y == 0) { rv = 255; }
                else if (y == 1) { rv = 255; gv = 255; }
                else if (y == 2){ gv = 255; } 
                else { gv = 255; }
                pico_rgb_keypad.illuminate(x, y, rv, gv, 0);
            } else {
                pico_rgb_keypad.illuminate(x, y, r, g, 0);
            }
        }

        // decrement y (start from bottom and go up)
        y--;

        // if at top then move to next column
        if (y < 0){
            x++;
            y = 3;
        }
    }
}

/// @brief Main function
/// @return 
int main (){
    // initialise pico rgb keypad
    pico_rgb_keypad.init();

    // initialise ADC
    adc_init();
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    // Select ADC input 0 (GPIO26)
    adc_select_input(0);

    while (true){ 
        // read voltage as percentage
        float v = adc_read() / 4096.0f; // 4096 == 1 << 12

        switch (calibrate_stage){
            case 0:
                // illuminate record button and fill the rest white
                for (int i = 0; i < 16; i++){
                    if (i != 15){
                        pico_rgb_keypad.illuminate(i, 50, 50, 50);
                    } else {
                        pico_rgb_keypad.illuminate(15, 0, 255, 0);
                    }
                }
                
                // if the record button is pressed change stage
                if (pico_rgb_keypad.get_button_states() & (0b1 << 15)){
                    calibrate_stage = 1;
                }

                break;

            case 1:
                pico_rgb_keypad.clear();

                // update all keypads
                for (int i = 0; i < 16; i++){
                    pico_rgb_keypad.illuminate(0, 255 - s_captures, s_captures, 0);
                }

                // if we've captured x amount of peaks      
                if (s_captures >= 255){
                    // calculate average silence using peaks
                    s = s / s_captures;

                    // clear visuals
                    pico_rgb_keypad.clear();

                    // change stage
                    calibrate_stage = 2;
                // if we haven't captured enough peaks and the button is released
                } else if (!(pico_rgb_keypad.get_button_states() & (0b1 << 15))) {
                    // reset
                    s = 0;
                    s_peak = 0;
                    s_fi = 0;
                    s_captures = 0;
                    calibrate_stage = 0;
                }

                // record peak
                if (v > s_peak){
                    s_peak = v;
                }

                // if the frames are over the limit
                if (s_fi == s_frames){
                    // record the most recent peak to "s" var
                    s += s_peak;

                    // increment capture index
                    s_captures++; 

                    // reset peak value
                    s_peak = 0;

                    // reset frame index
                    s_fi = 0;
                }

                // increment frame index
                s_fi++;

                break;

            case 2:
                // show bar
                pico_rgb_keypad.clear();
                //bar((v - s) * 1.0f);

                // record samples
                sample[sample_i] = (v - s) * 100.0f;
                sample_i++;

                if (sample_i == 64){
                    // feed sample into fft
                    
                    // initialize input data for FFT
                    int nfft = sizeof(sample) / sizeof(float); // nfft = 8

                    // allocate input/output 1D arrays
                    kiss_fft_cpx* cin = new kiss_fft_cpx[nfft];
                    kiss_fft_cpx* cout = new kiss_fft_cpx[nfft];

                    // initialize data storage
                    memset(cin, 0, nfft * sizeof(kiss_fft_cpx));
                    memset(cout, 0, nfft * sizeof(kiss_fft_cpx));

                    // copy the input array to cin
                    for (int i = 0; i < nfft; ++i)
                    {
                        cin[i].r = sample[i];
                    }

                    // setup the size and type of FFT: forward
                    bool is_inverse_fft = false;
                    kiss_fft_cfg cfg_f = kiss_fft_alloc(nfft, is_inverse_fft, 0, 0); // typedef: struct kiss_fft_state*

                    // execute transform for 1D
                    kiss_fft(cfg_f, cin , cout);

                    // transformed: DC is stored in cout[0].r and cout[0].i
                    printf("\nForward Transform:\n");
                    for (int i = 0; i < nfft; ++i)
                    {
                        printf("#%d  %f %fj\n", i, cout[i].r,  cout[i].i);
                    }

                    for (int i = 0; i < 16; i++){
                        pico_rgb_keypad.illuminate(i, 0, cout[i].i, 0);
                    }

                    // release resources
                    kiss_fft_free(cfg_f);
                    delete[] cin;
                    delete[] cout;

                    // reset index
                    sample_i = 0;
                }

                break;
            
            default:
                break;
        }

        pico_rgb_keypad.update();
    }

    return 0;
}

// FFT
//#include "kissfft/kiss_fft.c" - thanks to CMake we don't need to import the C code anymore
//#include <cstdlib>
// https://www.fftw.org/
// https://www.fftw.org/fftw3_doc/Complex-One_002dDimensional-DFTs.html
// https://github.com/mborgerding/kissfft
// https://github.com/AlexFWulff/awulff-pico-playground/tree/main/adc_fft

// frequency
// https://stackoverflow.com/questions/4364823/how-do-i-obtain-the-frequencies-of-each-value-in-an-fft
/* 0:   Index * Sample rate / Bin size = frequency of index Hz
  1:   1 * 44100 / 1024 =    43.1 Hz <- The first bin is 43.1 Hz - if we get the REAL number for this we get the amplitude of this frequency
  // The imaginary number is slightly more complex - see https://stackoverflow.com/questions/25624548/fft-real-imaginary-abs-parts-interpretation
  2:   2 * 44100 / 1024 =    86.1 Hz
  */