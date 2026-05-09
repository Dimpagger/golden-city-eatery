# 肉夹馍传奇 (Roujiamo Legend)

> 一款 2D 模拟经营游戏，玩家扮演厨师制作肉夹馍、凉皮和烤串，服务顾客赚取金币。
> 7 天战役 + 无尽模式，支持中文 / English / 日本語。
>
> **约 6500 行 C++17 / Raylib 代码** — 适合游戏开发初学者学习完整的 2D 游戏制作流程。

---

## 目录

- [快速开始](#快速开始)
- [游戏功能](#游戏功能)
- [完整操作说明](#完整操作说明)
- [游戏机制详解](#游戏机制详解)
- [架构设计](#架构设计)
- [项目结构](#项目结构)
- [核心类说明](#核心类说明)
- [构建系统](#构建系统)
- [CI / 多平台发布](#ci--多平台发布)
- [初学者学习路径](#初学者学习路径)
- [技术栈与选型理由](#技术栈与选型理由)

---

## 快速开始

```bash
# 首次使用 — 拉取依赖并编译 raylib
make setup

# 编译、运行
make

# 开发模式（增量编译，不 clean）
make bin/app && make execute
```

**依赖**: clang++（macOS）或 g++（Linux/Windows），git，make。
详细平台依赖请参考 [docs/InstallingDependencies.md](docs/InstallingDependencies.md)。

---

## 游戏功能

### 核心玩法

| 系统 | 说明 |
|------|------|
| **4 个工位** | 烤肉架 → 砧板 → 组装台 → 出餐口 |
| **3 种食谱** | 肉夹馍（10g）、凉皮（6g）、烤串（8g），制作路径各不相同 |
| **3 类顾客** | 普通（1.0x）、急躁（1.5x 低耐心）、VIP（2.5x 高耐心） |
| **食谱匹配** | 顾客有随机食谱需求，匹配得全额奖励，错配只得一半 |
| **4 种升级** | 烤肉速度 / 砧板速度 / 组装速度 / 厨师移速（1/2/3/4 键） |
| **连击系统** | 连续服务每 5 次触发里程碑，获得额外金币 |
| **7 天战役** | 每日目标递增（5→35），难度逐渐加大 |
| **无尽模式** | 通关后继续，天数无限递增，难度持续上升 |
| **三语言切换** | 中文 / English / 日本語，按 L 键切换 |

### Juice / 打磨

| 效果 | 触发条件 |
|------|----------|
| **屏幕震动** | 里程碑达成、顾客流失、游戏结束 |
| **粒子特效** | 服务成功（金色 burst）、里程碑（橙色大 burst） |
| **浮动文字** | 金币收益 `+X`、丢失顾客 `!`、自动补肉 `+Raw` |
| **食谱发现提示** | 首次制作凉皮/烤串时弹出「New Recipe!」 |
| **食物视觉指示** | 工位内彩色圆点表示食物状态（红=生/棕=熟/绿=完成） |
| **厨师朝向** | 根据移动方向显示面向（白色眼睛指示） |
| **游戏结束闪红** | 流失 3 名顾客后红色遮罩 + 震动 |
| **程序生成音效** | 5 种音效由正弦波合成，无需外部资源文件：放食物（上扫音）、匹配上菜（清脆 ding）、错配上菜（下滑音）、里程碑（C5-E5-G5 三连音）、购买升级（上扫音） |

### 辅助功能

| 功能 | 按键 |
|------|------|
| **食谱参考面板** | Tab — 显示三条制作路径、顾客类型说明 |
| **游戏指南** | H — 全屏帮助面板，含操作流程、食谱、顾客、提示 |
| **暂停** | P — 暂停/继续 |
| **新纪录提示** | 首次游玩即可见，打破纪录时高亮显示 |
| **鼠标操作** | 点击工位直接交互，与键盘等价 |
| **统计面板** | 游戏结束/通关画面显示食谱分布（RM/LP/KB 各制作数） |

---

## 完整操作说明

```
移动          A / D 键
交互          空格键 / 鼠标点击工位
升级          1=烤肉速度  2=砧板速度  3=组装速度  4=厨师移速
暂停          P 键
语言切换      L 键
食谱参考      Tab 键
游戏指南      H 键
```

---

## 游戏机制详解

### 食物状态机与食谱路径

```
                  RAW（生肉）
                 /          \
            [Grill]      [Cutting]
              /               \
        COOKED（熟肉）      SLICED（已切）
         /      \            /        \
   [Cutting]  [Assembly]  [Assembly]
       /          \          /
  SLICED      ASSEMBLED  ASSEMBLED
     |         (Kebab)     |
 [Assembly]             (Roujiamo/Liangpi)
     |
 ASSEMBLED
 (Roujiamo/Liangpi)
```

| 食谱 | 路径 | 奖励 |
|------|------|------|
| **肉夹馍** RM | Raw → Grill → Cutting → Assembly | 10g |
| **凉皮** LP | Raw → Cutting → Assembly | 6g |
| **烤串** KB | Raw → Grill → Assembly | 8g |

食谱在 Cutting 或 Assembly 阶段自动判定 — 玩家不需要手动选择，只需将食物送到正确的工位序列。

### 顾客系统

```
ENTERING → WAITING → LEAVING
              ↓
          (被服务)
              ↓
          served=true → LEAVING（绿色）
          
          (耐心耗尽)
              ↓
          served=false → LEAVING（红色）→ lostCustomers++
```

- 最多 3 人同时排队
- 生成间隔随游戏时间缩短（难度递增）
- 每日 multiplier 影响生成速度和耐心值
- 丢 3 个顾客 = 游戏结束

### 难度曲线

```
dayMultiplier = 1.0 - (day - 1) × 0.04

Day 1: 1.00    耐心 ~30s    目标 5 人
Day 4: 0.88    耐心 ~26s    目标 20 人
Day 7: 0.76    耐心 ~23s    目标 35 人
```

游戏内时间也会增加难度（300 秒达到峰值）：生成间隔缩短到 3-6s，耐心降到最低 15s。

### 升级数值

| 等级 | 成本 | 工位时间倍率 | 厨师速度倍率 |
|------|------|-------------|-------------|
| Lv.0 | — | 1.0× | 1.0× |
| Lv.1 | 30g | 0.8× | 1.5× |
| Lv.2 | 60g | 0.6× | 2.0× |
| Lv.3 | 120g | 0.4× | 2.5× |
| Lv.4 | 240g | 0.2× | 3.0× |

最高级工位只需 1/5 时间（烤肉 5s→1s），厨师速度从 300→900 px/s。

---

## 架构设计

### 状态机

```
[MENU] ──Space──→ [PLAYING] ──目标达成──→ [DAY_COMPLETE] ──Space──→ [PLAYING]
   ↑                  │                        │
   │                  ├──3客流失──→ [GAME_OVER] ──Space──→ [PLAYING]
   │                  │                        │
   └──Space── [VICTORY] ←──7天通关──┘          │
                    │                          │
                    └──Space──→ [PLAYING] (无尽模式, day 8+)
```

`Game` 类是顶层编排器，拥有所有子系统并通过 `Update()` / `Draw()` 驱动每一帧。

### 类关系

```
Game（主状态机 + 帧循环）
 ├── Chef          玩家角色，持有 unique_ptr<Food>
 ├── Station[4]    工位数组，各自持有 unique_ptr<Food>
 ├── CustomerManager  顾客生成/排队/移除
 │    └── Customer  单个顾客（状态机 + 食谱需求）
 ├── UpgradeSystem  升级等级追踪 + 购买逻辑
 ├── UI            全静态渲染类（持有 Font）
 └── Input         静态键盘/鼠标封装
```

### 数据流（核心交互）

```
1. 玩家按 Space / 点击工位
2. Game::GetNearbyStation() — 碰撞检测（60% 缩小判定区）
3. 若厨师持食物 & 工位 CanAccept() → PlaceFood + StartWork
4. 若厨师空手 & 工位 IsDone() → TakeFood（食物状态自动推进）
5. 出餐口 DONE → ServeFirstWaiting() → 食谱匹配判定 → 金币/分数/连击更新
6. 粒子特效 + 浮动文字 + 屏幕震动（根据事件）
```

### 设计模式

| 模式 | 应用 | 说明 |
|------|------|------|
| **状态机** | Game、Station、Food、Customer | 每个核心实体都有明确的状态转换 |
| **单例风格** | UI、Input | 全静态方法 + 静态成员，避免实例化开销 |
| **组合** | Game 拥有所有子系统 | 清晰的单一所有者，无全局变量 |
| **unique_ptr 所有权** | Chef::heldFood、Station::food | 食物对象独占所有权，通过 std::move 转移 |
| **工厂** | CustomerManager::RollCustomerType/RollRecipeType | 随机生成带权重的顾客/食谱 |
| **数据驱动** | Constants.h、Localization.cpp | 所有数值和文本集中管理 |

---

## 项目结构

```
golden-city/
├── src/                         # 源代码（约 6500 行）
│   ├── main.cpp                 # 窗口创建、字体加载、音频初始化、游戏循环
│   ├── Game.h / .cpp            # 顶层状态机 + 帧循环 + 粒子系统（~420 行）
│   ├── Constants.h              # 所有数值常量、布局坐标、键位绑定（~115 行）
│   ├── GameTypes.h              # 共享枚举类型（Station/Food/Customer/Upgrade）
│   ├── Chef.h / .cpp            # 玩家角色：移动、持物、朝向
│   ├── Station.h / .cpp         # 工位状态机 + 食物处理
│   ├── Food.h / .cpp            # 食物状态机 + 食谱判定
│   ├── Customer.h / .cpp        # 单个顾客：进入/等待/离开状态机
│   ├── CustomerManager.h / .cpp # 顾客生成、队列管理、难度曲线
│   ├── UpgradeSystem.h / .cpp   # 4 种升级的等级追踪与购买逻辑
│   ├── Input.h / .cpp           # 键盘 + 鼠标输入静态封装
│   ├── UI.h / .cpp              # 全静态渲染类（~550 行）
│   └── Localization.h / .cpp    # 三语言翻译系统（二分查找，零堆分配）
│
├── vendor/                      # Git submodules
│   ├── raylib/                  # raylib v4.5.0（图形/输入/音频库）
│   └── raylib-cpp/              # raylib 的 C++ 封装（头文件）
│
├── assets/
│   └── fonts/                   # CJK 字体文件和字符表（用于嵌入）
│
├── include/                     # 编译生成 — raylib/raylib-cpp 头文件
├── lib/                         # 编译生成 — libraylib.a 静态库
├── bin/                         # 编译生成 — .o / .d / 可执行文件
│
├── .github/workflows/           # CI — 4 平台自动构建
├── docs/                        # 平台依赖安装说明
├── Makefile                     # 构建系统（~155 行）
├── CLAUDE.md                    # Claude Code 项目指引
└── README.md                    # 本文档
```

### 文件依赖关系

```
main.cpp
  ├── Game.h ──────── 一切的总入口
  │    ├── Chef.h ─────── Food.h, GameTypes.h
  │    ├── Station.h ──── Food.h, GameTypes.h
  │    ├── CustomerManager.h ── Customer.h ── GameTypes.h
  │    ├── UpgradeSystem.h ───── GameTypes.h
  │    ├── UI.h ──────────────── Station.h, Customer.h, Chef.h, UpgradeSystem.h
  │    └── Input.h
  ├── Localization.h
  └── Constants.h → GameTypes.h
```

不存在循环依赖。`GameTypes.h` 是最底层（只含枚举，无其他依赖）。`Game.h` 是最顶层。

---

## 核心类说明

### Game（`src/Game.h`）

游戏的主控制器。拥有所有子系统实例，管理顶层状态机（MENU/PLAYING/DAY_COMPLETE/GAME_OVER/VICTORY）。

关键方法：
- `Update(float dt)` — 每帧调用，分发到当前状态的处理逻辑
- `Draw()` — 渲染当前状态的画面（含屏幕震动 Camera2D）
- `UpdatePlaying(float dt)` — 游戏主逻辑：实体更新、交互、服务、连击
- `GetNearbyStation()` — 碰撞检测（缩小至 60% 判定区）
- `SpawnParticles(x, y, color, count)` — 粒子特效生成

### Chef（`src/Chef.h`）

玩家角色。32×32 蓝色矩形。持有 `unique_ptr<Food>`。

- A/D 键控制左右移动，边界限制在 0~SCREEN_WIDTH
- `facingRight` — 记录朝向，绘制时显示白色「眼睛」
- `ResetPosition(x, y)` — 重设位置和速度（避免临时对象构造）

### Station（`src/Station.h`）

工位。状态机 `IDLE → WORKING → DONE`。

- `CanAccept(Food)` — 根据工位类型和食物状态判定是否可接收
- `PlaceFood()` + `StartWork()` — 放入食物即自动开始工作
- `Update(dt)` — 推进进度条，完成时调用 `Food::ProcessedAt()` 推进食物状态
- 进度条和倒计时由 UI 层渲染

### Food（`src/Food.h`）

食物实体。状态机 `RAW → COOKED → SLICED → ASSEMBLED`。

- `ProcessedAt(StationType)` — 根据工位推进状态并自动判定食谱类型
- 食谱判定逻辑：Cutting 时 RAW→凉皮，COOKED→肉夹馍；Assembly 时 COOKED→烤串
- `GetReward()` — 返回当前食谱的金币价值

### Customer（`src/Customer.h`）

单个顾客。状态机 `ENTERING → WAITING → LEAVING`。

- 从屏幕右侧移入，到达队列位置后开始等待
- 耐心 timer 归零后自动离开（未服务则计为流失）
- `desiredRecipe` — 随机分配的食谱需求（RM/LP/KB）
- 头顶显示：类型标识（!/$）、食谱缩写（RM/LP/KB）、面部表情（:)/:\|/:\|/:(）

### CustomerManager（`src/CustomerManager.h`）

顾客系统的管理器。

- `ResetForDay(dayMult)` — 每天开始时重置，应用当日难度倍率
- `Update(dt)` — 生成新顾客、更新等待 timer、移除离开的顾客
- `RollCustomerType()` — 加权随机：60% Normal / 25% Impatient / 15% VIP
- `RollRecipeType()` — 只在玩家已解锁的食谱中随机（游戏开始只有肉夹馍，首次制作凉皮/烤串后解锁对应顾客）
- `SetAvailableRecipes(liangpi, kebab)` — 由 Game 每帧同步解锁状态
- `GetDifficulty()` — 游戏内时间 / 300s，线性递增到 1.0
- 移除顾客时重新排列队列位置（`MoveTo`）

### UpgradeSystem（`src/UpgradeSystem.h`）

升级系统。4 种升级类型，每级最高 Lv.4。

- 成本公式：`30 × 2^(level-1)` → 30/60/120/240/480
- 工位速度倍率：`1.0 - 0.2 × level`（最高 0.2×，即 5 倍速）
- 厨师速度倍率：`1.0 + 0.5 × level`（最高 3.0×）
- `Purchase(type, coins)` — 扣除金币，升级成功返回 true

### UI（`src/UI.h`）

全静态渲染类。持有 `static Font s_font` 通过 `SetFont()` 初始化。

涵盖的绘制功能：
- **全屏画面**：菜单、游戏结束、日完成、通关、暂停
- **HUD 顶栏**：天数/进度、金币、最高分、流失数、总分、队列人数、连击
- **游戏实体**：工位（含食物圆点）、顾客（含食谱/表情）、厨师（含朝向）
- **面板**：升级面板（4 行）、食谱参考（Tab）、游戏指南（H）
- **特效**：浮动文字（含特殊类型）、里程碑、发现消息、粒子、闪红

### Localization（`src/Localization.cpp`）

三语言翻译系统。

- 翻译表 `kTranslations[]` 按 key 排序，共 85+ 条目
- `Loc::T(key)` — 直接在排序数组上二分查找（`std::strcmp`），零堆分配
- `ToggleLanguage()` — 循环切换 ZH→JA→EN
- `GetRequiredCodepoints()` — 扫描所有翻译文本的 UTF-8 码位，用于字体子集化

**学习点**：这展示了如何用静态数组 + 二分查找替代 `std::map`/`std::unordered_map`，在游戏场景中消除不必要的堆分配和启动开销。

---

## 构建系统

### Makefile 设计要点

```makefile
# 递归通配符 — 自动发现 src/ 下所有 .cpp
rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# 自动生成 .d 依赖文件
$(buildDir)/%.o: src/%.cpp Makefile
    $(CXX) -MMD -MP -c $(compileFlags) $< -o $@ $(CXXFLAGS)

# 多平台自动检测
ifeq ($(OS), Windows_NT) ... endif
UNAMEOS := $(shell uname)  # Linux / Darwin
```

关键特性：
- **自动源文件发现**：无需手动列出 .cpp 文件
- **自动依赖追踪**：`-MMD -MP` 生成 .d 文件，修改头文件自动重编译
- **增量编译**：`make` 默认不 clean（已从 all 目标移除）
- **编译优化**：`-O2 -Wall -Wextra`
- **Vendor 警告屏蔽**：`-isystem include` 代替 `-I include`，vendor 头文件的警告自动抑制，不影响项目源码的 warning 检查
- **跨平台链接**：Windows（mingw）/ macOS / Linux 自动切换链接参数

### 字体嵌入（可选）

```bash
# 1. 放置 .ttf 字体到 assets/fonts/
# 2. 安装 fonttools: pip install fonttools
# 3. 子集化 + 嵌入
make setup-font
# 4. 正常编译 — 字体内嵌到二进制
make bin/app
```

原理：`pyftsubset` 只保留游戏中实际用到的字符 → `xxd -i` 转换为 C 字节数组 → `src/FontData.h` → `__has_include` 条件编译加载。

---

## CI / 多平台发布

GitHub Actions（`.github/workflows/build.yml`）在每次 push 到 main 时构建 3 个平台：

| 平台 | Runner | 产物 |
|------|--------|------|
| macOS ARM64 | `macos-latest` | `RoujiamoLegend-macOS-arm64` |
| Windows x86_64 | `windows-latest` | `RoujiamoLegend-Windows-x86_64` |
| Linux x86_64 | `ubuntu-latest` | `RoujiamoLegend-Linux-x86_64` |

`src/FontData.h`（内嵌 CJK 字体，~290KB）已提交到 git，CI 构建无需 `pyftsubset`。

---

## 初学者学习路径

这个项目适合按以下顺序阅读，从简单到复杂：

### 第 1 步：理解类型系统

1. **`src/GameTypes.h`**（17 行）— 所有枚举类型，无任何依赖。理解游戏的「词汇表」。
2. **`src/Constants.h`**（115 行）— 所有数值、布局、键位集中管理。学习「调参不改代码」的数据驱动思想。

### 第 2 步：理解单个实体

3. **`src/Food.h` + `src/Food.cpp`**（55 行）— 最简单的状态机。学习 `enum class` 做状态、`switch` 做状态转移。
4. **`src/Chef.h` + `src/Chef.cpp`**（65 行）— 玩家实体。学习 `unique_ptr` 所有权、`ResetPosition` 避免临时对象。
5. **`src/Customer.h` + `src/Customer.cpp`**（80 行）— 三状态机 + 平滑移动。学习用 `dt` 做帧无关移动、耐心计时器。

### 第 3 步：理解管理器

6. **`src/Station.h` + `src/Station.cpp`**（75 行）— 状态机 + 进度条。学习进度累加 `progress += (100.0f / workTime) * dt`。
7. **`src/UpgradeSystem.h` + `src/UpgradeSystem.cpp`**（65 行）— 升级成本公式、倍率计算。学习指数增长 `1 << (n-1)`。
8. **`src/CustomerManager.h` + `src/CustomerManager.cpp`**（105 行）— 顾客生成、队列管理、难度曲线。学习加权随机、线性插值。

### 第 4 步：理解系统集成

9. **`src/Game.h` + `src/Game.cpp`**（420 行）— 顶层状态机、交互逻辑、服务匹配、粒子系统。学习如何把所有子系统串联成一个完整游戏。
10. **`src/UI.h` + `src/UI.cpp`**（550 行）— 全静态渲染类。学习函数分层（DrawPanel → DrawStation → DrawUpgradeRow）、特效（粒子/震动/浮动文字）。

### 第 5 步：理解基础设施

11. **`src/Localization.cpp`**（155 行）— 翻译系统。学习**用排序数组 + 二分查找替代 map**，以及 UTF-8 解码。
12. **`src/Input.h` + `src/Input.cpp`**（30 行）— 输入封装。学习薄封装层的作用（方便未来替换输入后端或添加按键重映射）。
13. **`src/main.cpp`**（85 行）— 程序入口。学习窗口创建、字体加载（含 `__has_include` 条件编译）、音频初始化、游戏循环。
14. **`Makefile`**（155 行）— 跨平台构建。学习 GNU Make 的通配符、自动依赖、平台检测。

### 值得注意的 C++ 技巧

| 技巧 | 位置 | 说明 |
|------|------|------|
| `enum class` | `GameTypes.h` | 类型安全的枚举，避免隐式转换 |
| `unique_ptr` 所有权 | `Chef.h`, `Station.h` | 食物对象独占所有权，`std::move` 转移 |
| `static_cast<int>(Enum)` | `Game.cpp` | 枚举到数组索引的安全转换 |
| 二分查找替代 map | `Localization.cpp` | 排序数组 + `strcmp`，零堆分配 |
| `__has_include` 条件编译 | `main.cpp` | 有内嵌字体就加载，没有就回退系统字体 |
| `-MMD -MP` 自动依赖 | `Makefile` | 修改头文件自动触发重新编译 |
| `std::vector` 遍历删除 | `Game.cpp`, `CustomerManager.cpp` | `it = vec.erase(it)` 模式 |
| Camera2D 屏幕震动 | `Game.cpp:Draw()` | 随机偏移 + 时间衰减 |
| dt 帧无关计算 | 全部 `Update()` | `x += speed * dt` 确保不同帧率一致 |

---

## 技术栈与选型理由

| 技术 | 用途 | 选型理由 |
|------|------|----------|
| **C++17** | 语言 | `constexpr`、`if constexpr`、结构化绑定、`__has_include` |
| **Raylib v4.5.0** | 图形/输入/音频 | 单头文件、无外部依赖、内置 UI 绘图函数 |
| **raylib-cpp** | C++ 封装 | 头文件级封装，无额外编译开销 |
| **GNU Make** | 构建 | 跨平台、零配置增量编译、自动依赖追踪 |
| **Git Submodules** | 依赖管理 | 锁定 raylib 版本，无需包管理器 |
| **pyftsubset** | 字体子集化 | 将 CJK 字体从 15MB 压缩到 ~300KB |
| **GitHub Actions** | CI/CD | 4 平台自动构建，每次 push 发布产物 |

### 为什么不用 CMake？

项目早期使用了 CMake + vcpkg，后来切换到纯 Makefile。理由：
- 这是一个单 target 的简单项目，CMake 的配置量远超实际收益
- Makefile 更透明，初学者能逐行理解构建过程
- raylib 官方提供 Makefile 构建，直接复用

---

## 参考资源

- [Raylib 官网](https://www.raylib.com/)
- [Raylib Cheatsheet](https://www.raylib.com/cheatsheet/cheatsheet.html) — 所有 API 速查
- [raylib-cpp](https://github.com/robloach/raylib-cpp) — C++ 封装头文件
- [GNU Make Manual](https://www.gnu.org/software/make/manual/)

---

*项目基于 zlib/libpng 协议开源，详见 [LICENCE](LICENCE)。*
