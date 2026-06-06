# Shake POV Lightstick

ESP32-C3 based persistence-of-vision lightstick experiment. The lightstick uses a 50 LED strip and an MPU6500 motion sensor to render text while the stick is waved by hand.

This repository is a public, cleaned-up snapshot of the prototype research. It focuses on reproducible firmware, hardware notes, and a small browser-based tuning console.

## What it does

- Renders short text as POV columns on a single 50 pixel NeoPixel strip.
- Uses MPU6500 gyro data to detect swing direction and motion range.
- Drives the display from an ESP32-C3 on GPIO 4.
- Supports a Web Serial tuning page for text, speed, brightness, swing thresholds, direction, and preview commands.
- Keeps the implementation small enough to inspect and modify directly in Arduino IDE or PlatformIO-style workflows.

## Repository layout

```text
firmware/esp32c3_50_pov/
  Arduino firmware for the ESP32-C3 + 50 LED + MPU6500 prototype.

web/pov-controller/
  Browser tuning console using Web Serial.

docs/
  Hardware notes and BLE/control protocol notes from the broader lightstick work.
```

## Hardware target

- MCU: ESP32-C3
- LED strip: 50 x WS2812 / NeoPixel compatible LEDs
- LED data pin: GPIO 4
- Motion sensor: MPU6500 over I2C
- I2C SDA: GPIO 7
- I2C SCL: GPIO 6
- Default LED brightness: 135

See [docs/hardware-notes.md](docs/hardware-notes.md) for wiring and bring-up notes.

## Firmware dependencies

The Arduino sketch uses:

- `Adafruit_NeoPixel`
- `Wire`
- ESP32 Arduino core

Open `firmware/esp32c3_50_pov/esp32c3_50_pov.ino` in Arduino IDE, select an ESP32-C3 board profile, install the dependency above, and upload.

## Web tuning console

Open `web/pov-controller/pov.html` in a Chromium-based browser with Web Serial support.

The page can:

- connect to the board over serial
- send text columns
- tune thresholds and timing
- preview or clear the LED strip
- display serial telemetry from the firmware

## Status

Confirmed in the prototype:

- ESP32-C3 drives 50 LEDs on GPIO 4.
- MPU6500 motion data is read over I2C.
- Swing direction controls POV column progression.
- Text can be tuned through serial commands and a browser console.

Still experimental:

- Mechanical mounting and optical spacing need per-build calibration.
- Chinese character support currently uses fixed bitmap columns in firmware; the browser console ships with a compact ASCII 5x7 font path.
- Motion thresholds depend on battery weight, grip, and LED strip placement.

## Scope

This repository is only for the hand-waved POV lightstick prototype. It does not include private app dumps, APKs, raw captures, proprietary firmware images, or unrelated commercial lightstick protocol research.

## License

MIT License. See [LICENSE](LICENSE).
