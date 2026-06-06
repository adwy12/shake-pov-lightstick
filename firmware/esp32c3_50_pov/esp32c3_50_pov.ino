#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>

static constexpr uint8_t LED_PIN = 4;
static constexpr uint16_t DISPLAY_LEDS = 50;
static constexpr uint16_t DRIVER_LEDS = 50;
static constexpr uint8_t BRIGHTNESS = 135;
static constexpr uint16_t DEFAULT_COLUMN_US = 1200;
static constexpr uint16_t FRAME_GAP_US = 0;
static constexpr uint16_t CHAR_SWITCH_MS = 100;
static constexpr uint8_t CAMERA_FLOOR = 3;
static constexpr uint8_t VERTICAL_SHIFT_UP = 6;
static constexpr uint64_t DISPLAY_MASK = (1ULL << DISPLAY_LEDS) - 1ULL;
static constexpr uint8_t SDA_PIN = 7;
static constexpr uint8_t SCL_PIN = 6;
static constexpr uint32_t I2C_FREQ = 400000;
static constexpr uint16_t SWING_THRESHOLD = 25;
static constexpr uint16_t REST_THRESHOLD = 70;
static constexpr uint16_t STILL_CLEAR_MS = 120;
static constexpr uint32_t MIN_SWEEP_UNITS = 70000;
static constexpr uint32_t MAX_SWEEP_UNITS = 520000;
static constexpr uint32_t DEG_PER_COLUMN_MICRO = 600000;
static constexpr uint16_t GYRO_LSB_PER_DPS = 131;

Adafruit_NeoPixel strip(DRIVER_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

static const uint64_t MESSAGE[] = {
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x003FFFFFFFF00ULL,
  0x003FFFFFFFF00ULL,
  0x003FFFFFFFF00ULL,
  0x003FFFFFFFF00ULL,
  0x003FFFFFFFF00ULL,
  0x0000000000F00ULL,
  0x0000000000F00ULL,
  0x0000000000F00ULL,
  0x0000000000F00ULL,
  0x0000000000F00ULL,
  0x0000000000F00ULL,
  0x0000000000F00ULL,
  0x0000000000F00ULL,
  0x0000000000F00ULL,
  0x0000000000F00ULL,
  0x0000000000F00ULL,
  0x0000000000F00ULL,
  0x0000000000F00ULL,
  0x0000000000F00ULL,
  0x0000000000F00ULL,
  0x0000000000F00ULL,
  0x0000000000F00ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x003FFFFFFFF00ULL,
  0x003FFFFFFFF00ULL,
  0x003FFFFFFFF00ULL,
  0x003FFFFFFFF00ULL,
  0x003C003C00000ULL,
  0x003C003C00000ULL,
  0x003C003C00000ULL,
  0x003C003C00000ULL,
  0x003C003C00000ULL,
  0x003C003C00000ULL,
  0x003C003C00000ULL,
  0x003C003C00000ULL,
  0x003C003C00000ULL,
  0x003C003C00000ULL,
  0x003C003C00000ULL,
  0x003C003C00000ULL,
  0x003C007C00000ULL,
  0x003E007C00000ULL,
  0x003E007C00000ULL,
  0x001F00F800000ULL,
  0x001F83F800000ULL,
  0x001FFFF000000ULL,
  0x000FFFF000000ULL,
  0x0007FFE000000ULL,
  0x0003FF8000000ULL,
  0x0000FE0000000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x00007F8000000ULL,
  0x0003FFE01E000ULL,
  0x0007FFF01F800ULL,
  0x000FFFF81FC00ULL,
  0x001FFFFC1FC00ULL,
  0x003F807E07E00ULL,
  0x003E003E03F00ULL,
  0x007E001F01F00ULL,
  0x007C001F00F00ULL,
  0x0078000F00F00ULL,
  0x0078000F00F00ULL,
  0x0078000F00F00ULL,
  0x0078000F00F00ULL,
  0x007C001F01F00ULL,
  0x007C001E03E00ULL,
  0x003E003E03E00ULL,
  0x003F007C0FC00ULL,
  0x001FC1F87FC00ULL,
  0x000FFFFFFF800ULL,
  0x0007FFFFFF000ULL,
  0x0003FFFFFC000ULL,
  0x0000FFFFF0000ULL,
  0x000007FE00000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x00007F8000000ULL,
  0x0003FFE01E000ULL,
  0x0007FFF01F800ULL,
  0x000FFFF81FC00ULL,
  0x001FFFFC1FC00ULL,
  0x003F807E07E00ULL,
  0x003E003E03F00ULL,
  0x007E001F01F00ULL,
  0x007C001F00F00ULL,
  0x0078000F00F00ULL,
  0x0078000F00F00ULL,
  0x0078000F00F00ULL,
  0x0078000F00F00ULL,
  0x007C001F01F00ULL,
  0x007C001E03E00ULL,
  0x003E003E03E00ULL,
  0x003F007C0FC00ULL,
  0x001FC1F87FC00ULL,
  0x000FFFFFFF800ULL,
  0x0007FFFFFF000ULL,
  0x0003FFFFFC000ULL,
  0x0000FFFFF0000ULL,
  0x000007FE00000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL
};

static const uint64_t CHAR_LEI[] = {
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x0007F80000000ULL,
  0x0007F80000000ULL,
  0x0007F80000000ULL,
  0x0007F80000080ULL,
  0x00E7000FFFF80ULL,
  0x00E7000FFFF80ULL,
  0x00E731CFFFF80ULL,
  0x00E731CE30C00ULL,
  0x00E731CE30C00ULL,
  0x00E731CE30C00ULL,
  0x00E731CE30C00ULL,
  0x00E731CE30C00ULL,
  0x00E731CE30C00ULL,
  0x00E7000E30C00ULL,
  0x00E7000E30C00ULL,
  0x00E7000E30C00ULL,
  0x00FFFFCFFFC00ULL,
  0x00FFFFCFFFC00ULL,
  0x00FFFFCFFFC00ULL,
  0x00E7004E30C00ULL,
  0x00E7000E30C00ULL,
  0x00E7000E30C00ULL,
  0x00E731CE30C00ULL,
  0x00E731CE30C00ULL,
  0x00E731CE30C00ULL,
  0x00E731CE30C00ULL,
  0x00E731CE30C00ULL,
  0x00E731CE30C00ULL,
  0x00E731CFFFF00ULL,
  0x00E731CFFFF00ULL,
  0x00E7000FFFF00ULL,
  0x00E7000FFFF00ULL,
  0x0007F00000000ULL,
  0x0007F00000000ULL,
  0x0007F00000000ULL,
  0x0007F00000000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL
};

static const uint64_t CHAR_PENG[] = {
  0x0000000000000ULL,
  0x0000000000000ULL,
  0x0000000000400ULL,
  0x0000000001C00ULL,
  0x000000000FE00ULL,
  0x007FFFFFFFE00ULL,
  0x007FFFFFFF800ULL,
  0x007FFFFFFE000ULL,
  0x007FFFFFE0000ULL,
  0x0070380E00000ULL,
  0x0070380E02000ULL,
  0x0070380E03800ULL,
  0x0070380E03C00ULL,
  0x0070380E03C00ULL,
  0x0070380E03C00ULL,
  0x007FFFFFFFC00ULL,
  0x007FFFFFFF800ULL,
  0x007FFFFFFF000ULL,
  0x007FFFFFFC600ULL,
  0x0000000001E00ULL,
  0x0000000007F00ULL,
  0x000000007FE00ULL,
  0x007FFFFFFFC00ULL,
  0x007FFFFFFF000ULL,
  0x007FFFFFF8000ULL,
  0x007FFFFE00000ULL,
  0x00701C0E00000ULL,
  0x00701C0E00000ULL,
  0x00701C0E00000ULL,
  0x00701C0E00C00ULL,
  0x00701C0E00F00ULL,
  0x00701C0E00F00ULL,
  0x00701C0E00F00ULL,
  0x00701C0E01F00ULL,
  0x007FFFFFFFE00ULL,
  0x007FFFFFFFE00ULL,
  0x007FFFFFFFC00ULL,
  0x007FFFFFFF000ULL,
  0x0000000000000ULL,
  0x0000000000000ULL
};

static constexpr uint16_t MESSAGE_COLS = sizeof(MESSAGE) / sizeof(MESSAGE[0]);
static constexpr uint8_t CHAR_COUNT = 2;
static const uint64_t *const CHAR_FRAMES[CHAR_COUNT] = {CHAR_LEI, CHAR_PENG};
static constexpr uint16_t CHAR_COLS[CHAR_COUNT] = {
  sizeof(CHAR_LEI) / sizeof(CHAR_LEI[0]),
  sizeof(CHAR_PENG) / sizeof(CHAR_PENG[0])
};

uint16_t col = 0;
uint16_t columnUs = DEFAULT_COLUMN_US;
uint32_t nextColumnUs = 0;
uint8_t currentChar = 0;
uint32_t nextCharMs = 0;
uint8_t mpuAddress = 0;
int16_t gyroBiasX = 0;
int16_t gyroBiasY = 0;
int16_t gyroBiasZ = 0;
uint32_t strokeUnits = 0;
uint32_t learnedSweepUnits = 150000;
uint32_t angleAccumMicroDeg = 0;
uint32_t lastSampleUs = 0;
uint32_t lastMotionMs = 0;
uint32_t lastLogMs = 0;
uint32_t lastMotionReadUs = 0;
uint16_t currentColumn = 0xffff;
int8_t lastDirection = 0;
int8_t scanDirection = 1;
int32_t xMotionAccum = 0;
uint32_t lastReverseMs = 0;
bool strokeActive = false;
String serialLine;
uint8_t textR = 170;
uint8_t textG = 0;
uint8_t textB = 255;

void showSolidColor() {
  uint32_t color = strip.Color(textR, textG, textB);
  for (uint16_t i = 0; i < DRIVER_LEDS; i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

uint32_t wheel(uint8_t pos) {
  pos = 255 - pos;
  if (pos < 85) {
    return strip.Color(255 - pos * 3, 0, pos * 3);
  }
  if (pos < 170) {
    pos -= 85;
    return strip.Color(0, pos * 3, 255 - pos * 3);
  }
  pos -= 170;
  return strip.Color(pos * 3, 255 - pos * 3, 0);
}

bool readReg(uint8_t address, uint8_t reg, uint8_t &value) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) {
    return false;
  }
  if (Wire.requestFrom(address, static_cast<uint8_t>(1)) != 1) {
    return false;
  }
  value = Wire.read();
  return true;
}

bool writeReg(uint8_t address, uint8_t reg, uint8_t value) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}

int16_t readI16(uint8_t *bytes, uint8_t offset) {
  return static_cast<int16_t>((bytes[offset] << 8) | bytes[offset + 1]);
}

bool readMotion(int16_t &gx, int16_t &gy, int16_t &gz) {
  if (mpuAddress == 0) {
    return false;
  }
  Wire.beginTransmission(mpuAddress);
  Wire.write(0x3B);
  if (Wire.endTransmission(false) != 0) {
    return false;
  }
  if (Wire.requestFrom(mpuAddress, static_cast<uint8_t>(14)) != 14) {
    return false;
  }

  uint8_t bytes[14];
  for (uint8_t i = 0; i < 14; i++) {
    bytes[i] = Wire.read();
  }
  gx = readI16(bytes, 8);
  gy = readI16(bytes, 10);
  gz = readI16(bytes, 12);
  return true;
}

void setupMpu() {
  for (uint8_t address : {static_cast<uint8_t>(0x68), static_cast<uint8_t>(0x69)}) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      mpuAddress = address;
      break;
    }
  }
  if (mpuAddress == 0) {
    Serial.println("MPU not found");
    return;
  }

  writeReg(mpuAddress, 0x6B, 0x00);
  delay(120);
  writeReg(mpuAddress, 0x1C, 0x00);
  writeReg(mpuAddress, 0x1B, 0x00);

  uint8_t who = 0;
  readReg(mpuAddress, 0x75, who);
  Serial.printf("MPU address=0x%02X WHO_AM_I=0x%02X\n", mpuAddress, who);

  int32_t sumX = 0;
  int32_t sumY = 0;
  int32_t sumZ = 0;
  uint16_t samples = 0;
  for (uint8_t i = 0; i < 80; i++) {
    int16_t gx = 0;
    int16_t gy = 0;
    int16_t gz = 0;
    if (readMotion(gx, gy, gz)) {
      sumX += gx;
      sumY += gy;
      sumZ += gz;
      samples++;
    }
    delay(4);
  }
  if (samples > 0) {
    gyroBiasX = sumX / samples;
    gyroBiasY = sumY / samples;
    gyroBiasZ = sumZ / samples;
  }
  Serial.printf("Gyro bias x=%d y=%d z=%d\n", gyroBiasX, gyroBiasY, gyroBiasZ);
}

int32_t dominantSwing(int16_t gx, int16_t gy, int16_t gz, char &axis) {
  int32_t sx = static_cast<int32_t>(gx) - gyroBiasX;
  int32_t sy = static_cast<int32_t>(gy) - gyroBiasY;
  (void)gz;

  axis = 'X';
  int32_t swing = sx;
  if (labs(sy) > labs(sx)) {
    axis = 'Y';
    swing = sy;
  }
  return swing;
}

void learnSweep(uint32_t units) {
  if (units < MIN_SWEEP_UNITS) {
    return;
  }
  uint32_t target = constrain(units, MIN_SWEEP_UNITS, MAX_SWEEP_UNITS);
  learnedSweepUnits = (learnedSweepUnits * 7 + target * 3) / 10;
}

void startStroke(int8_t direction) {
  strokeActive = true;
  strokeUnits = 0;
  angleAccumMicroDeg = 0;
  col = 0;
  lastDirection = direction;
  currentColumn = 0xffff;
}

void applyColumnFrequency(uint16_t hz) {
  hz = constrain(hz, static_cast<uint16_t>(120), static_cast<uint16_t>(3000));
  columnUs = constrain(static_cast<uint32_t>(1000000UL / hz), 250UL, 8000UL);
  Serial.printf("CFG freq=%u columnUs=%u\n", hz, columnUs);
}

void applyTextColor(uint8_t r, uint8_t g, uint8_t b) {
  textR = r;
  textG = g;
  textB = b;
  if (!strokeActive) {
    showSolidColor();
  }
  Serial.printf("CFG color=#%02X%02X%02X\n", textR, textG, textB);
}

void handleCommand(String line) {
  line.trim();
  if (line.length() == 0) {
    return;
  }
  line.toUpperCase();
  if (line.startsWith("FREQ=")) {
    applyColumnFrequency(line.substring(5).toInt());
    return;
  }
  if (line.startsWith("FREQ,")) {
    applyColumnFrequency(line.substring(5).toInt());
    return;
  }
  if (line.startsWith("US=")) {
    columnUs = constrain(line.substring(3).toInt(), 250, 8000);
    Serial.printf("CFG columnUs=%u freq=%lu\n", columnUs, 1000000UL / max<uint16_t>(columnUs, 1));
    return;
  }
  if (line.startsWith("COLOR=")) {
    String raw = line.substring(6);
    raw.trim();
    if (raw.startsWith("#")) {
      raw = raw.substring(1);
    }
    if (raw.length() >= 6) {
      uint32_t rgb = strtoul(raw.substring(0, 6).c_str(), nullptr, 16);
      applyTextColor((rgb >> 16) & 0xff, (rgb >> 8) & 0xff, rgb & 0xff);
    }
    return;
  }
  if (line == "STATUS") {
    Serial.printf("STATUS columnUs=%u freq=%lu color=#%02X%02X%02X active=%u col=%u\n",
                  columnUs, 1000000UL / max<uint16_t>(columnUs, 1),
                  textR, textG, textB,
                  strokeActive ? 1 : 0, currentColumn == 0xffff ? 999 : currentColumn);
  }
}

void handleSerial() {
  while (Serial.available()) {
    char c = static_cast<char>(Serial.read());
    if (c == '\n' || c == '\r') {
      handleCommand(serialLine);
      serialLine = "";
    } else if (serialLine.length() < 80) {
      serialLine += c;
    }
  }
}

uint16_t columnForStroke() {
  uint16_t index = min<uint16_t>(col, MESSAGE_COLS - 1);
  return lastDirection > 0 ? index : (MESSAGE_COLS - 1 - index);
}

void advanceAngleColumns(uint32_t deltaMicroDeg) {
  angleAccumMicroDeg += deltaMicroDeg;
  uint16_t steps = angleAccumMicroDeg / DEG_PER_COLUMN_MICRO;
  if (steps == 0) {
    return;
  }
  angleAccumMicroDeg %= DEG_PER_COLUMN_MICRO;
  col = min<uint16_t>(MESSAGE_COLS, col + steps);
  if (col >= MESSAGE_COLS) {
    strokeActive = false;
    col = 0;
    currentColumn = 0xffff;
    return;
  }
  currentColumn = columnForStroke();
  showColumn(currentColumn);
}

void showColumn(uint16_t index) {
  uint16_t charCols = CHAR_COLS[currentChar];
  index = min<uint16_t>(index, charCols - 1);
  uint64_t mask = (CHAR_FRAMES[currentChar][index] << VERTICAL_SHIFT_UP) & DISPLAY_MASK;
  uint32_t floorColor = strip.Color(
    (static_cast<uint16_t>(textR) * CAMERA_FLOOR) / 255,
    (static_cast<uint16_t>(textG) * CAMERA_FLOOR) / 255,
    (static_cast<uint16_t>(textB) * CAMERA_FLOOR) / 255
  );
  uint32_t textColor = strip.Color(textR, textG, textB);

  for (uint16_t led = 0; led < DRIVER_LEDS; led++) {
    strip.setPixelColor(led, floorColor);
  }

  for (uint16_t led = 0; led < DISPLAY_LEDS; led++) {
    if ((mask >> led) & 1ULL) {
      strip.setPixelColor(led, textColor);
    }
  }
  strip.show();
}

void setup() {
  Serial.begin(115200);
  delay(250);
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.clear();
  strip.show();
  Wire.begin(SDA_PIN, SCL_PIN, I2C_FREQ);
  setupMpu();
  nextColumnUs = micros();
  nextCharMs = millis() + CHAR_SWITCH_MS;
  Serial.printf("Auto POV text demo, switch=%ums, X-axis reverse, GPIO%u, display=%u, columnUs=%u\n",
                CHAR_SWITCH_MS, LED_PIN, DISPLAY_LEDS, columnUs);
}

void loop() {
  handleSerial();
  uint32_t nowUs = micros();
  uint32_t nowMs = millis();

  if (static_cast<uint32_t>(nowUs - lastMotionReadUs) >= 4000UL) {
    lastMotionReadUs = nowUs;
    int16_t gx = 0;
    int16_t gy = 0;
    int16_t gz = 0;
    if (readMotion(gx, gy, gz)) {
      int32_t xSwing = static_cast<int32_t>(gx) - gyroBiasX;
      if (labs(xSwing) > SWING_THRESHOLD) {
        xMotionAccum += constrain(xSwing * 2, -900, 900);
        xMotionAccum = constrain(xMotionAccum, -2600, 2600);
        if (nowMs - lastReverseMs > 80) {
          if (xMotionAccum > 950 && scanDirection < 0) {
            scanDirection = 1;
            xMotionAccum = 0;
            lastReverseMs = nowMs;
          } else if (xMotionAccum < -950 && scanDirection > 0) {
            scanDirection = -1;
            xMotionAccum = 0;
            lastReverseMs = nowMs;
          }
        }
      } else {
        xMotionAccum = xMotionAccum * 85 / 100;
      }
    }
  }

  if (static_cast<int32_t>(nowMs - nextCharMs) >= 0) {
    currentChar = (currentChar + 1) % CHAR_COUNT;
    col = 0;
    nextCharMs += CHAR_SWITCH_MS;
  }

  if (static_cast<int32_t>(nowUs - nextColumnUs) >= 0) {
    uint16_t charCols = CHAR_COLS[currentChar];
    currentColumn = scanDirection > 0 ? col : (charCols - 1 - col);
    showColumn(currentColumn);
    col++;
    if (col >= charCols) {
      col = 0;
      nextColumnUs = nowUs + FRAME_GAP_US;
    } else {
      nextColumnUs = nowUs + columnUs;
    }
  }
}
