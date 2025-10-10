#include "Audio-visual-device.hpp"

#include <math.h>
#include <stdio.h>

/*std::bitset<8> addbinary(std::bitset<8> a, std::bitset<8> b){
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

    return a;
}*/

/*
Scanning bar graph onsets 

Delay between beats for onset detection 

detect start and end of beats

Frequency change based on oscillations 

Double fft then take half of the sample
Start Index from last position then negate from there
*/

/// @brief Main function
/// @return 
int main (){
    // -- INITIALISE
    stdio_init_all();
    keypad.init();
    microphone.init();
    display.init();

    while (true){
        // -- KEYPAD

        // button index
        ind++;

        if (ind > 15){
            ind = 0;
        }

        // if any button is pressed
        held = keypad.get_button_states() > 0;

        // if pressed button with index of "ind"
        if (keypad.get_button_states() & (0b1 << ind)) { 
            // if current button doesn't equal selected button
            if (ind != sel){

                // set the previous selection
                prevsel = sel;

                // select current button
                sel = ind;
            }
        }

        // colors
        keypad.illuminate(ind, 255, 255, 255); 
        keypad.illuminate(prevsel, 255, 255, 0);
        if (held) { keypad.illuminate(sel, 0, 255, 0); } else { keypad.illuminate(sel, 255, 0, 0); }
        keypad.update();

        // -- ANALYSIS

        // input
        float v = microphone.getVoltage() - silence;

        switch (sel) {
            // FFT
            case 0:
                /*// record samples
                sample[ffti] = v;
                ffti++;

                if (ffti == (nfft * 2) - 1){
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

                    display.clear();

                    // select row
                    for (int x = 0; x < 8; x++){
                        // select display
                        for (int d = 0; d < display.DISPLAYS; d++){
                            // construct a row of bits
                            for (int bit = 0; bit < 8; bit++){
                                int ffti = (d * 8) + (bit);

                                display.rows[0].set(
                                    7 - bit, // position
                                    cout[ffti].i > (microphone.getPeak() / 8) * x // display bit? (if it's more than range)
                                );
                            }

                            // write a row of bits (x) to display (d)
                            display.write(x+1, binarytoint(display.rows[0]), false);

                            // reset row
                            display.rows[0].reset();
                        }

                        // update each display with a row of bits
                        display.update();
                    }

                    // release resources
                    kiss_fft_free(cfg_f);
                    delete[] cin;
                    delete[] cout;

                    // reset index
                    ffti = 0;
                }*/

                break;

            // MAGNITUDE
            case 1:
                display.clear();

                // build all rows
                for (int y = 0; y < display.HEIGHT; y++){
                    // build all displays
                    for (int d = 0; d < display.DISPLAYS; d++){
                        // build a row
                        for (int x = 0; x < display.WIDTH; x++){
                            display.rows[0].set(7 - x, v > (microphone.getPeak() / nfft) * (x + (d * display.WIDTH)));
                        }
          
                        display.write(y + 1, binarytoint(display.rows[0]), false);

                        display.rows[0].reset();
                    }
    
                    display.update();
                }

                break;

            // ONSET SCANNING
            case 2:
                t++;

                if (t > 200){
                    barpos++;

                    o_height = (v / microphone.getPeak()) * 8.0f;
                    
                    if (barpos > 8){
                        dis++;

                        display.resetRows();

                        barpos = 0;
                    }

                    if (dis > display.DISPLAYS - 1){
                        //display.clear();
                        dis = 0;
                    }

                    for (int y = 0; y < display.HEIGHT; y++){
                        // set point
                        if (barpos < 8) { 
                            display.rows[y].set(barpos, y == o_store); 
                        }
                            
                        // build all displays
                        for (int d = 0; d < display.DISPLAYS; d++){
                            if (d == (display.DISPLAYS - 1) - dis){ // && y == o_height 
                                if (barpos < 7) { display.rows[y].set(barpos + 1, true); }
                                
                                display.write(y + 1, binarytoint(display.rows[y]), false);
                            } else {
                                display.write(0x00, 0, false);
                            }
                        }

                        display.update();
                    }

                    o_store = o_height;

                    t = 0;
                }

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

            // RECORD SILENCE
            case 15:
                if (held){
                    t++;     

                    // capture silence
                    if (t > s_time){
                        s += v;

                    // reset
                    } else {
                        silence = 0;
                    }
                } else {
                    t = 0;
                    if (t > s_time) { silence = s / (t - s_time); } else { s = 0; }
                    sel = prevsel; // select previous button
                }

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