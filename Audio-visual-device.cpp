#include <math.h>
#include "pico/stdlib.h"
#include <stdio.h>

// include library
#include "Audio-visual-device.hpp"
#include "hardware/spi.h"

/// @brief Displays a bar
/// @param x position on x axis
/// @param height height of bar - maximum 3
/// @return last peak
int bar(int x, int height){
    int r = 0;
    int g = 0;
    int b = 0;
    int peak = 0;

    for (int i = 0; i < height; i++){
        if (i + 1 == height){
            peak = x;

            r = 255;
            g = 0;
        } else {
            g = 255;
            r = 0;
        }

        pico_rgb_keypad.illuminate(x, 3 - i, r, g, b);
    }

    return peak;
}


struct led_item {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} RGB;

// https://forum.arduino.cc/t/max7219-and-8x8led-matrix-all-leds-are-constantly-on/123887/2
// https://www.analog.com/media/en/technical-documentation/data-sheets/max7219-max7221.pdf
// https://www.dropbox.com/scl/fi/priug31ipifdky06i5uw9/libraries.rar?dl=0&e=1&file_subpath=%2FMD_MAX72XX%2Fsrc%2FMD_MAX72xx.cpp&rlkey=mebs7nk8my03sc72g9kvqzgg3
// https://youngkin.github.io/post/spidotmatrixmodule/#controlling-the-max7219---main-program

/// @brief Main function
/// @return 
int main (){
    max.init();
    /*const uint16_t test = 0b0000111100000000;
    max.write(&test);

    const uint16_t shutdown = 0b0000110000000001;
    max.write(&shutdown);

    const uint16_t decode = 0b0000100100000000;
    max.write(&decode);

    const uint16_t scan = 0b0000101100000000;
    max.write(&scan);

    const uint16_t num = 0b0000100100000100;
    max.write(&num);*/

    //const uint16_t num = 0b0000111100000001;
    //max.write(&num);

    //sleep_ms(1000);

    /*const uint16_t num2 = 0b0000111100000000;
    max.write(&num2);*/

    max.write_register_all(15, 1);
    sleep_ms(1000);
    max.write_register_all(15, 0);

    // initialise ADC
    adc_init();
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    // Select ADC input 0 (GPIO26)
    adc_select_input(0);

    while (true){ 
        // read voltage as percentage
        float v = adc_read() / 4096.0f; // 4096 == 1 << 12

        // fade
        int t [4] = { 255 };
        int peak [4] = { 0 };

        switch (calibrate_stage){
            case 0:
                // illuminate record button
                pico_rgb_keypad.illuminate(0, 255, 0, 0);
                
                // if the record button is pressed change stage
                if (pico_rgb_keypad.get_button_states() & (0b1 << 0)){
                    calibrate_stage = 1;
                }

                break;

            case 1:
                pico_rgb_keypad.clear();

                // update 
                pico_rgb_keypad.illuminate(0, 255 - s_captures, s_captures, 0);

                // if we've captured x amount of peaks      
                if (s_captures >= 255){
                    // calculate average silence using peaks
                    s = s / s_captures;

                    // clear visuals
                    pico_rgb_keypad.clear();

                    // change stage
                    calibrate_stage = 2;
                // if we haven't captured enough peaks and the button is released
                } else if (!(pico_rgb_keypad.get_button_states() & (0b1 << 0))) {
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
                // record samples
                sample[sample_i] = (v - s) * 4096;
                sample_i++;

                if (sample_i == 64){
                    pico_rgb_keypad.clear();
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

                    for (int i = 0; i < 4; i++){
                        int mean = 0;
                        int size = nfft / 4;

                        for (int j = 0; j < size; j++){
                            mean += cout[(size * i) + j].i;
                        }

                        mean = mean / size;
                        int height = (0.25f + round((mean / 4096))) * 4;

                        // define peak
                        /*if (t[i] <= 0){
                            t[i] = 255;
                            pico_rgb_keypad.illuminate(peak[i], 0, 0, 0);
                            
                        }*/

                        peak[i] = bar(i, height);
                    }

                    // release resources
                    kiss_fft_free(cfg_f);
                    delete[] cin;
                    delete[] cout;

                    // reset index
                    sample_i = 0;
                }

                // fade peak
                /*for (int i = 0; i < 4; i++){
                    t[i]--;
                    if (t[i] > 0) { pico_rgb_keypad.illuminate(peak[i], t[i], t[i], t[i]); }
                }*/

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

// LED Screen
// https://www.aliexpress.com/item/1005008005112441.html?spm=a2g0o.tesla.0.0.3dc5oZXgoZXgOa&pdp_npi=6%40dis%21USD%21%2413.92%21%240.99%21%21%21%21%21%40211b618e17572643030058606e76ce%2112000043231311504%21btf%21%21%21%211%210%21&afTraceInfo=1005008005112441__pc__c_ppc_item_bridge_pc_main__UgW8L9M__1757264303097
