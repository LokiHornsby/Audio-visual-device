#include <math.h>
#include "pico/stdlib.h"
#include <stdio.h>
#include "Audio-visual-device.hpp"
#include "hardware/spi.h"
#include <bits/stdc++.h>

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

//std::bitset<8>{0b10010000}, 
//std::bitset<8>{0b01010000}
////             0b11011000
int addbinary(std::bitset<8> a, std::bitset<8> b){
    int c = 0;

    for (int i = 0; i < 8; i++){
        int ab = (int)a.test(7 - i) + (int)b.test(7 - i); // LMB first
        
        if (ab > 1){
            c++;
        } 

        a.set(7 - i, ab + c > 0);

        if (ab == 0 && c > 0){
            c--;
        }
    }

    return static_cast<int>(a.to_ulong());
}

/// @brief Main function
/// @return 
int main (){
    // test
    for (int display = 0; display < max.DISPLAYS; display++){
        max.write(0x0F, 0); 
    }
    
    max.update();

    // shutdown mode
    for (int display = 0; display < max.DISPLAYS; display++){
        if (display == 0) {
            max.write(12, 1); 
        } else {
            max.write(12, 1);
        }
    }

    max.update();

    // intensity
    for (int display = 0; display < max.DISPLAYS; display++){
        max.write(10, 10);
        max.update();
    }

    max.update();

    // scan limit
    for (int display = 0; display < max.DISPLAYS; display++){
        max.write(0x0B, 7);
    }

    max.update();
    
    
    
    // write to 4th display
    /*max.write(0x00, 0b00000000); // line 1, display 1 (NO-OP)
    max.write(0x00, 0b00000000); // line 1, display 2 (NO-OP)
    max.write(0x00, 0b00000000); // line 1, display 3 (NO-OP)
    max.write(1, 0b00000000); // line 1, display 4
    max.update();*/

    // initialise ADC
    adc_init();
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    // Select ADC input 0 (GPIO26)
    adc_select_input(0);

    calibrate_stage = 2;

    while (true){ 
        // read voltage as percentage
        float v = adc_read() / 4096.0f; // 4096 == 1 << 12

        // fade
        int t [4] = { 255 };
        int peak [4] = { 0 };

        // binary
        uint8_t b;

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

                if (sample_i == 8){
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

                    // find max
                    int m;
                    for (int i = 0; i < nfft; i++){
                        if (cout[i].i > m) { m = cout[i].i; }
                    }
                    

                    // transformed: DC is stored in cout[0].r and cout[0].i

                    // calculate mean values for fft
                    /*for (int i = 0; i < 8; i++){
                        int mean = 0;
                        int size = nfft / 8;

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


                        //peak[i] = bar(i, height);*/
                    //}

                    // clear all LEDS
                    for (int x = 0; x < 8; x++){
                        max.write(x+1, 0b00000000);
                        max.write(x+1, 0b00000000);
                        max.write(x+1, 0b00000000);
                        max.write(x+1, 0b00000000);
                        max.update();
                    }

                    // display height
                    for (int x = 0; x < 8; x++){
                        // display fft
                        max.write(x+1, addbinary(
                        std::bitset<8>{0b10010000}, 
                        std::bitset<8>{0b01010000}
                        //             0b11011000
                        ));
                        
                        // display fft r
                        max.write(x+1, 0b00000000);

                        // display fft i
                        max.write(x+1, 0b00000000);

                        // display adc voltage
                        if (v > (1.0f / 8.0f) * (x + 1)) { max.write(x+1, 0b11111111); } else { max.write(0x00, 0b00000000); }
                        max.update();
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
