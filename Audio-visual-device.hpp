// NEEDS UNUSED VARIABLES REMOVED

#pragma once
#include <bits/stdc++.h>

// HARDWARE
#include "libraries/hardware/hardware.hpp"

// KEYPAD
using namespace pimoroni;
PicoRGBKeypad keypad = PicoRGBKeypad();

int ind = 0;
int prevsel = 0;
int sel = 0;
int selc [3];
bool held = false;
int buttonup = 0;

// MICROPHONE
using namespace adafruit;
MAX981 microphone = MAX981();

// DISPLAY
using namespace digishuo;
MAX7219 display = MAX7219();

// ANALYSIS
int t = 0;

// FFT
// https://stackoverflow.com/questions/4364823/how-do-i-obtain-the-frequencies-of-each-value-in-an-fft
// FFT BIN X = X * (Sample rate / nfft (size of fft))
#include "libraries/kissfft/kiss_fft.h" 
bool enableFFT = false;
bool FFTdataused = true;
kiss_fft_cpx* cin;
kiss_fft_cpx* cout;
kiss_fft_cfg cfg_f;
const int nfft = (MATRIX_DISPLAYS * MATRIX_WIDTH);
float sample [nfft] = { 0 };
int ffti = 0;

// FFT DRAW
int bin8 [nfft] = { };

// ONSET SCANNING
int barpos = 0;
int dis = 0;
int o_height = 0;
int c = 0;
int o_store = 0;
int o_avg = 0;
int o_v = 0;
const int o_max = 100;

// Frequency detection
#define B { \
    0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 1, 1, 1, 0, 0, 0, \
    0, 0, 1, 0, 0, 1, 0, 0, \
    0, 0, 1, 1, 1, 0, 0, 0, \
    0, 0, 1, 0, 0, 1, 0, 0, \
    0, 0, 1, 0, 0, 1, 0, 0, \
    0, 0, 1, 1, 1, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0  \
}

#define V { \
    0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 1, 0, 0, 0, 1, 0, \
    0, 0, 1, 0, 0, 0, 1, 0, \
    0, 0, 0, 1, 0, 1, 0, 0, \
    0, 0, 0, 1, 0, 1, 0, 0, \
    0, 0, 0, 0, 1, 0, 0, 0, \
    0, 0, 0, 0, 1, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0  \
}

// Silence (frequencies to ignore as they are background noise)
#define S { \
    0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 1, 1, 1, 1, 1, 0, \
    0, 1, 0, 0, 0, 0, 0, 0, \
    0, 1, 0, 0, 0, 0, 0, 0, \
    0, 0, 1, 1, 1, 1, 0, 0, \
    0, 0, 0, 0, 0, 0, 1, 0, \
    0, 1, 1, 1, 1, 1, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0 \
}

// X (index) | B (sample rate = 44100) | C (nfft (fft size) = 128)
// Q (result) = (X * 44100 / 128)
#define r1 { 0, 1 } // BASS (Q1 | Q2 = 0Hz to 344.53125Hz) 
#define r2 { 1, 10 } // VOCALS (Q1 | Q2 = 344.53125Hz to 3445.3125Hz) 
#define r3 { 1, 2 }
int r [MATRIX_DISPLAYS][2] =                             { r1, r2, r3, r3, r3, r3, r3, r3, r3, r3, r3, r3, r3, r3, r3, r3 };
int ST [MATRIX_DISPLAYS][MATRIX_WIDTH * MATRIX_HEIGHT] = { B,  V,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S,  S  };
float sum = 0;

// RECORD SILENCE
const int s_time = 500;
float silence = 0;