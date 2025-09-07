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
