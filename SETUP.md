# Golden City Eatery - 开发环境配置指南

本文档详细介绍如何在本地配置开发环境，支持 macOS 和 Windows 平台。

## 环境要求

| 工具 | macOS 要求 | Windows 要求 |
|------|------------|--------------|
| 操作系统 | macOS 12.0+ | Windows 10/11 |
| CMake | 3.16+ | 3.16+ |
| C++ 编译器 | Apple Clang (Xcode) | MSVC 或 MinGW |
| Git | - | - |

---

## 快速开始 (macOS)

```bash
# 1. 设置环境变量
export VCPKG_ROOT=~/Documents/Code/vcpkg

# 2. 进入项目目录
cd ~/Documents/Code/GoldenCityEatery

# 3. 编译运行
mkdir -p build && cd build
cmake ..
cmake --build .
./RoujiamoLegend
```

---

## 完整配置步骤 (macOS)

### 第一步：安装基础工具

#### 1.1 安装 CMake

```bash
# 使用 Homebrew 安装
brew install cmake

# 验证安装
cmake --version
```

#### 1.2 克隆 vcpkg

vcpkg 是 C++ 的包管理器，用于管理项目依赖。

```bash
# 进入代码目录
cd ~/Documents/Code

# 克隆 vcpkg 仓库
git clone https://github.com/microsoft/vcpkg.git

# 进入 vcpkg 目录
cd vcpkg

# 运行引导脚本 (macOS/Linux)
./bootstrap-vcpkg.sh
```

> **注意**: vcpkg 只需要 clone 一次，多个项目可以共享。

---

### 第二步：安装项目依赖

```bash
cd ~/Documents/Code/vcpkg

# 安装 raylib (图形库)
./vcpkg install raylib
```

---

### 第三步：配置环境变量

将环境变量添加到 shell 配置文件中：

```bash
# 添加到 ~/.zshrc
echo 'export VCPKG_ROOT=~/Documents/Code/vcpkg' >> ~/.zshrc

# 重新加载配置
source ~/.zshrc
```

验证配置：

```bash
echo $VCPKG_ROOT
# 输出: /Users/你的用户名/Documents/Code/vcpkg
```

---

### 第四步：编译项目

```bash
# 进入项目目录
cd ~/Documents/Code/GoldenCityEatery

# 创建构建目录
mkdir -p build
cd build

# 配置项目
cmake ..

# 编译
cmake --build .
```

---

### 第五步：运行游戏

```bash
cd build
./RoujiamoLegend
```

---

## Windows 配置步骤

### 第一步：安装必要工具

#### 1.1 安装 CMake

从官网下载安装：https://cmake.org/download/

或使用 winget：

```powershell
winget install Kitware.CMake
```

#### 1.2 安装 Git

从官网下载：https://git-scm.com/

或使用 winget：

```powershell
winget install Git.Git
```

#### 1.3 安装 Visual Studio (推荐) 或 MinGW

**方案 A: Visual Studio (推荐)**
- 下载 Visual Studio 2022: https://visualstudio.microsoft.com/
- 安装时选择「使用 C++ 的桌面开发」 workload

**方案 B: MinGW-w64**
- 下载 MSYS2: https://www.msys2.org/
- 运行 `pacman -S mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-gcc`

---

### 第二步：克隆 vcpkg

```powershell
# 进入代码目录
cd $env:USERPROFILE\Documents\Code

# 克隆 vcpkg
git clone https://github.com/microsoft/vcpkg.git

# 进入 vcpkg 目录
cd vcpkg

# 运行引导脚本 (Windows)
.\bootstrap-vcpkg.bat
```

---

### 第三步：安装项目依赖

```powershell
# 安装 raylib
.\vcpkg install raylib
```

---

### 第四步：配置环境变量

```powershell
# 设置环境变量 (PowerShell)
$env:VCPKG_ROOT = "$env:USERPROFILE\Documents\Code\vcpkg"

# 验证
echo $env:VCPKG_ROOT
```

**永久设置** (PowerShell 管理员)：

```powershell
[System.Environment]::SetEnvironmentVariable("VCPKG_ROOT", "$env:USERPROFILE\Documents\Code\vcpkg", "User")
```

---

### 第五步：编译项目

**使用 Visual Studio (MSVC)**:

```powershell
# 进入项目目录
cd $env:USERPROFILE\Documents\Code\GoldenCityEatery

# 创建构建目录
mkdir build
cd build

# 配置 (x64 Native Tools Command Prompt 中运行)
cmake .. -G "Visual Studio 17 2022" -A x64

# 编译
cmake --build .
```

**使用 MinGW**:

```powershell
# 配置
cmake .. -G "MinGW Makefiles"

# 编译
cmake --build .
```

---

### 第六步：运行游戏

```powershell
cd build
.\RoujiamoLegend.exe
```

---

## Windows 平台注意事项

### 1. vcpkg_triplet 配置

Windows 上需要指定正确的 triplet：

```powershell
# 安装特定平台的 raylib
.\vcpkg install raylib:x64-windows
# 或
.\vcpkg install raylib:x64-mingw-dynamic
```

### 2. CMake Generator 选择

| 编译器 | CMake Generator |
|--------|-----------------|
| Visual Studio 2022 | "Visual Studio 17 2022" |
| Visual Studio 2019 | "Visual Studio 16 2019" |
| MinGW | "MinGW Makefiles" |

### 3. 链接库差异

Windows 上不需要 macOS 的框架 (Cocoa, OpenGL 等)，CMakeLists.txt 会自动处理。

### 4. 路径问题

Windows 上路径使用反斜杠 `\` 或正斜杠 `/` 都可以，CMake 会自动处理。

### 5. 中文路径问题

建议项目路径不要包含中文或特殊字符：

```
❌ C:\用户\用户名\文档\项目
✅ C:\Projects\GoldenCityEatery
```

---

## 常见问题 (跨平台)

### Q1: CMake 报错 "Please set VCPKG_ROOT environment variable"

**解决**:
```bash
# macOS/Linux
export VCPKG_ROOT=~/Documents/Code/vcpkg

# Windows PowerShell
$env:VCPKG_ROOT = "$env:USERPROFILE\Documents\Code\vcpkg"
```

### Q2: 编译报错 "Could not find a package configuration file provided by raylib"

**解决**:
```bash
cd $VCPKG_ROOT
./vcpkg install raylib
```

### Q3: 链接错误 "undefined symbol"

**解决**:
```bash
# 清理构建目录重新编译
cd build
rm -rf *
cmake ..
cmake --build .
```

### Q4: Windows 上找不到 vcpkg.cmake

**解决**:
确保环境变量 `VCPKG_ROOT` 已正确设置，并且路径不包含中文。

---

## 团队协作说明

### 环境变量说明

| 环境变量 | 说明 | 示例 |
|----------|------|------|
| `VCPKG_ROOT` | vcpkg 安装目录 | `~/Documents/Code/vcpkg` (macOS)<br>`C:\Projects\vcpkg` (Windows) |

### 开发流程

1. 新成员 clone 项目
2. 按照本文档配置环境
3. 编译验证
4. 开始开发

---

## 项目结构

```
GoldenCityEatery/
├── SETUP.md                    # 本文档
├── ONE_WEEK_PLAN.md            # 开发计划
├── TECHNICAL_DESIGN.md         # 技术方案
├── CMakeLists.txt              # 构建配置
├── vcpkg.json                  # 依赖声明
├── src/
│   └── main.cpp                # 源代码
└── build/                      # 构建输出
    ├── RoujiamoLegend          # macOS/Linux 可执行文件
    └── RoujiamoLegend.exe      # Windows 可执行文件
```

---

*文档版本: 1.2*
*最后更新: 2026-04-30*
