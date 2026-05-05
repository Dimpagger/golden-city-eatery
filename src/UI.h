#pragma once

#include <raylib-cpp.hpp>
#include "Station.h"
#include "Customer.h"
#include "Chef.h"
#include "UpgradeSystem.h"

class UI {
public:
    static void SetFont(Font font);

    // ── Full Screens ──────────────────────────────────
    static void DrawMenuScreen();
    static void DrawGameOverScreen(int score, int coins, int lost, int bestScore, int day,
                                   int rjm, int lp, int kb);
    static void DrawDayCompleteScreen(int day, int score, int coins);
    static void DrawVictoryScreen(int score, int coins, int bestScore,
                                  int rjm, int lp, int kb);
    static void DrawPauseOverlay();

    // ── HUD ───────────────────────────────────────────
    static void DrawTopBar(int coins, int score, int queueCount,
                           int lostCount, int maxLost, int bestScore, int streak,
                           int currentDay, int servedToday);

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
    static void DrawFloatText(float x, float y, float alpha, int value);
    static void DrawMilestone(int streak, float timer);
    static void DrawGameOverFlash(float alpha);
    static void DrawRecipePanel();
    static void DrawHelpPanel();
    static void DrawDiscoveryMessage(RecipeType recipe, float timer);

private:
    // ── Helpers ───────────────────────────────────────
    static void DrawPanel(float x, float y, float w, float h, Color bg);
    static int TextWidth(const char* text, int size);
    static const char* FoodStateLabel(FoodState state);
    static const char* CustomerFace(const Customer& c);
    static void DrawUpgradeRow(int ux, int uy, int uw, const char* detailText,
                               const UpgradeSystem& upg, UpgradeType type, int coins);

    static Font s_font;
};
