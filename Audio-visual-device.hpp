#pragma once

// hardware
#include "libraries/hardware/hardware.hpp"

// keypad
using namespace pimoroni;
PicoRGBKeypad keypad = PicoRGBKeypad();

int ind = 0;
int prevsel = 0;
int sel = 0;
bool held = false;

// microphone
using namespace adafruit;
MAX981 microphone = MAX981();

// display
using namespace digishuo;
MAX7219 display = MAX7219();

// ANALYSIS
int t = 0;

// FFT
#include "libraries/kissfft/kiss_fft.h" 
const int nfft = MAX7219::DISPLAYS * MAX7219::WIDTH;
float sample [nfft] = { 0 };
int ffti = 0;

// ONSET SCANNING
int barpos = 0;
int dis = 0;
int o_height = 0;
int c = 0;
int o_store = 0;

// RECORD SILENCE
int s_time = 50;
int silence = 0;
int s = 0;