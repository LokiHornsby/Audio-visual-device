#include <string.h>
#include <stdint.h>

#include "hardware.hpp"

int binarytoint(std::bitset<MATRIX_WIDTH> a){
    return static_cast<int>(a.to_ulong());
}

namespace digishuo {
    void MAX7219::update(){
        gpio_put(pin::CS, 1);
    }

    void MAX7219::init(){
        // general init
        spi_init(spi1, 10 * 1000 * 1000);

        gpio_set_function(pin::CLK, GPIO_FUNC_SPI);
        gpio_set_function(pin::DIN, GPIO_FUNC_SPI);

        gpio_init(pin::CS);
        gpio_set_dir(pin::CS, GPIO_OUT);

        // test on
        write(0x0F, 1, true);

        // delay
        sleep_ms(1000);

        // test off
        write(0x0F, 0, true);

        // shutdown mode
        write(0x0C, 0b00000001, true);

        // intensity
        write(0X0A, 0b00000001, true);
        
        // scan limit
        write(0x0B, 7, true);

        // clear
        clear();
    }

    void MAX7219::write(uint8_t reg, uint8_t data, bool block) {
        // set write mode
        if (gpio_get(pin::CS) != 0) { gpio_put(pin::CS, 0); }

        // inputs
        uint8_t buf[2];
        buf[0] = reg;
        buf[1] = data;
        
        // write to all displays?
        if (block){
            for (int d = 0; d < MATRIX_DISPLAYS; d++){ spi_write_blocking(spi1, buf, 2); }
            update();
        } else {
            spi_write_blocking(spi1, buf, 2);
        }
    }

    void MAX7219::clear(){
        columns.reset();

        for (int y = 0; y < MATRIX_HEIGHT; y++){ // rows
            write(y+1, binarytoint(columns), true); // columns
        }
    }
}

namespace adafruit {
	void MAX981::init(){
		// initialise ADC
		adc_init();
		// Make sure GPIO is high-impedance, no pullups etc
		adc_gpio_init(26);
		// Select ADC input 0 (GPIO26)
		adc_select_input(0);
	}

	float MAX981::getVoltage(){
		return adc_read();
	}

	float MAX981::getPeak(){
		return 4096.0f;
	}
}

namespace pimoroni {
    // constructor
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