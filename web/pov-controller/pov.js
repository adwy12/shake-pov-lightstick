let messageCols = [
  0x7f, 0x02, 0x04, 0x02, 0x7f, 0x00,
  0x7f, 0x41, 0x41, 0x22, 0x1c, 0x00,
  0x7f, 0x20, 0x18, 0x20, 0x7f, 0x00, 0x00,
];

const FONT_5X7 = {
  " ": [0x00, 0x00, 0x00],
  "0": [0x3e, 0x45, 0x49, 0x51, 0x3e],
  "1": [0x00, 0x21, 0x7f, 0x01, 0x00],
  "2": [0x23, 0x45, 0x49, 0x51, 0x21],
  "3": [0x42, 0x41, 0x51, 0x69, 0x46],
  "4": [0x0c, 0x14, 0x24, 0x7f, 0x04],
  "5": [0x72, 0x51, 0x51, 0x51, 0x4e],
  "6": [0x1e, 0x29, 0x49, 0x49, 0x06],
  "7": [0x40, 0x47, 0x48, 0x50, 0x60],
  "8": [0x36, 0x49, 0x49, 0x49, 0x36],
  "9": [0x30, 0x49, 0x49, 0x4a, 0x3c],
  "A": [0x3f, 0x44, 0x44, 0x44, 0x3f],
  "B": [0x7f, 0x49, 0x49, 0x49, 0x36],
  "C": [0x3e, 0x41, 0x41, 0x41, 0x22],
  "D": [0x7f, 0x41, 0x41, 0x22, 0x1c],
  "E": [0x7f, 0x49, 0x49, 0x49, 0x41],
  "F": [0x7f, 0x48, 0x48, 0x48, 0x40],
  "G": [0x3e, 0x41, 0x49, 0x49, 0x2e],
  "H": [0x7f, 0x08, 0x08, 0x08, 0x7f],
  "I": [0x00, 0x41, 0x7f, 0x41, 0x00],
  "J": [0x02, 0x01, 0x41, 0x7e, 0x40],
  "K": [0x7f, 0x08, 0x14, 0x22, 0x41],
  "L": [0x7f, 0x01, 0x01, 0x01, 0x01],
  "M": [0x7f, 0x20, 0x18, 0x20, 0x7f],
  "N": [0x7f, 0x10, 0x08, 0x04, 0x7f],
  "O": [0x3e, 0x41, 0x41, 0x41, 0x3e],
  "P": [0x7f, 0x48, 0x48, 0x48, 0x30],
  "Q": [0x3e, 0x41, 0x45, 0x42, 0x3d],
  "R": [0x7f, 0x48, 0x4c, 0x4a, 0x31],
  "S": [0x31, 0x49, 0x49, 0x49, 0x46],
  "T": [0x40, 0x40, 0x7f, 0x40, 0x40],
  "U": [0x7e, 0x01, 0x01, 0x01, 0x7e],
  "V": [0x7c, 0x02, 0x01, 0x02, 0x7c],
  "W": [0x7f, 0x02, 0x0c, 0x02, 0x7f],
  "X": [0x63, 0x14, 0x08, 0x14, 0x63],
  "Y": [0x70, 0x08, 0x07, 0x08, 0x70],
  "Z": [0x43, 0x45, 0x49, 0x51, 0x61],
};

const config = {
  swingThreshold: 220,
  restThreshold: 90,
  strongSwing: 2600,
  maxColumnDelayUs: 24000,
  minColumnDelayUs: 9500,
  stillTimeoutMs: 80,
  brightness: 135,
  oneShot: 1,
  reverse: 0,
  invertRows: 0,
  textSpeed: 100,
  autoRange: 1,
  rangeLearning: 28,
  xyOnly: 1,
};

const controlDefs = [
  ["swingThreshold", "触发阈值", 0, 4000, 10],
  ["restThreshold", "静止阈值", 0, 2000, 10],
  ["strongSwing", "手动画幅上限", 200, 16000, 100],
  ["textSpeed", "文字速度", 25, 300, 5],
  ["maxColumnDelayUs", "慢摇列间隔", 1000, 90000, 500],
  ["minColumnDelayUs", "快摇列间隔", 300, 90000, 100],
  ["stillTimeoutMs", "静止清屏", 20, 2000, 10],
  ["rangeLearning", "画幅学习", 1, 90, 1],
  ["brightness", "亮度", 1, 255, 1],
];

const $ = (selector) => document.querySelector(selector);
const statusDot = $("#statusDot");
const statusText = $("#statusText");
const connectBtn = $("#connectBtn");
const disconnectBtn = $("#disconnectBtn");
const previewBtn = $("#previewBtn");
const clearBtn = $("#clearBtn");
const getBtn = $("#getBtn");
const textInput = $("#textInput");
const sendTextBtn = $("#sendTextBtn");
const controls = $("#controls");
const oneShotInput = $("#oneShotInput");
const autoRangeInput = $("#autoRangeInput");
const xyOnlyInput = $("#xyOnlyInput");
const reverseInput = $("#reverseInput");
const invertRowsInput = $("#invertRowsInput");
const logBox = $("#logBox");
const lastCommand = $("#lastCommand");
const speedNow = $("#speedNow");
const rangeNow = $("#rangeNow");
const axisNow = $("#axisNow");
const columnNow = $("#columnNow");
const activeNow = $("#activeNow");
const fontCanvas = $("#fontCanvas");
const stripCanvas = $("#stripCanvas");

const state = {
  port: null,
  reader: null,
  writer: null,
  readActive: false,
  buffer: "",
  lastColumn: 0,
};

function textToColumns(text) {
  const clean = text.toUpperCase().replace(/[^A-Z0-9 ]/g, "").slice(0, 16) || "HELLO";
  const cols = [];
  for (const char of clean) {
    const glyph = FONT_5X7[char] || FONT_5X7[" "];
    cols.push(...glyph, 0x00);
  }
  cols.push(0x00);
  return cols.slice(0, 96);
}

function sendTextToBoard() {
  messageCols = textToColumns(textInput.value);
  textInput.value = textInput.value.toUpperCase().replace(/[^A-Z0-9 ]/g, "").slice(0, 16) || "HELLO";
  state.lastColumn = 0;
  drawPreview();
  const hex = messageCols.map((value) => value.toString(16).padStart(2, "0").toUpperCase()).join("");
  return sendLine(`COLS,${hex}`);
}

function setConnected(connected, text) {
  statusDot.classList.toggle("on", connected);
  statusText.textContent = text;
  connectBtn.disabled = connected;
  disconnectBtn.disabled = !connected;
}

function appendLog(line) {
  const time = new Date().toLocaleTimeString();
  logBox.textContent = `[${time}] ${line}\n${logBox.textContent}`.slice(0, 8000);
}

function buildControls() {
  controls.innerHTML = "";
  for (const [key, label, min, max, step] of controlDefs) {
    const row = document.createElement("label");
    row.className = "control";
    row.innerHTML = `
      <span>${label}</span>
      <input id="${key}Input" type="range" min="${min}" max="${max}" step="${step}" value="${config[key]}" />
      <b id="${key}Value">${config[key]}</b>
    `;
    controls.appendChild(row);
    const input = $(`#${key}Input`);
    input.addEventListener("input", () => {
      config[key] = Number(input.value);
      normalizeDelayPair(key);
      syncControls();
      drawPreview();
      queueConfigSend();
    });
  }

  oneShotInput.addEventListener("change", () => {
    config.oneShot = oneShotInput.checked ? 1 : 0;
    queueConfigSend();
  });
  autoRangeInput.addEventListener("change", () => {
    config.autoRange = autoRangeInput.checked ? 1 : 0;
    queueConfigSend();
  });
  xyOnlyInput.addEventListener("change", () => {
    config.xyOnly = xyOnlyInput.checked ? 1 : 0;
    queueConfigSend();
  });
  reverseInput.addEventListener("change", () => {
    config.reverse = reverseInput.checked ? 1 : 0;
    drawPreview();
    queueConfigSend();
  });
  invertRowsInput.addEventListener("change", () => {
    config.invertRows = invertRowsInput.checked ? 1 : 0;
    drawPreview();
    queueConfigSend();
  });
}

function normalizeDelayPair(changedKey) {
  if (config.minColumnDelayUs <= config.maxColumnDelayUs) {
    return;
  }
  if (changedKey === "minColumnDelayUs") {
    config.maxColumnDelayUs = config.minColumnDelayUs;
  } else {
    config.minColumnDelayUs = config.maxColumnDelayUs;
  }
}

function syncControls() {
  for (const [key] of controlDefs) {
    const input = $(`#${key}Input`);
    const value = $(`#${key}Value`);
    if (input) input.value = config[key];
    if (value) value.textContent = config[key];
  }
  oneShotInput.checked = Boolean(config.oneShot);
  autoRangeInput.checked = Boolean(config.autoRange);
  xyOnlyInput.checked = Boolean(config.xyOnly);
  reverseInput.checked = Boolean(config.reverse);
  invertRowsInput.checked = Boolean(config.invertRows);
  drawPreview();
}

function configLine() {
  return [
    "CFG",
    config.swingThreshold,
    config.restThreshold,
    config.strongSwing,
    config.maxColumnDelayUs,
    config.minColumnDelayUs,
    config.stillTimeoutMs,
    config.brightness,
    config.oneShot,
    config.reverse,
    config.invertRows,
    config.textSpeed,
    config.autoRange,
    config.rangeLearning,
    config.xyOnly,
  ].join(",");
}

let sendTimer = 0;
function queueConfigSend() {
  window.clearTimeout(sendTimer);
  sendTimer = window.setTimeout(() => sendLine(configLine()), 35);
}

async function sendLine(line) {
  if (!state.writer) {
    lastCommand.textContent = "未连接，参数只更新了页面";
    return;
  }
  const payload = new TextEncoder().encode(`${line}\n`);
  await state.writer.write(payload);
  lastCommand.textContent = line;
}

async function connectSerial() {
  if (!("serial" in navigator)) {
    appendLog("当前浏览器不支持 Web Serial，请用 Chrome 打开这个页面");
    return;
  }

  state.port = await navigator.serial.requestPort({
    filters: [{ usbVendorId: 0x303a }],
  });
  await state.port.open({ baudRate: 115200 });
  state.writer = state.port.writable.getWriter();
  state.reader = state.port.readable.getReader();
  state.readActive = true;
  setConnected(true, "已连接 C3");
  appendLog("串口已连接");
  readLoop();
  await sleep(260);
  await sendLine("GET");
}

async function disconnectSerial() {
  state.readActive = false;
  try {
    if (state.reader) {
      await state.reader.cancel();
      state.reader.releaseLock();
    }
  } catch {}
  try {
    if (state.writer) {
      state.writer.releaseLock();
    }
  } catch {}
  try {
    if (state.port) {
      await state.port.close();
    }
  } catch {}
  state.port = null;
  state.reader = null;
  state.writer = null;
  setConnected(false, "未连接");
  appendLog("串口已断开");
}

async function readLoop() {
  const decoder = new TextDecoder();
  while (state.readActive && state.reader) {
    try {
      const { value, done } = await state.reader.read();
      if (done) break;
      state.buffer += decoder.decode(value, { stream: true });
      const lines = state.buffer.split(/\r?\n/);
      state.buffer = lines.pop() || "";
      for (const line of lines) {
        handleSerialLine(line.trim());
      }
    } catch (error) {
      if (state.readActive) appendLog(`读取错误：${error.message}`);
      break;
    }
  }
}

function handleSerialLine(line) {
  if (!line) return;
  appendLog(line);
  if (line.startsWith("COLS,")) {
    lastCommand.textContent = `已发送 ${line.slice(5)} 列文字`;
    return;
  }
  if (line.startsWith("CFG,")) {
    const values = line.split(",").slice(1).map(Number);
    [
      config.swingThreshold,
      config.restThreshold,
      config.strongSwing,
      config.maxColumnDelayUs,
      config.minColumnDelayUs,
      config.stillTimeoutMs,
      config.brightness,
      config.oneShot,
      config.reverse,
      config.invertRows,
      config.textSpeed,
      config.autoRange,
      config.rangeLearning,
      config.xyOnly,
    ] = values;
    if (!Number.isFinite(config.textSpeed)) config.textSpeed = 100;
    if (!Number.isFinite(config.autoRange)) config.autoRange = 1;
    if (!Number.isFinite(config.rangeLearning)) config.rangeLearning = 28;
    if (!Number.isFinite(config.xyOnly)) config.xyOnly = 1;
    syncControls();
    return;
  }

  const match = line.match(/POV axis=(\w) swing=(-?\d+) speed=(\d+)(?: range=(\d+))? dir=(-?\d+) col=(\d+) active=(\d+) done=(\d+)/);
  if (match) {
    axisNow.textContent = match[1];
    speedNow.textContent = match[3];
    rangeNow.textContent = match[4] || config.strongSwing;
    columnNow.textContent = match[6];
    activeNow.textContent = match[7] === "1" ? "active" : (match[8] === "1" ? "done" : "idle");
    state.lastColumn = Number(match[6]);
    drawPreview();
  }
}

function sleep(ms) {
  return new Promise((resolve) => window.setTimeout(resolve, ms));
}

function drawPreview() {
  drawFontCanvas();
  drawStripCanvas();
}

function rowColor(row) {
  const colors = ["#ff235a", "#ff7300", "#ffe600", "#28ff78", "#00d2ff", "#505fff", "#ff46dc"];
  return colors[row % colors.length];
}

function drawFontCanvas() {
  const ctx = fontCanvas.getContext("2d");
  const width = fontCanvas.width;
  const height = fontCanvas.height;
  ctx.clearRect(0, 0, width, height);
  ctx.fillStyle = "#0c0d10";
  ctx.fillRect(0, 0, width, height);

  const columns = config.reverse ? [...messageCols].reverse() : messageCols;
  const cell = Math.floor(Math.min((width - 56) / columns.length, (height - 46) / 7));
  const startX = Math.floor((width - columns.length * cell) / 2);
  const startY = Math.floor((height - 7 * cell) / 2);

  for (let col = 0; col < columns.length; col++) {
    const bits = columns[col];
    for (let row = 0; row < 7; row++) {
      const visualRow = config.invertRows ? row : 6 - row;
      const x = startX + col * cell;
      const y = startY + visualRow * cell;
      ctx.fillStyle = bits & (1 << row) ? rowColor(row) : "#1c2028";
      ctx.fillRect(x + 2, y + 2, cell - 4, cell - 4);
    }
  }

  const activeCol = Math.max(0, Math.min(columns.length - 1, state.lastColumn));
  ctx.strokeStyle = "#ffffff";
  ctx.lineWidth = 2;
  ctx.strokeRect(startX + activeCol * cell + 1, startY + 1, cell - 2, 7 * cell - 2);
}

function drawStripCanvas() {
  const ctx = stripCanvas.getContext("2d");
  const width = stripCanvas.width;
  const height = stripCanvas.height;
  ctx.clearRect(0, 0, width, height);
  ctx.fillStyle = "#0c0d10";
  ctx.fillRect(0, 0, width, height);

  const radius = 13;
  const gap = (width - 40 - radius * 2 * 15) / 14;
  const y = height / 2;
  const bits = messageCols[state.lastColumn % messageCols.length] || 0;
  for (let pixel = 0; pixel < 15; pixel++) {
    let color = "#202631";
    if (pixel < 14) {
      const visualRow = Math.floor(pixel / 2);
      const row = config.invertRows ? visualRow : 6 - visualRow;
      if (bits & (1 << row)) color = rowColor(row);
    }
    const x = 20 + radius + pixel * (radius * 2 + gap);
    ctx.beginPath();
    ctx.arc(x, y, radius, 0, Math.PI * 2);
    ctx.fillStyle = color;
    ctx.fill();
  }
}

connectBtn.addEventListener("click", () => connectSerial().catch((error) => appendLog(`连接失败：${error.message}`)));
disconnectBtn.addEventListener("click", () => disconnectSerial());
previewBtn.addEventListener("click", () => sendLine("PREVIEW"));
clearBtn.addEventListener("click", () => sendLine("CLEAR"));
getBtn.addEventListener("click", () => sendLine("GET"));
sendTextBtn.addEventListener("click", () => sendTextToBoard());
textInput.addEventListener("change", () => sendTextToBoard());

buildControls();
messageCols = textToColumns(textInput.value);
syncControls();
setConnected(false, "未连接");
