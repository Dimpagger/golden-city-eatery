#pragma once

#include <raylib-cpp.hpp>

// ── Screen ────────────────────────────────────────────
constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

// ── Player ────────────────────────────────────────────
constexpr float PLAYER_SIZE = 32.0f;
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

// ── Game rules ────────────────────────────────────────
constexpr int BASE_REWARD = 10;
constexpr int MAX_LOST = 3;

// ── Key bindings ──────────────────────────────────────
constexpr int KEY_MOVE_LEFT = KEY_A;
constexpr int KEY_MOVE_RIGHT = KEY_D;
constexpr int KEY_INTERACT = KEY_SPACE;

// ── Layout ────────────────────────────────────────────
constexpr float STATION_Y = 220.0f;
constexpr float STATION_X[STATION_COUNT] = {50.0f, 200.0f, 350.0f, 500.0f};
constexpr float CHEF_X_INIT = 384.0f;
constexpr float CHEF_Y = 285.0f;
constexpr float QUEUE_Y = 350.0f;
constexpr float QUEUE_X[MAX_QUEUE] = {620.0f, 670.0f, 720.0f};
