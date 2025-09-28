#include <math.h>
#include "pico/stdlib.h"
#include <stdio.h>
#include "Audio-visual-device.hpp"
#include "hardware/spi.h"
#include <bits/stdc++.h>

int binarytoint(std::bitset<8> a){
    return static_cast<int>(a.to_ulong());
}

/// @brief Main function
/// @return 
int main (){
    // initialise
    stdio_init_all();
    keypad.init();
    microphone.init();
    display.init();

    // stage
    int stage = 0;

    // index
    int ind;

    // selected
    int sel;

    while (true){
        // button index
        ind++;

        if (ind > 15){
            stage = sel;
            ind = 0;
        }

        // pressed button
        if (keypad.get_button_states() & (0b1 << ind)) { sel = ind; }

        // colors
        keypad.illuminate(ind, 255, 255, 255); 
        keypad.illuminate(stage, 0, 255, 0);
        keypad.update();

        // input
        float v = microphone.getVoltage();

        // column
        std::bitset<8> b { 0b00000000 };

        switch (stage) {
            // FFT
            case 0:
                // record samples
                sample[sample_i] = v;
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

                    // find max
                    int m;
                    for (int i = 0; i < nfft; i++){
                        if (cout[i].i > m) { m = cout[i].i; }
                    }

                    // clear all LEDS
                    display.clear();

                    // display FFT
                    for (int x = 0; x < 8; x++){
                        for (int j = 0; j < display.DISPLAYS; j++){
                            for (int y = 0; y < 8; y++){
                                b.set(7 - y, cout[(y + (j * 8))].i > (4096 / 8) * x);
                            }
                            
                            display.write(x+1, binarytoint(b));

                            b.reset();
                        }

                        display.update();
                    }

                    // release resources
                    kiss_fft_free(cfg_f);
                    delete[] cin;
                    delete[] cout;

                    // reset index
                    sample_i = 0;
                }

                break;

            case 1:
                /*for (int i = 0; i < 8; i++){
                    for (int j = 0; j < 4; i++){
                        if (v > (1.0f / 8.0f) * (i + 1)) { bin = 0b11111111; }
                        max.write(i+1, bin);
                        bin = 0b00000000;
                    }

                    max.update();
                }*/

                display.clear();
                
                for (int i = 0; i < display.DISPLAYS; i++){
                    display.write(2, 0b11111111);
                }

                display.update();

                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                break;
            case 8:
                break;
            case 9:
                break;
            case 10:
                break;
            case 11:
                break;
            case 12:
                break;
            case 13:
                break;
            case 14:
                break;
            case 15:
                break;
        }
    }

    return 0;
}

/* while (true){ 
        

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
                
                }

                // fade peak
                /*for (int i = 0; i < 4; i++){
                    t[i]--;
                    if (t[i] > 0) { pico_rgb_keypad.illuminate(peak[i], t[i], t[i], t[i]); }
                }

                break;
            
            default:
                break;*/
        //}

        //pico_rgb_keypad.update();
    //}