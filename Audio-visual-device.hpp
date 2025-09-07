#pragma once

// keypad
#include "picolib/pico_rgb_keypad.hpp"
using namespace pimoroni;
PicoRGBKeypad pico_rgb_keypad;

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
#include "kissfft/kiss_fft.h"

//// record sample
int sample [64] = { 0 };
int sample_i;