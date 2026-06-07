# Innovation Optimizations

This project is evolving from a fixed POV demo into a small workflow for repeatable, hand-waved text rendering.

## 1. Motion-aware scan model

The firmware uses gyro direction changes to advance text columns. This is more robust than a fixed timer because the displayed text follows the user's swing instead of assuming a constant hand speed.

Current design:

- detect swing direction from gyro deltas
- reverse scan direction when motion crosses a threshold
- clear the display when the stick is still
- adapt column timing between slow and fast swing ranges

## 2. Browser-side tuning loop

The Web Serial console now persists its local profile in browser storage. Builders can tune thresholds, brightness, direction, range learning, and text speed without losing settings on refresh.

This reduces the firmware reflash loop:

1. upload firmware once
2. tune parameters in the browser
3. preview on the strip
4. keep or copy stable values into firmware later

## 3. Repeatable text-to-column generation

The `tools/text_to_columns.js` script converts ASCII text into firmware-ready column arrays:

```bash
node tools/text_to_columns.js "HELLO"
```

This makes the project easier to review because generated bitmap data can be reproduced from text instead of manually edited.

## 4. Open research path

The next useful improvements are:

- taller generated glyphs for 50-pixel POV output
- calibration logs that recommend threshold ranges
- profile import/export from the browser UI
- a small validation step that rejects too-wide or empty glyph output
