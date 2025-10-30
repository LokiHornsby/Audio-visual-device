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

allow rows to be stored for each display
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
                if (performFFT(v)){
                    display.clear();

                    // build the bin8 array (stores the height of each bin with a maximum value of MATRIX_HEIGHT)

                    // loop through each fft bin
                    for (int bin = 0; bin < nfft; bin++){
                        // calculate height of an FFT bin (max = MATRIX_HEIGHT)
                        for (int lvl = 0; lvl < MATRIX_HEIGHT; lvl++){
                            int l = (microphone.getPeak() / MATRIX_HEIGHT);
                            int l1 = l * lvl;
                            int l2 = l * (lvl + 1);

                            if (cout[bin].i >= l1 && cout[bin].i <= l2) { bin8[bin] = lvl; }
                        }
                    }

                    // draw each row and thus visually build the height of each bin using the bin8 array

                    // loop through each height
                    for (int y = 0; y < MATRIX_HEIGHT; y++){
                        // loop through each display
                        for (int d = 0; d < MATRIX_DISPLAYS; d++){
                            // loop through each column
                            display.columns.reset();
                            
                            for (int x = 0; x < MATRIX_WIDTH; x++){
                                display.columns.set((MATRIX_WIDTH - 1) - x, y <= bin8[(d * MATRIX_WIDTH) + x]);
                            }
                            
                            // draw columns in a single display
                            display.write(y+1, binarytoint(display.columns), false);
                        }

                        // update display
                        display.update();
                    }

                    // free resources
                    performFFT(v);
                }

                break;

            // MAGNITUDE
            case 1:
                /*
                display.clear();

                // build all rows
                for (int y = 0; y < MATRIX_HEIGHT; y++){
                    // build all displays
                    for (int d = 0; d < MATRIX_DISPLAYS; d++){
                        // build a row
                        for (int x = 0; x < MATRIX_WIDTH; x++){
                            display.rows[0].set(
                                (MATRIX_WIDTH - 1) - x, 
                                v > (microphone.getPeak() / (float)nfft) * (float)(x + (d * MATRIX_WIDTH))
                            );
                        }
        
                        display.write(y + 1, binarytoint(display.rows[0]), false);

                        display.rows[0].reset();
                    }
    
                    display.update();
                }*/

                break;

            // ONSET SCANNING 
            // ------------------------------------------------------------------- Average and peak??
            case 2: 
                /*
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

                    if (dis > MATRIX_DISPLAYS - 1){
                        //display.clear();
                        dis = 0;
                    }

                    for (int y = 0; y < MATRIX_HEIGHT; y++){
                        // set point
                        if (barpos < 8) { 
                            display.rows[y].set(barpos, y == o_store); 
                        }
                            
                        // build all displays
                        for (int d = 0; d < MATRIX_DISPLAYS; d++){
                            if (d == (MATRIX_DISPLAYS - 1) - dis){ // && y == o_height 
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
                }*/

                break;            

            // Instruments
            case 3: // this needs optimising
                /*if (performFFT(v)){
                    for (int y = 0; y < MATRIX_HEIGHT; y++){
                        for (int d = 0; d < MATRIX_DISPLAYS; d++){
                            for (int x = 0; x < MATRIX_WIDTH; x++){
                                // calculate fft sum of a range
                                for (int ri = r[d][0]; ri < r[d][1]; ri++){
                                    sum += cout[ri].i;
                                }
                                
                                // calculate value to pass to bitset array
                                bool val;

                                // if fft sum of a range is more than 0
                                if (sum > 0) {
                                    // set value to integer image (ST = STORE)
                                    val = ST[d][(MATRIX_WIDTH * y) + x];
                                } else {
                                    // set value to false
                                    val = false;
                                }

                                // set value in bitset array
                                display.rows[d][(MATRIX_HEIGHT - 1) - y].set(x, val);

                                // reset sum
                                sum = 0;
                            }

                            display.write(y + 1, binarytoint(display.rows[d][y]), false);
                        }

                        display.update();
                    }

                    // free resources
                    performFFT(v);
                }*/

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
                /*
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
                for (int y = 0; y < MATRIX_HEIGHT; y++){
                    // build all displays
                    for (int d = 0; d < MATRIX_DISPLAYS; d++){
                        // build a row
                        for (int x = 0; x < MATRIX_WIDTH; x++){
                            display.rows[0].set(7 - x, v - silence > (microphone.getPeak() / nfft) * (x + (d * MATRIX_WIDTH)));
                        }
        
                        display.write(y + 1, binarytoint(display.rows[0]), false);

                        display.rows[0].reset();
                    }
    
                    display.update();
                }*/

                break;
        }
    }

    return 0;
}
