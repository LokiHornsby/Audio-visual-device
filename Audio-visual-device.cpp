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
Delay between beats for onset detection 

detect start and end of beats

Frequency change based on oscillations 

Double fft then take half of the sample
Start Index from last position then negate from there

clear displays on each button press

optimise resetting rows and for loops
*/

void back(){
    selc[0] = 0; selc[1] = 255; selc[2] = 0;
    sel = prevsel;
    t = 0;
}

// FFT
bool performFFT(float v){
    // size of sample
    int size = nfft - 1;

    // if the index is less than the sample size
    if (ffti < size){
        // record sample
        sample[ffti] = v;

        // increment index
        ffti++;

        return false;

    // if the index is at the sample size
    } else if (ffti == size){
        // allocate input/output 1D arrays
        cin = new kiss_fft_cpx[nfft];
        cout = new kiss_fft_cpx[nfft];

        // initialize data storage
        memset(cin, 0, nfft * sizeof(kiss_fft_cpx));
        memset(cout, 0, nfft * sizeof(kiss_fft_cpx));

        // copy the input array to cin
        for (int i = 0; i < nfft; ++i){
            cin[i].r = sample[i];
        }

        // setup the size and type of FFT: forward
        bool is_inverse_fft = false;
        cfg_f = kiss_fft_alloc(nfft, is_inverse_fft, 0, 0); // typedef: struct kiss_fft_state*

        // execute transform for 1D
        kiss_fft(cfg_f, cin , cout);

        // increcement index out of bounds
        ffti++;

        return true;

    // if the index is out of bounds
    } else {
        // release resources
        kiss_fft_free(cfg_f);
        delete[] cin;
        delete[] cout;

        // reset index
        ffti = 0;

        return false;
    }
}

/// @brief Main function
/// @return 
int main (){
    // initialise all hardware
    stdio_init_all();
    keypad.init();
    microphone.init();
    display.init();

    selc[1] = 255;

    while (true){
        // button index
        ind++;

        if (ind > 15){
            ind = 0;
        }

        // if any button is pressed
        held = keypad.get_button_states() > 0;

        if (!held){
            buttonup = sel;
        }

        // if pressed button with index of "ind"
        if (keypad.get_button_states() & (0b1 << ind)) { 

            // if the current button doesn't equal the selected button 
            if (ind != sel && buttonup != ind){
                t = 0;

                // set the previous selection
                prevsel = sel;

                // clear display
                display.clear();

                // select current button
                sel = ind;
            }
        }

        // colors
        keypad.illuminate(ind, 255, 255, 255); 
        keypad.illuminate(prevsel, 255, 255, 0);
        keypad.illuminate(sel, selc[0], selc[1], selc[2]);
        keypad.update();

        // input
        float v = microphone.getVoltage();

        // switch to selected mode
        switch (sel) {
            // FFT
            case 0:
                display.clear();

                if (performFFT(v)){
                    // select row
                    for (int x = 0; x < display.HEIGHT; x++){
                        // select display
                        for (int d = 0; d < display.DISPLAYS; d++){
                            // construct a row of bits
                            for (int bit = 0; bit < display.WIDTH; bit++){
                                int ffti = (d * display.WIDTH) + (bit);

                                display.rows[0].set(
                                    (display.WIDTH - 1) - bit, // position
                                    cout[ffti].i > (microphone.getPeak() / display.WIDTH) * x // display bit? (if it's more than range)
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

                    // free resources
                    performFFT(v);
                }

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
                            display.rows[0].set(
                                (display.WIDTH - 1) - x, 
                                v > (microphone.getPeak() / (float)nfft) * (float)(x + (d * display.WIDTH))
                            );
                        }
        
                        display.write(y + 1, binarytoint(display.rows[0]), false);

                        display.rows[0].reset();
                    }
    
                    display.update();
                }

                break;

            // ONSET SCANNING 
            // ------------------------------------------------------------------- Average and peak??
            case 2: 
                o_v = ((v - silence) / microphone.getPeak()) * 8.0f;
                if (o_v > o_height) { o_height = o_v; }
                t++;

                if (t > o_max){
                    o_avg = (float)o_avg / (float)o_max;

                    barpos++;
                    
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
                    o_avg = 0;
                    o_height = 0;
                    t = 0;
                }

                break;

            // Instruments
            case 3:
                if (performFFT(v)){
                    if (cout[display.WIDTH / 8].i > microphone.getPeak() / 4){
                        for (int y = 0; y < display.HEIGHT; y++){
                            for (int x = 0; x < display.WIDTH; x++){
                                display.rows[(display.HEIGHT - 1) - y].set(x, B[display.WIDTH * y + x]);
                            }

                            for (int d = 0; d < display.DISPLAYS; d++){
                                display.write(y + 1, binarytoint(display.rows[y]), false);
                            }

                            display.update();
                        }  
                    }

                    // clear the display after a few seconds
                    sleep_ms(200);
                    display.clear();

                    // free resources
                    performFFT(v);
                }

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
                buttonup = 15;

                if (t < s_time){
                    if (held){
                        t++;
                        silence = silence + v;
                    } else {
                        silence = 0;
                        back();
                    }
                } else {
                    silence = (silence / s_time);
                    back();
                }

                int tv = ((float)t / (float)s_time) * 255.0f;
                selc[0] = 255 - tv; 
                selc[1] = tv; 

                display.clear();

                // build all rows
                for (int y = 0; y < display.HEIGHT; y++){
                    // build all displays
                    for (int d = 0; d < display.DISPLAYS; d++){
                        // build a row
                        for (int x = 0; x < display.WIDTH; x++){
                            display.rows[0].set(7 - x, v - silence > (microphone.getPeak() / nfft) * (x + (d * display.WIDTH)));
                        }
        
                        display.write(y + 1, binarytoint(display.rows[0]), false);

                        display.rows[0].reset();
                    }
    
                    display.update();
                }

                break;
        }
    }

    return 0;
}
