# Hardware Notes

## Prototype bill of materials

- ESP32-C3 development board
- 50 LED WS2812 / NeoPixel compatible strip
- MPU6500 motion sensor module
- 5 V power source sized for the LED strip
- Level shifting or safe data wiring as required by the LED strip and board

## Wiring used by the current firmware

| Signal | ESP32-C3 pin |
| --- | --- |
| LED data | GPIO 4 |
| MPU6500 SDA | GPIO 7 |
| MPU6500 SCL | GPIO 6 |
| I2C speed | 400 kHz |

The firmware assumes a 50 pixel display. If the strip length changes, update `DISPLAY_LEDS` and `DRIVER_LEDS` in the sketch.

## Motion model

The prototype reads gyro data and treats left/right hand motion as the scan driver for the text columns. The useful values are build-dependent:

- `SWING_THRESHOLD`
- `REST_THRESHOLD`
- `MIN_SWEEP_UNITS`
- `MAX_SWEEP_UNITS`
- `DEG_PER_COLUMN_MICRO`

The browser tuning console is intended for quick iteration before baking stable values into the firmware.

## Safety notes

- Do not power a long LED strip directly from a weak USB port.
- Add a common ground between the ESP32-C3, LED strip, and sensor.
- Start at low brightness while testing battery and wire temperature.
- This prototype is for local hardware experimentation, not for venue control or unauthorized control of third-party devices.
