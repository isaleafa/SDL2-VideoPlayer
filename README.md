# 视频转图形化播放器项目

这是一个 C++ 项目，旨在将标准的 MP4 视频文件转换为自定义的二进制数据格式，并使用 SDL2 库在一个独立的图形窗口中进行播放。项目支持两种截然不同的渲染风格：

1.  **像素风格 (Pixel Style)**：将视频的每一帧渲染为彩色的像素块，提供一种清晰、复古的视觉效果。
2.  **字符复古风格 (# Retro Style)**：将视频的每一帧渲染为由带颜色的“#”字符组成的字符画，带来独特的“文明倒退”美学。

---

## 🚀 功能特性

- **跨平台播放**: 基于 SDL2 库，理论上可轻松移植到 Linux 和 macOS。
- **高性能渲染**: 使用 SDL2 的硬件加速渲染，播放流畅。
- **音画同步**: 自动从视频文件中提取并使用精确的帧率信息，保证音画同步。
- **灵活的音频支持**: 自动检测并加载多种音频格式 (`.mp3`, `.m4a`, `.ogg`, `.wav`)。
- **两种渲染模式**: 用户可以选择编译不同版本的播放器，以体验像素或字符画两种视觉风格。
- **自动化预处理**: 提供 Python 脚本，可自动完成视频抽帧、音频提取和格式转换等所有预处理工作。

---

## 📁 项目结构

为了获得最佳体验，建议采用以下文件和目录结构：

```
E:\Code\Cpp\PlayVideoOnTerminal\
│
├── SDL2/                # 存放所有 SDL 开发库的头文件和库文件
│   ├── include/
│   └── lib/
│
├── run/                 # 最终程序的运行环境
│   ├── videoPlay_sdl.exe      # (编译后生成) 像素版可执行文件
│   ├── videoPlay_sdl_ttf.exe  # (编译后生成) 字符版可执行文件
│   ├── frames.data          # (生成后放入) 视频数据
│   ├── audio.mp3            # (生成后放入) 音频文件
│   ├── font.ttf             #  字体文件
│   └── *.dll                #  所有需要的DLL文件
│
├── frame.mp4            # 你的源视频文件
├── audio.m4a            # (可选) 你的源音频文件
│
├── videoPlay_sdl.cpp      # 像素版 C++ 源代码
├── videoPlay_sdl_ttf.cpp  # 字符版 C++ 源代码
│
├── DataGenerator.py     # 用于生成 frames.data 的 Python 脚本
├── m4aTomp3.py          # 用于将 M4A 转换为 MP3 的 Python 脚本
│
├── videoPlay_sdl.bat      # 像素版 Windows 编译脚本
└── videoPlay_sdl_ttf.bat  # 字符版 Windows 编译脚本

```

---

## 🛠️ 环境配置指南

在开始之前，请确保你已经配置好了以下所有环境。

### 1. C++ 编译器

本项目使用 **MinGW-w64 g++** 编译器。请确保你已正确安装并将其添加到了系统环境变量中。推荐使用 64 位 (x86_64) 版本。

### 2. Python 环境

需要安装 Python 3.x 版本，并确保 `pip` 包管理器可用。

- **安装必要的库**:
  ```bash
  pip install opencv-python pydub
  ```

### 3. FFmpeg

这是一个用于音视频转换的命令行工具，是成功转换 M4A 到 MP3 的关键。
当然如果你的视频自带背景音乐，那你也可以不安装 FFmpeg 。
从 ffmpeg.org 下载 release-essentials.zip 版本。

- 解压到一个固定的位置（例如 E:\ffmpeg）。
- 将解压后文件夹里的 bin 目录 (例如 E:\ffmpeg\bin) 添加到系统的 Path 环境变量中。
- 打开一个新的终端，输入 ffmpeg -version，如果能看到版本信息，则代表安装成功。

## 🚀 使用流程

### 第一步：准备媒体文件

将你的视频文件命名为 `frame.mp4` 并放入项目根目录。

**Ps: 如果你做好了这一步但是不想看 2-4 步，你可以直接运行编译脚本文件 `videoPlay_sdl.bat` 或 `videoPlay_sdl_ttf.bat`，它们会自动完成后续步骤。**

✅ 像素版

```bash
.\videoPlay_sdl.bat
```

✅ 字符复古版

```bash
.\videoPlay_sdl_ttf.bat
```

### 第二步：提取和转换音频 (如果需要)

使用`extract_audio.py` 脚本从视频中提取音频，生成 `audio.mp3` 文件。

```bash
python extract_audio.py
```

或者你自带音频文件，但不是 MP3 格式，请运行 `m4aToMp3.py` 脚本，它会自动将 `audio.m4a` 转换为 `audio.mp3`。

```bash
python m4aToMp3.py
```

### 第三步：生成视频数据 (frames.data)

使用 `DataGenerator.py` 脚本生成 `frames.data` 文件。
会自动保存到`run/`目录下。

```bash
python DataGenerator.py
```

### 第四步：编译程序

打开终端，确保你在项目根目录下，然后根据你的选择，运行对应的编译脚本：
✅ 编译【像素版】:

```bash
g++ videoPlay_sdl.cpp -o ./run/videoPlay_sdl -I./SDL2/include -L./SDL2/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer
```

✅ 编译【字符复古版】:

```bash
g++ videoPlay_sdl_ttf.cpp -o ./run/videoPlay_sdl_ttf.exe -I./SDL2/include -L./SDL2/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_ttf
```

### 第五步：运行程序

进入 `run` 目录:

```bash
cd run
```

运行你刚刚编译好的程序:
✅ 运行像素版

```bash
.\videoPlay_sdl.exe
```

✅ 运行字符复古版

```bash
.\videoPlay_sdl_ttf.exe
```

然后，享受你的成果吧！

## 🧩 扩展阅读

爬取视频：

```bash
yt-dlp --no-playlist "https://www.bilibili.com/video/BV1xZ421Y7a1/""
```
