#pragma once

// hardware
#include "libraries/hardware/hardware.hpp"

// keypad
using namespace pimoroni;
PicoRGBKeypad pico_rgb_keypad;

// max7219
using namespace digishuo;
max7219 max;

// microphone
#include "hardware/adc.h"
int calibrate_stage = 0;

//// Calculate silence
float s = 0.0;
float s_peak;
const int s_frames = 64;
int s_fi;
int s_captures;

// FFT
#include "libraries/kissfft/kiss_fft.h"

//// record sample
int sample [64] = { 0 };
int sample_i;
