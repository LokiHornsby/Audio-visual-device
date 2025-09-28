#pragma once

// hardware
#include "libraries/hardware/hardware.hpp"

// keypad
using namespace pimoroni;
PicoRGBKeypad keypad = PicoRGBKeypad();

// microphone
using namespace adafruit;
MAX981 microphone = MAX981();

// display
using namespace digishuo;
max7219 display = max7219();

// FFT
#include "libraries/kissfft/kiss_fft.h"

//// record sample
int sample [64] = { 0 };
int sample_i;

