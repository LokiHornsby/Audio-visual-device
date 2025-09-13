#include <string.h>
#include <math.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"

#include "hardware.hpp"
#include <cstdint>
#include <bitset>

// pins
enum pin {
  SDA       =  4,
  SCL       =  5,
  CS        = 17,
  SCK       = 18,
  MOSI      = 19
};

int DATAGPIO = 22;

// https://www.arrow.com/en/research-and-events/articles/protocol-for-the-ws2812b-programmable-led
// https://tomverbeure.github.io/2019/10/13/WS2812B_Reset_Old_and_New.html
void WS2812B_sendtiming(int z){
  switch (z){
        case 0:
            gpio_put(DATAGPIO, 1);
            sleep_us(0.38);
            gpio_put(DATAGPIO, 0);
            sleep_us(1);
            break;
        case 1:
            gpio_put(DATAGPIO, 1);
            sleep_us(1);
            gpio_put(DATAGPIO, 0);
            sleep_us(0.42);
            break;
    }
}

void WS2812B_reset(){
    gpio_put(DATAGPIO, 0);
    sleep_us(300);
}

void WS2812B_init(){
    // init
    gpio_init(DATAGPIO);
    gpio_set_dir(DATAGPIO, true);

    // reset all LEDs
    for (int i = 0; i < 256 * 3; i++){
        WS2812B_sendtiming(0);
        WS2812B_sendtiming(0);
        WS2812B_sendtiming(0);
        WS2812B_sendtiming(0);
        WS2812B_sendtiming(0);
        WS2812B_sendtiming(0);
        WS2812B_sendtiming(0);
        WS2812B_sendtiming(1); 
    }

    // reset to start
    gpio_put(DATAGPIO, 0);
    sleep_us(300);

    sleep_ms(1000);

    ////////////////////// led
    
    WS2812B_sendtiming(0);
    WS2812B_sendtiming(0);
    WS2812B_sendtiming(0);
    WS2812B_sendtiming(0);
    WS2812B_sendtiming(0);
    WS2812B_sendtiming(0);
    WS2812B_sendtiming(0);
    WS2812B_sendtiming(1);

    WS2812B_sendtiming(1);
    WS2812B_sendtiming(1);
    WS2812B_sendtiming(1);
    WS2812B_sendtiming(1);
    WS2812B_sendtiming(1);
    WS2812B_sendtiming(1);
    WS2812B_sendtiming(1);
    WS2812B_sendtiming(1);

    WS2812B_sendtiming(0);
    WS2812B_sendtiming(0);
    WS2812B_sendtiming(0);
    WS2812B_sendtiming(0);
    WS2812B_sendtiming(0);
    WS2812B_sendtiming(0);
    WS2812B_sendtiming(0);
    WS2812B_sendtiming(1);
}

void WS2812B_setpixel(int i, uint8_t r, uint8_t g, uint8_t b){
    // skip to index
    for (size_t x = 0; x < i - 1; x++){
      for (int y = 0; y < 3; y++){
        for (int j = 0; j < 8; j++){
          WS2812B_sendtiming(0);
        }

        sleep_ms(50);
      }
    }
    

    // red
    for (int j = 0; j < i - 1; j++){
      WS2812B_sendtiming((r >> (j-1)) & 1);
    }

    sleep_ms(50);

    // green
    for (int j = 0; j < i - 1; j++){
      WS2812B_sendtiming((g >> (j-1)) & 1);
    }
    
    sleep_ms(50);

    // blue
    for (int j = 0; j < i - 1; j++){
      WS2812B_sendtiming((b >> (j-1)) & 1);
    }
  
    sleep_ms(50);
    
    // reset
    sleep_ms(500);
    gpio_put(DATAGPIO, 0);
    sleep_ms(500);
}

namespace pimoroni {

    // constructor
    PicoRGBKeypad::~PicoRGBKeypad() {
      clear();
      update();
    } 

    // initialise rgb keypad
    void PicoRGBKeypad::init() {
      memset(buffer, 0, sizeof(buffer));
      led_data = buffer + 4;

      set_brightness(1); //Must be called to init each LED frame

      // setup i2c interface
      i2c_init(i2c0, 400000);
      gpio_set_function(pin::SDA, GPIO_FUNC_I2C); gpio_pull_up(pin::SDA);
      gpio_set_function(pin::SCL, GPIO_FUNC_I2C); gpio_pull_up(pin::SCL);

      spi_init(spi0, 4 * 1024 * 1024);
      gpio_set_function(pin::CS, GPIO_FUNC_SIO);
      gpio_set_dir(pin::CS, GPIO_OUT);
      gpio_put(pin::CS, 1);
      gpio_set_function(pin::SCK, GPIO_FUNC_SPI);
      gpio_set_function(pin::MOSI, GPIO_FUNC_SPI);

      update();
    }

    void PicoRGBKeypad::update() {
      gpio_put(pin::CS, 0);
      spi_write_blocking(spi0, buffer, sizeof(buffer));
      gpio_put(pin::CS, 1);
    }

    void PicoRGBKeypad::set_brightness(float brightness) {
      if(brightness < 0.0f || brightness > 1.0f) {
        return;
      }

      for(uint16_t i = 0; i < NUM_PADS; i++)
          led_data[i * 4] = 0b11100000 | (uint8_t)(brightness * (float)0b11111);
    }

    void PicoRGBKeypad::illuminate(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
      if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
        return;
      }

      uint16_t offset = (x + (y * WIDTH)) * 4;
      //led_data[offset + 0] = 0xff;  //Not needed as set at init
      led_data[offset + 1] = b;
      led_data[offset + 2] = g;
      led_data[offset + 3] = r;
    }

    void PicoRGBKeypad::illuminate(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
      if(i < 0 || i >= NUM_PADS) {
        return;
      }

      uint16_t offset = i * 4;
      //led_data[offset + 0] = 0xff;  //Not needed as set at init
      led_data[offset + 1] = b;
      led_data[offset + 2] = g;
      led_data[offset + 3] = r;
    }

    void PicoRGBKeypad::clear() {
      for(uint16_t i = 0; i < NUM_PADS; i++)
        illuminate(i, 0, 0, 0);
    }

    uint16_t PicoRGBKeypad::get_button_states() {
      uint8_t i2c_read_buffer[2];
      uint8_t reg = 0;
      i2c_write_blocking(i2c0, KEYPAD_ADDRESS, &reg, 1, true);
      i2c_read_blocking(i2c0, KEYPAD_ADDRESS, i2c_read_buffer, 2, false);
      return ~((i2c_read_buffer[0]) | (i2c_read_buffer[1] << 8));
    }

}