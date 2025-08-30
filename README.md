# Audio-visual-Device
An audio-visual setup for the pimoroni RGB keypad

# Dependencies
- [Pimoroni RGB keypad](https://shop.pimoroni.com/products/pico-rgb-keypad-base?variant=32369517166675)
  - [C++ library](https://github.com/pimoroni/pimoroni-pico/tree/main/libraries/pico_rgb_keypad)
- [Zadig](https://zadig.akeo.ie/)
- [VSCode](https://code.visualstudio.com/)
  - [Raspberry pi pico extension](https://marketplace.visualstudio.com/items?itemName=raspberry-pi.raspberry-pi-pico)
- [KissFFT](https://github.com/mborgerding/kissfft)

# Build
1. Solder headers to the raspberry pi pico and attach to the pimoroni RGB keypad
2. Install VSCode and Raspberry pi pico extension
4. Hold down the BOOTSEL button and plug in the raspberry pi pico
5. Use Zadig to install drivers (Options/List All devices, RP2 BOOT INTERFACE 0, WINUSB)
6. Select Import project from the Raspberry pi pico extension tab, enable CMAKE-Tools and select this folder
7. Select Run Project (USB)
