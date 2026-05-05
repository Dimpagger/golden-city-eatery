#pragma once

#include <raylib-cpp.hpp>
#include <array>

// ── Screen ────────────────────────────────────────────
constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

// ── Player ────────────────────────────────────────────
constexpr float BASE_CHEF_SPEED = 300.0f;   // px/s

// ── Stations ──────────────────────────────────────────
constexpr int   STATION_COUNT = 4;
constexpr float STATION_WIDTH = 100.0f;
constexpr float STATION_HEIGHT = 80.0f;

// Station work times (seconds)
constexpr float BASE_GRILL_TIME = 5.0f;
constexpr float BASE_CUTTING_TIME = 3.0f;
constexpr float BASE_ASSEMBLY_TIME = 2.0f;
constexpr float BASE_SERVING_TIME = 1.0f;

// ── Customers ─────────────────────────────────────────
constexpr float CUSTOMER_PATIENCE = 30.0f;    // s
constexpr float SPAWN_INTERVAL_MIN = 8.0f;    // s
constexpr float SPAWN_INTERVAL_MAX = 15.0f;   // s
constexpr int   MAX_QUEUE = 3;

// Customer types
constexpr float REWARD_MULT_NORMAL = 1.0f;
constexpr float REWARD_MULT_IMPATIENT = 1.5f;
constexpr float REWARD_MULT_VIP = 2.5f;
constexpr float PATIENCE_MULT_NORMAL = 1.0f;
constexpr float PATIENCE_MULT_IMPATIENT = 0.5f;
constexpr float PATIENCE_MULT_VIP = 1.5f;
constexpr int   SPAWN_WEIGHT_NORMAL = 60;
constexpr int   SPAWN_WEIGHT_IMPATIENT = 25;
constexpr int   SPAWN_WEIGHT_VIP = 15;

// ── Difficulty ────────────────────────────────────────
constexpr float DIFFICULTY_RAMP_TIME = 300.0f;     // 5 min to max difficulty
constexpr float SPAWN_INTERVAL_MIN_FAST = 3.0f;
constexpr float SPAWN_INTERVAL_MAX_FAST = 6.0f;
constexpr float PATIENCE_MIN = 12.0f;

// ── Milestones ────────────────────────────────────────
constexpr int MILESTONE_INTERVAL = 5;   // every 5 consecutive serves

// ── Recipes ───────────────────────────────────────────
constexpr int REWARD_ROUJIAMO = 10;
constexpr int REWARD_LIANGPI = 6;
constexpr int REWARD_KEBAB = 8;

// ── Days ──────────────────────────────────────────────
constexpr int TOTAL_DAYS = 7;
constexpr int MAX_LOST = 3;
constexpr float DAY_DIFFICULTY_STEP = 0.06f;  // 6% harder per day
constexpr std::array<int, TOTAL_DAYS> DAY_TARGETS = {5, 10, 15, 20, 25, 30, 35};

// ── Key bindings ──────────────────────────────────────
constexpr int KEY_MOVE_LEFT = KEY_A;
constexpr int KEY_MOVE_RIGHT = KEY_D;
constexpr int KEY_INTERACT = KEY_SPACE;
constexpr int KEY_UPGRADE_FOUR = KEY_FOUR;
constexpr int KEY_RECIPE_GUIDE = KEY_TAB;
constexpr int KEY_HELP = KEY_H;

// ── Layout ────────────────────────────────────────────
constexpr float STATION_Y = 220.0f;
constexpr float STATION_X[STATION_COUNT] = {50.0f, 200.0f, 350.0f, 500.0f};
constexpr float CHEF_X_INIT = 384.0f;
constexpr float CHEF_Y = 285.0f;
constexpr float QUEUE_Y = 350.0f;
constexpr float QUEUE_X[MAX_QUEUE] = {620.0f, 670.0f, 720.0f};

// ── UI Layout ─────────────────────────────────────────
// Panel inset constants for centered panels
constexpr float UI_PANEL_PAD = 6.0f;
constexpr int   UI_FONT_TITLE = 36;
constexpr int   UI_FONT_HEAD  = 24;
constexpr int   UI_FONT_BODY  = 20;
constexpr int   UI_FONT_SMALL = 14;
constexpr int   UI_FONT_TINY  = 12;

// Common panel sizes
constexpr float UI_MENU_PANEL_X = 206.0f;
constexpr float UI_MENU_PANEL_Y = 136.0f;
constexpr float UI_MENU_PANEL_W = 388.0f;
constexpr float UI_MENU_PANEL_H = 348.0f;

constexpr float UI_GAMEOVER_PANEL_X = 206.0f;
constexpr float UI_GAMEOVER_PANEL_Y = 106.0f;
constexpr float UI_GAMEOVER_PANEL_W = 388.0f;
constexpr float UI_GAMEOVER_PANEL_H = 408.0f;

constexpr float UI_DAY_PANEL_X = 206.0f;
constexpr float UI_DAY_PANEL_Y = 156.0f;
constexpr float UI_DAY_PANEL_W = 388.0f;
constexpr float UI_DAY_PANEL_H = 268.0f;

constexpr float UI_VICTORY_PANEL_X = 156.0f;
constexpr float UI_VICTORY_PANEL_Y = 136.0f;
constexpr float UI_VICTORY_PANEL_W = 488.0f;
constexpr float UI_VICTORY_PANEL_H = 348.0f;

// Upgrade panel
constexpr float UI_UPGRADE_PANEL_X = 590.0f;
constexpr float UI_UPGRADE_PANEL_Y = 48.0f;
constexpr float UI_UPGRADE_PANEL_W = 200.0f;
constexpr float UI_UPGRADE_PANEL_H = 90.0f;
