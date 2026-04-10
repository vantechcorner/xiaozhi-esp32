# An MCP-based Chatbot

（中文 | [English](README.md) | [日本語](README_ja.md)）

## 介绍

👉 [人类：给 AI 装摄像头 vs AI：当场发现主人三天没洗头【bilibili】](https://www.bilibili.com/video/BV1bpjgzKEhd/)

👉 [手工打造你的 AI 女友，新手入门教程【bilibili】](https://www.bilibili.com/video/BV1XnmFYLEJN/)

小智 AI 聊天机器人作为一个语音交互入口，利用 Qwen / DeepSeek 等大模型的 AI 能力，通过 MCP 协议实现多端控制。

<img src="docs/mcp-based-graph.jpg" alt="通过MCP控制万物" width="320">

### 版本说明

当前 v2 版本与 v1 版本分区表不兼容，所以无法从 v1 版本通过 OTA 升级到 v2 版本。分区表说明参见 [partitions/v2/README.md](partitions/v2/README.md)。

使用 v1 版本的所有硬件，可以通过手动烧录固件来升级到 v2 版本。

v1 的稳定版本为 1.9.2，可以通过 `git checkout v1` 来切换到 v1 版本，该分支会持续维护到 2026 年 2 月。

### 已实现功能

- Wi-Fi / ML307 Cat.1 4G
- 离线语音唤醒 [ESP-SR](https://github.com/espressif/esp-sr)
- 支持两种通信协议（[Websocket](docs/websocket.md) 或 MQTT+UDP）
- 采用 OPUS 音频编解码
- 基于流式 ASR + LLM + TTS 架构的语音交互
- 声纹识别，识别当前说话人的身份 [3D Speaker](https://github.com/modelscope/3D-Speaker)
- OLED / LCD 显示屏，支持表情显示
- 电量显示与电源管理
- 支持多语言（中文、英文、日文）
- 支持 ESP32-C3、ESP32-S3、ESP32-P4 芯片平台
- 通过设备端 MCP 实现设备控制（音量、灯光、电机、GPIO 等）
- 通过云端 MCP 扩展大模型能力（智能家居控制、PC桌面操作、知识搜索、邮件收发等）
- 自定义唤醒词、字体、表情与聊天背景，支持网页端在线修改 ([自定义Assets生成器](https://github.com/78/xiaozhi-assets-generator))

## 硬件

### 面包板手工制作实践

详见飞书文档教程：

👉 [《小智 AI 聊天机器人百科全书》](https://ccnphfhqs21z.feishu.cn/wiki/F5krwD16viZoF0kKkvDcrZNYnhb?from=from_copylink)

面包板效果图如下：

![面包板效果图](docs/v1/wiring2.jpg)

### Cytron Maker Feather AIoT S3（社区接线）

本仓库支持 [Cytron Maker Feather AIoT S3](https://www.cytron.io/p-maker-feather-aiot-s3)（ESP32-S3，8 MB Flash，OPI PSRAM）的一种 **DIY 组合**：Cytron **ST7789 240×240** SPI 屏（7 线、模块上 CS 常接 GND）、**INMP441** I2S 麦克风、**MAX98357** I2S 功放。下表引脚名为开发板丝印 **Dxx**。本仓库内参考文件：[针脚图](docs/MAKER-FEATHER-AIOT-S3%20Pinout.png)、[开发板说明书（PDF）](docs/Maker%20Feather%20AIoT%20S3%20Datasheet.pdf)（最新资料以 Cytron 官网为准）。

**物料清单**

| 器件 | 作用 |
|------|------|
| Cytron Maker Feather AIoT S3 | 主控 |
| Cytron（或兼容）ST7789 240×240 IPS，7 线 SPI | 显示屏 |
| INMP441 模块 | I2S 麦克风 |
| MAX98357A / I2S 功放模块 | 喇叭输出 |
| 喇叭（4–8 Ω）、USB 线、杜邦线 | — |

**显示屏（ST7789 7 线）→ Feather**

| 屏 | Feather | 说明 |
|----|---------|------|
| GND | GND | 共地 |
| VCC | 3V3 | ESP32-S3 请用 3.3 V |
| SCL | **D17**（SCK） | SPI 时钟 |
| SDA | **D8**（MOSI） | SPI 数据 |
| RES | **D16** | 复位 |
| DC | **D18** | 命令/数据 |
| BLK | **VPeripheral** 或 悬空 / 3V3 | Cytron：BLK 悬空常亮；拉低关。BLK 接 VPeripheral 时由 **GPIO11** 控制供电轨 |

**软件 CS 说明：** 模块无 CS 引脚，驱动仍需要 **Dummy CS**：固件使用 **D7**，**不要把 D7 接到屏幕**。

**INMP441 → Feather**

| INMP441 | Feather |
|---------|---------|
| VDD | 3V3 |
| GND | GND |
| L/R | GND（单声道） |
| SCK | **D42**（丝印 **SDA**，I2S BCLK） |
| WS | **D41**（丝印 **SCL**，I2S LRCK） |
| SD | **D40**（I2S DIN） |

**MAX98357 → Feather**

| MAX98357 | Feather |
|----------|---------|
| Vin | **VUSB**（USB 5 V） |
| GND | GND |
| LRC | **D41**（与麦克风共用 WS） |
| BCLK | **D42**（与麦克风共用 BCLK） |
| DIN | **D48**（I2S DOUT） |

**编译与烧录（ESP-IDF）**

1. 安装 **ESP-IDF 5.4+**（5.5.x 亦可）。Windows 建议使用开始菜单中的 **ESP-IDF PowerShell**，确保 `idf.py` 与 **Python 3.11** 虚拟环境正确（避免其它终端里 Python 3.13 抢 PATH）。
2. 若 PowerShell 禁止脚本：先执行 `Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass` 再运行 `export.ps1`，或在 **cmd** 中使用 `export.bat`。
3. 在项目根目录执行：

```bash
python scripts/release.py maker-feather-aiot-s3 --name maker-feather-aiot-s3
```

若提示 **Skipping … zip already exists**，请删除 `releases` 下对应 zip 后重试，或在已生成正确 `sdkconfig` 后直接使用 `idf.py build`。

4. 烧录（将 `COMx` 换成实际端口，如 `COM15`）：

```bash
idf.py -p COMx flash monitor
```

**menuconfig 方式：** 将 **Board Type** 选为 **Cytron Maker Feather AIoT S3**，Flash **8 MB**，分区表选 **v2 / 8 MB**（与 `release.py` 等效）。界面语言请在 **Default Language** 中选 **English**，状态栏即显示英文（如 “Connecting…”、“Listening…”、“Loading assets…”）。使用 `release.py` 时 `config.json` 已附带英文语言选项；若仍为中文，执行 `idf.py fullclean` 后重新编译。

更细的背光与引脚说明见 `main/boards/maker-feather-aiot-s3/config.h`。英文说明见 [README.md](README.md#cytron-maker-feather-aiot-s3-community-wiring)。

**固件交互：** 板载 **D3** 用户键映射为 `ToggleChatState()`（与启动后 BOOT 键同样用于切换对话/唤醒逻辑）。**D46** WS2812 作为 `Led` 输出，状态灯效与其它板一致，在 **说话（TTS）** 状态下为 **绿色闪烁**。

### 支持 70 多个开源硬件（仅展示部分）

- <a href="https://oshwhub.com/li-chuang-kai-fa-ban/li-chuang-shi-zhan-pai-esp32-s3-kai-fa-ban" target="_blank" title="立创·实战派 ESP32-S3 开发板">立创·实战派 ESP32-S3 开发板</a>
- <a href="https://github.com/espressif/esp-box" target="_blank" title="乐鑫 ESP32-S3-BOX3">乐鑫 ESP32-S3-BOX3</a>
- <a href="https://docs.m5stack.com/zh_CN/core/CoreS3" target="_blank" title="M5Stack CoreS3">M5Stack CoreS3</a>
- <a href="https://docs.m5stack.com/en/atom/Atomic%20Echo%20Base" target="_blank" title="AtomS3R + Echo Base">M5Stack AtomS3R + Echo Base</a>
- <a href="https://gf.bilibili.com/item/detail/1108782064" target="_blank" title="神奇按钮 2.4">神奇按钮 2.4</a>
- <a href="https://www.waveshare.net/shop/ESP32-S3-Touch-AMOLED-1.8.htm" target="_blank" title="微雪电子 ESP32-S3-Touch-AMOLED-1.8">微雪电子 ESP32-S3-Touch-AMOLED-1.8</a>
- <a href="https://github.com/Xinyuan-LilyGO/T-Circle-S3" target="_blank" title="LILYGO T-Circle-S3">LILYGO T-Circle-S3</a>
- <a href="https://oshwhub.com/tenclass01/xmini_c3" target="_blank" title="虾哥 Mini C3">虾哥 Mini C3</a>
- <a href="https://oshwhub.com/movecall/cuican-ai-pendant-lights-up-y" target="_blank" title="Movecall CuiCan ESP32S3">璀璨·AI 吊坠</a>
- <a href="https://github.com/WMnologo/xingzhi-ai" target="_blank" title="无名科技Nologo-星智-1.54">无名科技 Nologo-星智-1.54TFT</a>
- <a href="https://www.seeedstudio.com/SenseCAP-Watcher-W1-A-p-5979.html" target="_blank" title="SenseCAP Watcher">SenseCAP Watcher</a>
- <a href="https://www.bilibili.com/video/BV1BHJtz6E2S/" target="_blank" title="ESP-HI 超低成本机器狗">ESP-HI 超低成本机器狗</a>
- <a href="https://www.cytron.io/p-maker-feather-aiot-s3" target="_blank" title="Cytron Maker Feather AIoT S3">Cytron Maker Feather AIoT S3</a>（接线与编译说明见本文「Cytron Maker Feather AIoT S3（社区接线）」一节）

<div style="display: flex; justify-content: space-between;">
  <a href="docs/v1/lichuang-s3.jpg" target="_blank" title="立创·实战派 ESP32-S3 开发板">
    <img src="docs/v1/lichuang-s3.jpg" width="240" />
  </a>
  <a href="docs/v1/espbox3.jpg" target="_blank" title="乐鑫 ESP32-S3-BOX3">
    <img src="docs/v1/espbox3.jpg" width="240" />
  </a>
  <a href="docs/v1/m5cores3.jpg" target="_blank" title="M5Stack CoreS3">
    <img src="docs/v1/m5cores3.jpg" width="240" />
  </a>
  <a href="docs/v1/atoms3r.jpg" target="_blank" title="AtomS3R + Echo Base">
    <img src="docs/v1/atoms3r.jpg" width="240" />
  </a>
  <a href="docs/v1/magiclick.jpg" target="_blank" title="神奇按钮 2.4">
    <img src="docs/v1/magiclick.jpg" width="240" />
  </a>
  <a href="docs/v1/waveshare.jpg" target="_blank" title="微雪电子 ESP32-S3-Touch-AMOLED-1.8">
    <img src="docs/v1/waveshare.jpg" width="240" />
  </a>
  <a href="docs/v1/lilygo-t-circle-s3.jpg" target="_blank" title="LILYGO T-Circle-S3">
    <img src="docs/v1/lilygo-t-circle-s3.jpg" width="240" />
  </a>
  <a href="docs/v1/xmini-c3.jpg" target="_blank" title="虾哥 Mini C3">
    <img src="docs/v1/xmini-c3.jpg" width="240" />
  </a>
  <a href="docs/v1/movecall-cuican-esp32s3.jpg" target="_blank" title="CuiCan">
    <img src="docs/v1/movecall-cuican-esp32s3.jpg" width="240" />
  </a>
  <a href="docs/v1/wmnologo_xingzhi_1.54.jpg" target="_blank" title="无名科技Nologo-星智-1.54">
    <img src="docs/v1/wmnologo_xingzhi_1.54.jpg" width="240" />
  </a>
  <a href="docs/v1/sensecap_watcher.jpg" target="_blank" title="SenseCAP Watcher">
    <img src="docs/v1/sensecap_watcher.jpg" width="240" />
  </a>
  <a href="docs/v1/esp-hi.jpg" target="_blank" title="ESP-HI 超低成本机器狗">
    <img src="docs/v1/esp-hi.jpg" width="240" />
  </a>
</div>

## 软件

### 固件烧录

新手第一次操作建议先不要搭建开发环境，直接使用免开发环境烧录的固件。

固件默认接入 [xiaozhi.me](https://xiaozhi.me) 官方服务器，个人用户注册账号可以免费使用 Qwen 实时模型。

👉 [新手烧录固件教程](https://ccnphfhqs21z.feishu.cn/wiki/Zpz4wXBtdimBrLk25WdcXzxcnNS)

### 开发环境

- Cursor 或 VSCode
- 安装 ESP-IDF 插件，选择 SDK 版本 5.4 或以上
- Linux 比 Windows 更好，编译速度快，也免去驱动问题的困扰
- 本项目使用 Google C++ 代码风格，提交代码时请确保符合规范

### 开发者文档

- [自定义开发板指南](docs/custom-board.md) - 学习如何为小智 AI 创建自定义开发板
- [MCP 协议物联网控制用法说明](docs/mcp-usage.md) - 了解如何通过 MCP 协议控制物联网设备
- [MCP 协议交互流程](docs/mcp-protocol.md) - 设备端 MCP 协议的实现方式
- [MQTT + UDP 混合通信协议文档](docs/mqtt-udp.md)
- [一份详细的 WebSocket 通信协议文档](docs/websocket.md)

## 大模型配置

如果你已经拥有一个小智 AI 聊天机器人设备，并且已接入官方服务器，可以登录 [xiaozhi.me](https://xiaozhi.me) 控制台进行配置。

👉 [后台操作视频教程（旧版界面）](https://www.bilibili.com/video/BV1jUCUY2EKM/)

## 相关开源项目

在个人电脑上部署服务器，可以参考以下第三方开源的项目：

- [xinnan-tech/xiaozhi-esp32-server](https://github.com/xinnan-tech/xiaozhi-esp32-server) Python 服务器
- [joey-zhou/xiaozhi-esp32-server-java](https://github.com/joey-zhou/xiaozhi-esp32-server-java) Java 服务器
- [AnimeAIChat/xiaozhi-server-go](https://github.com/AnimeAIChat/xiaozhi-server-go) Golang 服务器
- [hackers365/xiaozhi-esp32-server-golang](https://github.com/hackers365/xiaozhi-esp32-server-golang) Golang 服务器

使用小智通信协议的第三方客户端项目：

- [huangjunsen0406/py-xiaozhi](https://github.com/huangjunsen0406/py-xiaozhi) Python 客户端
- [TOM88812/xiaozhi-android-client](https://github.com/TOM88812/xiaozhi-android-client) Android 客户端
- [100askTeam/xiaozhi-linux](http://github.com/100askTeam/xiaozhi-linux) 百问科技提供的 Linux 客户端
- [78/xiaozhi-sf32](https://github.com/78/xiaozhi-sf32) 思澈科技的蓝牙芯片固件
- [QuecPython/solution-xiaozhiAI](https://github.com/QuecPython/solution-xiaozhiAI) 移远提供的 QuecPython 固件

## 关于项目

这是一个由虾哥开源的 ESP32 项目，以 MIT 许可证发布，允许任何人免费使用，修改或用于商业用途。

我们希望通过这个项目，能够帮助大家了解 AI 硬件开发，将当下飞速发展的大语言模型应用到实际的硬件设备中。

如果你有任何想法或建议，请随时提出 Issues 或加入 [Discord](https://discord.gg/C759fGMBcZ) 或 QQ 群：1011329060

## Star History

<a href="https://star-history.com/#78/xiaozhi-esp32&Date">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/svg?repos=78/xiaozhi-esp32&type=Date&theme=dark" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/svg?repos=78/xiaozhi-esp32&type=Date" />
   <img alt="Star History Chart" src="https://api.star-history.com/svg?repos=78/xiaozhi-esp32&type=Date" />
 </picture>
</a>
