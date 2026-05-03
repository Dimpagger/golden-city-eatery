# 肉夹馍传奇 (Roujiamo Legend)

2D 模拟经营游戏 — 快节奏肉夹馍制作与销售，灵感来源于沙威玛传奇。

## 技术栈

| 技术 | 说明 |
|------|------|
| **语言** | C++17 |
| **图形库** | Raylib + raylib-cpp |
| **构建系统** | Make |

## 快速开始

### 安装依赖

请参考 [docs/InstallingDependencies.md](docs/InstallingDependencies.md) 安装对应平台的依赖。

### 构建项目

**macOS & Linux**
```console
$ make setup
$ make
```

**Windows**
```console
> mingw32-make setup
> mingw32-make
```

### 单独编译（增量构建）

**macOS & Linux**
```console
$ make bin/app; make execute
```

**Windows**
```console
> mingw32-make bin/app && mingw32-make execute
```

## 玩法

### 核心循环

```
顾客到达 → 制作肉夹馍 → 完成订单 → 获得金币 → 购买升级
```

### 4 个工位

1. **烤肉架** — 将生肉烤成熟肉
2. **砧板** — 将烤肉切碎
3. **组装台** — 将肉夹入馍中
4. **交付台** — 将成品交给顾客

### 操作

- **A/D 键**: 左右移动
- **空格键/鼠标点击**: 与工位交互

## 项目结构

```
golden-city-eatery/
├── src/           # 源代码
├── include/       # 头文件 (make setup 生成)
├── lib/           # 静态库 (make setup 生成)
├── bin/           # 构建输出
├── vendor/        # 第三方依赖 (git submodule)
│   ├── raylib/
│   └── raylib-cpp/
├── docs/          # 文档
├── .github/       # CI 配置
├── Makefile
└── README.md
```

## 进度

详见 [ONE_WEEK_PLAN.md](ONE_WEEK_PLAN.md)

## 许可

本项目基于 zlib/libpng 协议开源，详见 [LICENCE](LICENCE)。
