# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

肉夹馍传奇 (Roujiamo Legend) — 2D cooking simulation game built with C++17 + Raylib. Players operate 4 stations (Grill → Cutting → Assembly → Serving) to prepare food and serve customers over 7 days with increasing difficulty.

## Build & Run

```bash
# First time only — pulls git submodules and builds raylib
make setup

# Build, run, and clean
make

# Incremental build (compile only, for development)
make bin/app && make execute

# Clean build artifacts (including embedded font)
make clean
make font-clean         # clean font artifacts only
```

### Font embedding (optional but recommended for distribution)

The game supports embedding a CJK font directly into the binary. Without it, CJK text falls back to system font paths (macOS/Linux/Windows).

```bash
# 1. Place a .ttf font (e.g. Noto Sans SC) in assets/fonts/
#    Recommended: Noto Sans SC Regular (SIL OFL license)

# 2. Install fonttools (one time): pip install fonttools

# 3. Subset font to game characters + embed
make setup-font

# 4. Build normally — font is now embedded
make bin/app
```

How it works:
- `assets/fonts/chars.txt` contains all unique CJK characters from the translation tables
- `pyftsubset` creates a minimal font (~300KB) with only those characters
- `xxd -i` converts the subset .ttf → `src/FontData.h` (C byte array)
- `main.cpp` uses C++17 `__has_include("FontData.h")` — if the header exists, `LoadFontFromMemory()` loads the embedded font; otherwise falls back to system font paths

- **Compiler**: clang++ on macOS (`CXX ?= clang++` in Makefile), g++ on Linux/Windows
- **Standard**: C++17 (`-std=c++17`)
- **Dependencies**: raylib + raylib-cpp (git submodules in `vendor/`, compiled as static lib), fonttools (pip, for font embedding only)
- **Generated dirs**: `include/` (copied headers), `lib/` (static lib), `bin/` (build output) — all from `make setup`
- **No CMake in use** — CMakeLists.txt and vcpkg.json have been deleted; the project uses Makefile only

### Multi-platform CI

GitHub Actions (`.github/workflows/build.yml`) builds for 4 platforms on every push to `main`:

| Platform | Runner | Artifact |
|----------|--------|----------|
| macOS ARM64 | `macos-latest` | `RoujiamoLegend-macOS-arm64` |
| macOS x86_64 | `macos-13` | `RoujiamoLegend-macOS-x86_64` |
| Windows x86_64 | `windows-latest` | `RoujiamoLegend-Windows-x86_64` |
| Linux x86_64 | `ubuntu-latest` | `RoujiamoLegend-Linux-x86_64` |

`src/FontData.h` (embedded font, ~290KB) is committed to git — CI builds include CJK font support without needing `pyftsubset`. When translations change, regenerate locally with `make setup-font` and commit the updated header.

### When translations change

```bash
# 1. Regenerate character list
python3 -c "import sys; t=open('src/Localization.cpp').read(); chars=set(chr(i) for i in range(32,127))|set(c for c in t if ord(c)>127); sys.stdout.write(''.join(sorted(chars)))" > assets/fonts/chars.txt

# 2. Subset + embed (needs font in assets/fonts/ and pip install fonttools)
make setup-font

# 3. Commit the updated header
git add assets/fonts/chars.txt src/FontData.h && git commit -m "Update embedded font"
```

## Architecture

### Game State Machine

`Game` (`src/Game.h/.cpp`) is the orchestrator with a finite state machine:

```
MENU --[Space]--> PLAYING --[target hit]--> DAY_COMPLETE --[Space]--> PLAYING (next day)
                    |                         |
                    |--[3 lost customers]--> GAME_OVER --[Space]--> PLAYING (reset)
                    |--[all 7 days done]--> VICTORY --[Space]--> MENU
```

`Game::Update(float dt)` dispatches by `GameState`, `Game::Draw()` delegates to `UI` static methods for each screen.

### Core Classes & Ownership

- **`Game`** owns everything: `Chef`, `Station[4]`, `CustomerManager`, `UpgradeSystem`
- **`Chef`** — player avatar. Moves via A/D keys, holds a single `std::unique_ptr<Food>`. Speed affected by chef speed upgrade.
- **`Station`** — one of 4 stations (GRILL/CUTTING/ASSEMBLY/SERVING). State machine: `IDLE → WORKING → DONE`. Has a work timer (seconds) and progress (0–100). Work time is modified by upgrades.
- **`Food`** — state machine `RAW → COOKED → SLICED → ASSEMBLED`. The path through stations determines `RecipeType` (Roujiamo, Liangpi, Kebab) which determines reward value (10/6/15 coins).
- **`Customer`** — state machine: `ENTERING → WAITING → LEAVING`. Has patience timer, 3 types (NORMAL/IMPATIENT/VIP) with different reward multipliers (1.0x/1.5x/2.5x). Leaves if patience reaches 0.
- **`CustomerManager`** — spawns customers (max 3 in queue), manages difficulty scaling over time (faster spawns, lower patience), tracks lost customers.
- **`UpgradeSystem`** — 3 upgrade types (GRILL_SPEED, OVEN_SPEED, CHEF_SPEED), max level 4. Cost doubles per level: 30/60/120/240/480. Purchased via keys 1/2/3.
- **`UI`** — all-static class. Holds a `Font` (set once at startup). Handles all drawing: screens, HUD top bar, station/customer/chef rendering, upgrade panel, float text effects, milestone popups.
- **`Input`** — thin static wrapper over raylib `IsKeyDown`/`IsKeyPressed`.
- **`Localization`** (namespace `Loc`) — tri-language support (ZH/EN/JP), toggle with L key. Uses a static translation table and UTF-8 codepoint scanner for font loading.

### Interaction Flow (the core gameplay loop)

1. Player presses Space → `Game::GetNearbyStation()` checks AABB collision with all 4 stations
2. If chef holds food and station `CanAccept()` (checks food state vs station type) → place food, start work
3. If chef empty and station is DONE → take food (state advances)
4. If chef empty, station has food but is IDLE → start work
5. When station[3] (SERVING) has DONE food → auto-serve to first waiting customer in queue → coins, score, streak update

### Food Recipe Paths

```
Raw → Grill → Cooked → Cutting → Sliced (Roujiamo, 10 coins)
Raw → Cutting → Sliced (Liangpi, 6 coins)
Cooked → Assembly → Assembled (Kebab, 15 coins)
Sliced → Assembly → Assembled (Roujiamo/Liangpi, 10/6 coins)
```

`Food::ProcessedAt()` sets both the new `FoodState` and the `RecipeType`. Recipe is locked in at Cutting (differentiates RAW→SLICED vs COOKED→SLICED) or Assembly if not set earlier.

### Constants & Layout

All tuning values in `src/Constants.h`: screen size (800×600), station positions (y=220, x spaced 150px apart), chef start position, queue positions, work times (5/3/2/1s), patience (30s), spawn intervals, day targets (5→30), difficulty ramp (300s).

Key bindings are also in Constants.h: `KEY_A`/`KEY_D` (move), `KEY_SPACE` (interact), `KEY_P` (pause), `KEY_L` (language), `KEY_ONE`/`KEY_TWO`/`KEY_THREE` (upgrades).

### File Organization

```
src/
├── main.cpp              # Window creation, font loading, game loop
├── Game.h/.cpp            # Top-level orchestrator & state machine
├── Constants.h            # All numeric constants, layout, key bindings
├── GameTypes.h            # Shared enums (StationType, FoodState, RecipeType, etc.)
├── Chef.h/.cpp            # Player character
├── Station.h/.cpp         # Work station state machine
├── Food.h/.cpp            # Food state machine + recipe paths
├── Customer.h/.cpp        # Single customer entity
├── CustomerManager.h/.cpp # Customer spawning, queue, difficulty scaling
├── UpgradeSystem.h/.cpp   # Upgrade purchases & speed multipliers
├── Input.h/.cpp           # Static raylib input wrapper
├── UI.h/.cpp              # All drawing & rendering
└── Localization.h/.cpp    # ZH/EN/JP translation system
```

### The Makefile

Uses recursive wildcards to auto-discover all `.cpp` files under `src/`. Each `src/*.cpp` compiles to `bin/*.o` with auto-generated `.d` dependency files. The `make setup` target builds raylib from `vendor/raylib/src` and copies headers from both `vendor/raylib/src` and `vendor/raylib-cpp/include`.

The default target (`all`) compiles, runs the executable, then cleans. Use `make bin/app && make execute` for development to avoid cleaning between runs.
