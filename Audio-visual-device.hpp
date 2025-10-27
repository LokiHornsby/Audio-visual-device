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
#include "libraries/kissfft/kiss_fft.h" 
bool enableFFT = false;
bool FFTdataused = true;
kiss_fft_cpx* cin;
kiss_fft_cpx* cout;
kiss_fft_cfg cfg_f;
const int nfft = (MAX7219::DISPLAYS * MAX7219::WIDTH);
float sample [nfft] = { 0 };
int ffti = 0;

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
float sum = 0;

#define r1 { 0, display.WIDTH * 7 }
#define r2 { display.WIDTH * 7, display.WIDTH * 8 }
#define r3 { display.WIDTH * 8, display.WIDTH * display.DISPLAYS }
int r [display.DISPLAYS][2] = { r1, r2, r1, r1, r1, r1, r1, r1, r1, r1, r1, r3 };

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
    0, 0, 0, 0, 0, 0, 0, 0  \
    0, 0, 1, 1, 1, 1, 1, 0, \
    0, 1, 0, 0, 0, 0, 0, 0, \
    0, 1, 0, 0, 0, 0, 0, 0, \
    0, 0, 1, 1, 1, 1, 0, 0, \
    0, 0, 0, 0, 0, 0, 1, 0, \
    0, 1, 1, 1, 1, 1, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, \
}

int ST [display.DISPLAYS][display.WIDTH * display.HEIGHT] = { B, V, V, V, V, V, V, V, V, V, V, B };

// RECORD SILENCE
const int s_time = 500;
float silence = 0;