# Desk Thing

A simple desk thing that I made for my desk.

## Features

- [x] Display the current time
- [ ] Display the current weather
- [x] Control the brightness of a Philips Wiz lightbulb
  - 
- [x] Timer
- [ ] Settings menu to configure WiFi, timezones, etc.

## Hardware

- [M5Dial](https://shop.m5stack.com/products/m5stack-dial-esp32-s3-smart-rotary-knob-w-1-28-round-touch-screen): A
  rotary knob with a touch screen powered by an ESP32-S3.
- 3D printed stand for the M5Dial

### Emulator

LVGL provides a nice abstraction for the M5Dial's hardware. This allows for the development of the software without
the need for the hardware. To use the emulator, use the `emulator` environment in PlatformIO.

```bash
pio run -e emulator -t execute
```

## How to use

- Using PlatformIO, open the project and upload it to the M5Dial.
- Configure the WiFi settings by scanning the QR code on the M5Dial.

## Configuration

You can customize the build-time configuration by copying `.env.example` to `.env` and modifying the values.


