# 手机主控接入协议 V1

适用固件：`light_stick_v1`

## 设备侧参数

- BLE 名称：`LightStick-V1`
- Service UUID：`8f7a0001-6d2d-4d10-9c67-59414755414f`
- Command Characteristic UUID：`8f7a0002-6d2d-4d10-9c67-59414755414f`
- Status Characteristic UUID：`8f7a0003-6d2d-4d10-9c67-59414755414f`
- 当前设备活动号：`7`
- 当前设备分组：`0x01`
- 灯带接线：GPIO4 一条 15 颗，GPIO3 一条 15 颗

## 两种控制方式

### 1. 单设备连接控制

手机连接设备后，向 Command Characteristic 写入 21 字节命令包。

适合：调试、单根灯棒控制、配置设备。

Android 注意事项：

- 连接 GATT 成功后必须先调用 `requestMtu(64)` 或更高，再写入 21 字节命令包。
- Android 默认 ATT MTU 是 23，实际单次 payload 只有 20 字节；如果不请求 MTU，21 字节命令包会被截成 20 字节，固件会报 `PARSE_TOO_SHORT`。
- 等收到 `onMtuChanged(..., mtu >= 24, status=0)` 后，再发现服务或写入命令。

### 2. 手机 BLE 广播控制

手机把同样的 21 字节命令包放进 BLE 广播数据里，设备扫描到后执行。

设备侧目前支持两种广播数据来源：

- Manufacturer Data：可以直接放 21 字节，也可以前面带 2 字节 company id。
- Service Data：直接放 21 字节。

适合：一台手机同时控制附近多根灯棒。Android 更适合做这个广播主控；iOS 对广播内容、后台行为和 manufacturer data 控制更受限，后续如果要兼容 iPhone，建议 App 侧单独做一次真机验证。

2026-05-19 实测记录：

- Android 调试 App 使用 `BluetoothLeAdvertiser.addManufacturerData(0xffff, packet)` 广播同一份 21 字节命令包。
- ESP32-C3 串口收到 `adv-mfg len=21 result=applied(0)`，确认 Manufacturer Data 广播链路可用。
- 实测包：`59 01 07 FF 01 08 80 73 FF 14 5A 00 00 00 00 00 00 00 01 00 7D`。
- 解析结果：`effect=8`，`brightness=128`，`speed=115`，`primary=255,20,90`。

## 21 字节命令包

| 字节 | 含义 | 说明 |
| --- | --- | --- |
| 0 | magic | 固定 `0x59` |
| 1 | version | 固定 `0x01` |
| 2 | activity_id | 当前固件默认 `7` |
| 3 | group_mask | 分组掩码，控制 1 组用 `0x01`，全组可用 `0xff` |
| 4 | command | `1` 设置效果，`2` 全灭，`3` 测试 |
| 5 | effect_id | 效果编号；动态效果和纯色模式分开理解，见下方说明 |
| 6 | brightness | 亮度 `0-255` |
| 7 | speed | 速度 `0-255` |
| 8 | primary_r | 主色 R |
| 9 | primary_g | 主色 G |
| 10 | primary_b | 主色 B |
| 11 | secondary_r | 副色 R |
| 12 | secondary_g | 副色 G |
| 13 | secondary_b | 副色 B |
| 14 | music_energy | 手机分析出来的整体能量 |
| 15 | music_bass | 低频 |
| 16 | music_mid | 中频 |
| 17 | music_treble | 高频 |
| 18 | beat_index | 每次鼓点递增，用来触发闪动 |
| 19 | flags | bit0=`1` 表示音乐模式 |
| 20 | checksum | 前 20 字节校验 |

## 控制类型总览

| 控制类型 | command | effect_id | 是否动画 | 说明 |
| --- | --- |
| 动态灯效 | `1` | `1-7`、`9-50` | 是 | 颜色字段作为动态效果的主色/副色 |
| 纯色 | `1` | `8` | 否 | 只显示固定颜色，不播放动画 |
| 音乐灯效 | `1` | `5`、`6` | 是 | 手机持续发送音乐能量字段 |
| 全灭 | `2` | `250` | 否 | 关闭灯带 |

## 动态灯效控制

动态灯效用于“灯带一直在动”的场景。App 切换效果、调颜色、调速度、调亮度时，都可以发送同一个 21 字节包；关键是 `effect_id` 必须保持为动态效果编号。

格式：

```text
59 01 activity_id group_mask 01 effect_id brightness speed primary_R primary_G primary_B secondary_R secondary_G secondary_B music_energy music_bass music_mid music_treble beat_index flags checksum
```

动态效果编号：

| effect_id | 效果 | 颜色字段用途 |
| --- | --- | --- |
| `1` | 双向流星 | A 灯带偏主色，B 灯带偏副色 |
| `2` | 心跳呼吸 | 主色/副色混合呼吸 |
| `3` | 彩虹流动 | 内部彩虹色相流动，主色/副色只作为兼容字段 |
| `4` | 火焰 | 内部红黄火焰，音乐能量会影响高度 |
| `5` | 音乐脉冲 | 主色/副色随音乐能量混合 |
| `6` | 三频谱 | 低频/中频/高频分别控制颜色强度 |
| `7` | 水波扩散 | 主色到副色的扩散波纹 |
| `9` | 星光闪烁 | 主色/副色随机闪烁 |

第一版已扩展到 50 个效果，App 里按小按钮网格展示：

```text
1 双流星    2 心跳      3 彩虹      4 火焰      5 音乐脉冲
6 三频谱    7 水波      8 纯色      9 星闪      10 扫描
11 双扫描   12 剧院     13 彩剧院    14 跑浪     15 色环
16 闪点     17 暗闪     18 频闪      19 彩频闪   20 追逐
21 彩追逐   22 彩虹跑   23 极光      24 溪流     25 彗星
26 灯塔     27 烟花     28 雨滴      29 火闪     30 渐变
31 加载     32 双点     33 双区      34 马戏     35 三追
36 三淡入   37 闪电     38 多彗星    39 彩星     40 湖面
41 柔流星   42 彩 glitter 43 烛光    44 弹球     45 Sinelon
46 双 Sinelon 47 爆米花 48 等离子    49 海浪     50 音乐星火
```

2026-05-19 新效果广播实测：

- `effect=23` 极光：ESP32-C3 收到 `adv-mfg len=21 result=applied(0)`。
- `effect=48` 等离子、`effect=50` 音乐星火：Android App 日志确认按 21 字节广播发出。
- 麦克风律动：手机采集麦克风后持续广播 `effect=5`，`flags=0x01`，`music_energy/bass/mid/treble` 字段会随声音变化。
- 麦克风低延迟优化：Android 侧使用持续 AdvertisingSet 广播流，不再每帧停止/重新启动 BLE 广播；采样帧约 `32ms`，广播数据更新约 `55ms` 节流。
- 进一步实测结论：手机 BLE 广播更新仍然只有几 Hz 级别，不适合实时音乐律动。测试 App 现在支持“先连接设备，再开启麦克风”的 GATT 低延迟模式，实测约 `18Hz`，ESP32-C3 串口收到 `gatt len=21 result=applied(0)` 连续音乐包。

App 行为建议：

- “效果选择”改变 `effect_id`。
- “颜色选择器”只改变 `primary/secondary`，不要把 `effect_id` 改成 `8`。
- “速度滑杆”改变 `speed`。
- “亮度滑杆”改变 `brightness`。
- 如果用户当前选的是流星，点颜色后仍然发 `effect_id=1`。
- 如果用户当前选的是心跳，点颜色后仍然发 `effect_id=2`。

示例：红蓝双向流星。

```text
59 01 07 FF 01 01 50 78 FF 00 00 00 00 FF 00 00 00 00 01 00 9A
```

解析：

```text
command = 1
effect_id = 1
brightness = 0x50 = 80
speed = 0x78 = 120
primary = FF 00 00
secondary = 00 00 FF
```

## 纯色控制

纯色控制用于“整根灯棒固定显示一个颜色”的场景。它不是动画效果。

格式：

```text
59 01 activity_id group_mask 01 08 brightness speed R G B secondary_R secondary_G secondary_B 00 00 00 00 beat_index flags checksum
```

字段说明：

| 字段 | 推荐值 | 说明 |
| --- | --- | --- |
| `command` | `1` | 设置显示状态 |
| `effect_id` | `8` | 固定表示纯色模式 |
| `brightness` | `0-255` | 纯色亮度 |
| `speed` | `0` 或保留上次值 | 纯色模式下没有动画速度 |
| `primary_R/G/B` | 用户选择颜色 | 当前固件默认使用主色显示纯色 |
| `secondary_R/G/B` | 可填 `0,0,0` 或备用色 | 第一版可不依赖 |
| `music_*` | `0` | 纯色模式不需要音乐字段 |
| `flags` | `0` | 非音乐模式 |

App 行为建议：

- 只有用户明确进入“纯色/固定颜色”模式时，才发送 `effect_id=8`。
- 如果用户在动态效果界面点颜色，不要发 `effect_id=8`。
- 纯色模式下灯带不动是正确表现。

示例：玫粉纯色。

```text
59 01 07 FF 01 08 78 00 FF 14 5A 00 00 00 00 00 00 00 01 00 2D
```

解析：

```text
command = 1
effect_id = 8
brightness = 0x78 = 120
primary = FF 14 5A
```

## 校验算法

```js
function checksum(bytes20) {
  let c = 0xA7
  for (let i = 0; i < 20; i++) {
    c ^= bytes20[i]
    c = ((c << 1) | (c >> 7)) & 0xff
  }
  return c
}
```

## JavaScript 组包示例

```js
function makeLightCommand({
  activityId = 7,
  groupMask = 0xff,
  command = 1,
  effectId = 5,
  brightness = 80,
  speed = 120,
  primary = [255, 20, 0],
  secondary = [0, 60, 255],
  music = [180, 200, 90, 60],
  beatIndex = 1,
  flags = 1,
} = {}) {
  const b = new Uint8Array(21)
  b[0] = 0x59
  b[1] = 0x01
  b[2] = activityId
  b[3] = groupMask
  b[4] = command
  b[5] = effectId
  b[6] = brightness
  b[7] = speed
  b.set(primary, 8)
  b.set(secondary, 11)
  b.set(music, 14)
  b[18] = beatIndex
  b[19] = flags
  b[20] = checksum(b.subarray(0, 20))
  return b
}
```

## 第一版建议

- App 普通灯效：每次切换效果发 1 包即可。
- App 颜色选择器：不要固定发送 `effect_id=8`，否则灯棒会进入“分组纯色”静态效果，看起来就是没有动画。颜色选择器应该带着当前选中的动态效果一起发，比如流星用 `effect_id=1`，心跳用 `effect_id=2`，彩虹用 `effect_id=3`，音乐脉冲用 `effect_id=5`。
- App 音乐模式：建议每秒 20-30 包，持续更新 `music_energy/bass/mid/treble/beat_index`。
- 多灯棒统一控制：`group_mask=0xff`。
- 分组控制：比如 A 组 `0x01`，B 组 `0x02`，C 组 `0x04`。

## 2026-05-19 手机联调记录

实测手机端发来的包：

```text
59 01 07 FF 01 08 CC 1F FA 68 81 00 3C FF 00 00 00 00 00 00 F8
```

解析结果：

- `effect_id=8`：分组纯色，所以不会有动画。
- `brightness=204`：亮度偏高。
- `primary=250,104,129`：很亮的浅粉，肉眼容易接近发白。
- `secondary=0,60,255`

结论：手机已经成功写入控制包；问题在于 App 点颜色时发送了静态纯色效果，而不是当前动态效果。

## 已知正确测试包

手机端调试时，先不要走颜色选择器和对象序列化，直接按二进制字节写入下面任意一包。不要把 hex 字符串当文本写入，要转换成 21 个 byte。

红蓝双向流星：

```text
59 01 07 FF 01 01 50 78 FF 00 00 00 00 FF 00 00 00 00 01 00 9A
```

绿色心跳：

```text
59 01 07 FF 01 02 50 78 00 FF 00 00 00 28 00 00 00 00 01 00 F0
```

彩虹流动：

```text
59 01 07 FF 01 03 50 8C FF 00 00 00 00 FF 00 00 00 00 01 00 05
```

全灭：

```text
59 01 07 FF 02 0A 00 00 00 00 00 00 00 00 00 00 00 00 01 00 07
```

音乐脉冲：

```text
59 01 07 FF 01 05 5A 78 FF 14 00 00 3C FF B4 C8 5A 3C 02 01 F8
```
