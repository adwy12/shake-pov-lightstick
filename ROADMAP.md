# Roadmap

This roadmap tracks the next practical improvements for the hand-waved POV lightstick prototype. The project is intentionally scoped to open firmware, hardware notes, and tuning tools for a self-built lightstick.

## Near term

### Text and bitmap generation

- Add a repeatable bitmap generation pipeline for short words and names.
- Document the column format used by the firmware.
- Support more non-ASCII glyphs through generated column arrays instead of hand-edited constants.
- Add examples for 5x7 ASCII text and taller 50-pixel custom text.

### Motion calibration

- Record a simple calibration procedure for different stick lengths and battery weights.
- Expose recommended ranges for swing threshold, rest threshold, column delay, and brightness.
- Add serial output examples that make it easier to diagnose reversed direction or over-sensitive motion detection.

### Web Serial tuning

- Save tuning values in browser local storage.
- Add import/export for a tuning profile.
- Add a copyable firmware constant block for values found during tuning.
- Improve mobile-size layout for tablets used at a workbench.

## Medium term

### Hardware documentation

- Add a simple wiring diagram.
- Add build photos or neutral diagrams for the ESP32-C3, MPU6500, and LED strip layout.
- Document safe power options for 50 LEDs at low and high brightness.
- Add enclosure and mounting notes once the mechanical layout stabilizes.

### Firmware maintainability

- Split the firmware into clearer modules for motion, rendering, serial commands, and LED output.
- Add a small host-side script that validates generated column arrays before they are pasted into firmware.
- Add GitHub Actions or documented local checks for compile verification.

### Examples

- Add example patterns for names, short slogans, and classroom demos.
- Add a minimal "hello" firmware variant for first-time builders.
- Add a troubleshooting guide for common problems: no LED output, reversed text, unstable motion, and noisy I2C.

## Out of scope

- Unauthorized control of third-party devices.
- Proprietary app dumps, APKs, firmware images, or raw captures.
- Venue control systems or commercial lightstick protocol cloning.
- Any credential, token, serial number, or private device identifier.

## How AI assistance can help

Useful maintenance tasks for AI coding tools include:

- reviewing firmware changes for timing regressions
- generating and checking bitmap column arrays
- improving documentation from test notes
- creating small validation scripts for column data and serial commands
- drafting issue triage summaries and release notes
