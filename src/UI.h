#pragma once

#include <raylib-cpp.hpp>
#include "Station.h"
#include "Customer.h"
#include "Chef.h"
#include "UpgradeSystem.h"

class UI {
public:
    // ── Full Screens ──────────────────────────────────
    static void DrawMenuScreen();
    static void DrawGameOverScreen(int score, int coins, int lost);
    static void DrawPauseOverlay();

    // ── HUD ───────────────────────────────────────────
    static void DrawTopBar(int coins, int score, int queueCount,
                           int lostCount, int maxLost);

    // ── Game Entities ─────────────────────────────────
    static void DrawStation(const Station& s, const char* label,
                            Color color, float workTime);
    static void DrawCustomer(const Customer& c);
    static void DrawChef(const Chef& chef);
    static void DrawInteractionHint(const Station& s);
    static void DrawCountdown(const Station& s);

    // ── Panels ────────────────────────────────────────
    static void DrawUpgradePanel(const UpgradeSystem& upg, int coins);
    static void DrawFeedbackMessage(const char* text);
    static void DrawControlsHint();

    // ── Effects ───────────────────────────────────────
    static void DrawFloatText(float x, float y, float alpha);
    static void DrawGameOverFlash(float alpha);

private:
    // ── Helpers ───────────────────────────────────────
    static void DrawPanel(float x, float y, float w, float h, Color bg);
    static const char* FoodStateLabel(FoodState state);
    static const char* CustomerFace(const Customer& c);
};
